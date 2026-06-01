import Modelos.GameState;

import java.net.ServerSocket;
import java.net.Socket;
import java.util.ArrayList;
import java.util.List;
import java.util.Collections;

public class Main {

    public static final int PORT = 5000;

    private static GameState gameState = new GameState();
    private static List<ClientHandler> clients =
            Collections.synchronizedList(new ArrayList<>());


    public static void main(String[] args) {

        try {
            ServerSocket serverSocket = new ServerSocket(PORT);

            System.out.println("Servidor iniciado en puerto " + PORT);

            while (true) {

                Socket clientSocket = serverSocket.accept();

                System.out.println("Cliente conectado");

                ClientHandler clientHandler =
                        new ClientHandler(clientSocket, gameState, clients);

                clients.add(clientHandler);
                clientHandler.start();
            }

        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
