import Logica.GameState;
import Modelos.Player;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.Socket;
import java.util.List;
import Patrones.Observer.GameObserver;
import Patrones.Singlenton.ThreadExecutorSingleton;

public class ClientHandler extends GameObserver implements Runnable {

    private Socket socket;
    private BufferedReader in;
    private PrintWriter out;
    private GameState gameState;
    private List<ClientHandler> clients;
    private Player player;
    private boolean spectator;

    // Slot reservado (1 o 2) si es jugador; 0 si es espectador o aun no asignado
    private int mySlot;

    public ClientHandler(Socket socket, GameState gameState, List<ClientHandler> clients) {

        this.socket = socket;
        this.gameState = gameState;
        this.clients = clients;
        this.player = null;
        this.spectator = false;
        this.mySlot = 0;

        try {
            in = new BufferedReader(
                    new InputStreamReader(
                            socket.getInputStream()
                    )
            );

            out = new PrintWriter(
                    socket.getOutputStream(),
                    true
            );

            // NO se asigna rol todavia. Se envia la disponibilidad de slots
            // y se espera a que el cliente mande JOIN PLAYER1/PLAYER2/SPECTATOR
            boolean p1Free = gameState.isSlotFree(1);
            boolean p2Free = gameState.isSlotFree(2);
            out.println("SLOTS " + p1Free + " " + p2Free);
            System.out.println("Cliente conectado. Slots libres -> P1:" + p1Free + " P2:" + p2Free);

            // Se ejecuta la tarea usando el Singleton ThreadExecutor
            ThreadExecutorSingleton.getInstance().execute(this);

        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    @Override
    public void run() {

        try {

            String message;

            // ── Esperar el mensaje JOIN antes de hacer cualquier otra cosa ──
            boolean asignado = false;
            while (!asignado && (message = in.readLine()) != null) {

                message = message.trim();
                if (message.isEmpty()) continue;

                String[] parts = message.split("\\s+");

                if (parts[0].equals("JOIN") && parts.length >= 2) {

                    if (parts[1].equals("PLAYER1") || parts[1].equals("PLAYER2")) {
                        int slot = parts[1].equals("PLAYER1") ? 1 : 2;
                        Player asignadoPlayer = gameState.joinAsPlayer(slot);

                        if (asignadoPlayer != null) {
                            player = asignadoPlayer;
                            mySlot = slot;
                            spectator = false;
                            asignado = true;

                            gameState.addObserver(this);
                            out.println("PLAYER_ID " + player.getId());
                            System.out.println("Cliente asignado como jugador " + player.getId());

                        } else {
                            // Slot ocupado: informar y volver a enviar disponibilidad
                            out.println("JOIN_DENIED " + parts[1]);
                            boolean p1Free = gameState.isSlotFree(1);
                            boolean p2Free = gameState.isSlotFree(2);
                            out.println("SLOTS " + p1Free + " " + p2Free);
                            System.out.println("Slot " + parts[1] + " denegado (ya ocupado)");
                        }

                    } else if (parts[1].equals("SPECTATOR")) {
                        player = null;
                        spectator = true;
                        mySlot = 0;
                        asignado = true;

                        gameState.addObserver(this);
                        out.println("SPECTATOR");
                        System.out.println("Cliente asignado como espectador");

                    } else {
                        out.println("JOIN_DENIED UNKNOWN");
                    }

                } else {
                    // Cualquier otro mensaje antes de JOIN se ignora
                    System.out.println("Mensaje ignorado (esperando JOIN): " + message);
                }
            }

            if (!asignado) {
                // El cliente se desconecto antes de unirse
                return;
            }

            // Enviar estado inicial al cliente ya asignado
            sendMessage(gameState.getStateMessage());

            // ── Loop normal de mensajes del juego ───────────────────────
            while ((message = in.readLine()) != null) {

                if (spectator) {
                    System.out.println("Espectador dice: " + message);
                } else {
                    System.out.println("Cliente " + player.getId() + " dice: " + message);
                }
                processMessage(message);

            }

        } catch (Exception e) {
            if (player != null) {
                System.out.println("Cliente " + player.getId() + " desconectado");
            } else {
                System.out.println("Cliente desconectado");
            }

        } finally {
            gameState.removeObserver(this);

            // Liberar el slot de jugador si corresponde
            if (mySlot != 0) {
                gameState.leaveSlot(mySlot);
                System.out.println("Slot " + mySlot + " liberado");
            }

            synchronized (clients) {
                clients.remove(this);
            }
            closeConnection();
        }
    }

    private void processMessage(String message) {
        if (message == null) {
            return;
        }

        message = message.trim();

        if (message.isEmpty()) {
            return;
        }

        System.out.println("Procesando comando: [" + message + "]");

        if (gameState.isGameOver()){
            System.out.println("El juego ya termino. Comando ignorado.");
            return;
        }
        String[] parts = message.split("\\s+");

        if (parts.length == 0) {
            return;
        }
        if (spectator) {
            System.out.println("El espectador no puede ejecutar comandos.");
            return;
        }

        if (parts[0].equals("MOVE") && parts.length >= 2) {

            if (parts[1].equals("LEFT")) {
                System.out.println("Jugador " + player.getId() + " se movió a la izquierda");
                gameState.movePlayerLeft(player.getId());
            }

            else if (parts[1].equals("RIGHT")) {
                System.out.println("Jugador " + player.getId() + " se movió a la derecha");
                gameState.movePlayerRight(player.getId());
            }

            else {
                System.out.println("Dirección no reconocida: " + parts[1]);
            }
        }

        else if (parts[0].equals("SHOOT")) {
            gameState.registerShot(player.getId());
            System.out.println("Jugador " + player.getId() + " disparó");
        }

        else if (
                (parts.length == 3 && parts[0].equals("ALIEN") && parts[1].equals("KILLED")) ||
                        (parts.length == 2 && parts[0].equals("ALIEN_KILLED"))
        ) {
            try {
                int alienId;

                if (parts[0].equals("ALIEN_KILLED")) {
                    alienId = Integer.parseInt(parts[1]);
                } else {
                    alienId = Integer.parseInt(parts[2]);
                }

                boolean killed = gameState.killAlien(player.getId(), alienId);

                if (killed) {
                    System.out.println("Jugador " + player.getId() + " eliminó al alien " + alienId);
                } else {
                    System.out.println("No se pudo eliminar el alien " + alienId);
                }
            } catch (NumberFormatException e) {
                System.out.println("ID de alien inválido: " + message);
            }
        }

        else if (
                message.equals("PLAYER_HIT") ||
                        (parts.length == 2 && parts[0].equals("PLAYER") && parts[1].equals("HIT"))
        ) {
            gameState.playerHit(player.getId());
            System.out.println("Jugador " + player.getId() + " recibió daño");
        }

        else if (parts.length == 3 && parts[0].equals("BUNKER") && parts[1].equals("HIT")) {
            try {
                int bunkerId = Integer.parseInt(parts[2]);
                boolean hit = gameState.hitBunker(bunkerId);
                if (hit) {
                    System.out.println("Bunker " + bunkerId + " recibió daño");
                } else {
                    System.out.println("Bunker " + bunkerId + " no encontrado o ya destruido");
                }
            } catch (NumberFormatException e) {
                System.out.println("ID de bunker inválido: " + message);
            }
        }

        else if (parts[0].equals("ALIENS_REACHED")) {
            gameState.aliensLlegaronBase();
            System.out.println("Jugador " + player.getId() + " reportó que los aliens llegaron al cañón");
        }

        else if (parts.length >= 2 && parts[0].equals("UFO") && parts[1].equals("KILLED")) {
            boolean killed = gameState.destroyUFO(player.getId());

            if (killed) {
                System.out.println("Jugador " + player.getId() + " eliminó el OVNI");
            } else {
                System.out.println("No se pudo eliminar el OVNI");
            }
        }

        else {
            System.out.println("Comando no reconocido: " + message);
        }




    }
    public void sendMessage(String message) {
        out.println(message);
    }

    private void closeConnection() {
        try{
            socket.close();
        } catch (Exception e){
            System.out.println("No se pudo cerrar el socket");


        }
    }

    @Override
    public void update(String gameStateMessage) {
        sendMessage(gameStateMessage);
    }
}
