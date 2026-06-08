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

    public ClientHandler(Socket socket, GameState gameState, List<ClientHandler> clients) {

        this.socket = socket;
        this.gameState = gameState;
        this.clients = clients;


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

            gameState.addObserver(this);


            if (gameState.getPlayerCount() < 2) {
                player = gameState.addPlayer();
                spectator = false;

                out.println("PLAYER_ID " + player.getId());
                System.out.println("Cliente asignado como jugador " + player.getId());


            } else {
                player = null;
                spectator = true;

                out.println("SPECTATOR");
                System.out.println("Cliente asignado como espectador");
            }
            
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

            // Enviar estado inicial al cliente que se conecta
            sendMessage(gameState.getStateMessage());


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
                System.out.println("Espectador desconectado");
            }

        } finally {
            gameState.removeObserver(this);
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