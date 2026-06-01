import Modelos.GameState;
import Modelos.Player;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.Socket;
import java. util.List;

public class ClientHandler extends Thread {

    private Socket socket;
    private BufferedReader in;
    private PrintWriter out;
    private GameState gameState;
    private List<ClientHandler> clients;
    private Player player;

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
            player = gameState.addPlayer();

            out.println("PLAYER_ID " + player.getId());;

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

                System.out.println("Cliente " + player.getId() + " dice: " + message);
                processMessage(message);
                broadcast(gameState.getStateMessage());

            }

        } catch (Exception e) {

            System.out.println("Cliente" + player.getId() + "desconectado");

        } finally{
            clients.remove(this);
            closeConnection();
        }
    }
    private void processMessage(String message) {
        if (message == null) {
            return;
        }

        message = message.trim();

        System.out.println("Procesando comando: [" + message + "]");

        if (gameState.isGameOver()){
            System.out.println("El juego ya termino. Comando ignorado.");
            return;
        }
        String normalizedMessage = message.toUpperCase().replace("_", " ");
        String[] parts = message.split("\\s+");

        if (parts.length == 0) {
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
        }

        else if (message.equals("PLAYER_HIT")) {
            gameState.playerHit(player.getId());
            System.out.println("Jugador " + player.getId() + " recibió daño");
        }

        else if (parts.length == 2 && parts[0].equals("SPEED")) {
            int newSpeed = Integer.parseInt(parts[1]);
            gameState.setAlienSpeed(newSpeed);

            System.out.println("Velocidad cambiada a " + newSpeed);
        }

        else if (parts.length == 2 && parts[0].equals("BUNKERS")) {
            int health = Integer.parseInt(parts[1]);
            gameState.setBunkersHealth(health);

            System.out.println("Estado de bunkers cambiado a " + health + "%");
        }

        else if (parts.length == 5 && parts[0].equals("CREATE") && parts[1].equals("ALIEN")) {
            int x = Integer.parseInt(parts[2]);
            int y = Integer.parseInt(parts[3]);
            int points = Integer.parseInt(parts[4]);

            gameState.createAlien(x, y, points);

            System.out.println("Alien creado en (" + x + ", " + y + ") con " + points + " puntos");
        }

        else if (parts.length == 4 && parts[0].equals("CREATE") && parts[1].equals("UFO")) {
            String direction = parts[2];
            int points = Integer.parseInt(parts[3]);

            gameState.createUFO(direction, points);

            System.out.println("OVNI creado con direccion " + direction + " y " + points + " puntos");
        }

        else if (message.equals("UFO KILLED")) {
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
    private void broadcast(String message) {
        synchronized (clients) {
            for (ClientHandler client : clients) {
                client.sendMessage(message);
            }
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
}