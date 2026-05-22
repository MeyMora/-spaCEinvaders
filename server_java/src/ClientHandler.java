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

            while ((message = in.readLine()) != null) {

                System.out.println("Cliente " + player.getId() + " dice: " + message);
                processMessage(message);
                broadcast(gameState.getStateMessage());

            }

        } catch (Exception e) {

            System.out.println("Cliente desconectado");

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
        String[] parts = message.split(" ");

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

        else if (parts[0].equals("ALIEN") && parts.length >= 3 && parts[1].equals("KILLED")) {
            int alienId = Integer.parseInt(parts[2]);

            System.out.println("Jugador " + player.getId() + " eliminó al alien " + alienId);

            gameState.killAlien(player.getId(), alienId);
        }

        else if (message.equals("PLAYER_HIT")) {
            gameState.playerHit(player.getId());
            System.out.println("Jugador " + player.getId() + " recibió daño");
        }

        else {
            System.out.println("Comando no reconocido: " + message);
        }
    }
    private void broadcast(String message) {
        for (ClientHandler client : clients) {
            client.sendMessage(message);
        }
    }

    public void sendMessage(String message) {
        out.println(message);
    }
}