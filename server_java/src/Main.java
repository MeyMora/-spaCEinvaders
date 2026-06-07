import Logica.GameState;
import Patrones.Singlenton.ThreadExecutorSingleton;

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

        ServerSocket serverSocket = null;

        try {
            serverSocket = new ServerSocket(PORT);

            System.out.println("Servidor iniciado en puerto " + PORT);

            while (true) {

                Socket clientSocket = serverSocket.accept();

                System.out.println("Cliente conectado");

                ClientHandler clientHandler =
                        new ClientHandler(clientSocket, gameState, clients);

                clients.add(clientHandler);
            }

        } catch (Exception e) {
            e.printStackTrace();
        } finally {
            ThreadExecutorSingleton.getInstance().shutdown();
            if (serverSocket != null) {
                try {
                    serverSocket.close();
                } catch (Exception e) {
                    System.out.println("No se pudo cerrar el servidor");
                }
            }
        }
    }
}
