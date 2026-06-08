import Logica.GameState;
import Patrones.Singlenton.ThreadExecutorSingleton;

import java.net.ServerSocket;
import java.net.Socket;
import java.util.ArrayList;
import java.util.List;
import java.util.Collections;

public class Main {

    public static final int PORT = 5000;
    public static final int ADMIN_PORT = 5001;

    private static GameState gameState = new GameState();
    private static List<ClientHandler> clients =
            Collections.synchronizedList(new ArrayList<>());

    public static void main(String[] args) {

        ServerSocket serverSocket = null;
        ServerSocket adminServerSocket = null;

        try {
            serverSocket = new ServerSocket(PORT);
            adminServerSocket = new ServerSocket(ADMIN_PORT);

            System.out.println("Servidor de jugadores iniciado en puerto " + PORT);
            System.out.println("Servidor de administrador iniciado en puerto " + ADMIN_PORT);

            // Hilo separado para aceptar conexiones del administrador
            final ServerSocket adminSocket = adminServerSocket;
            Thread adminThread = new Thread(() -> {
                try {
                    while (true) {
                        Socket adminClientSocket = adminSocket.accept();
                        new AdminHandler(adminClientSocket, gameState);
                    }
                } catch (Exception e) {
                    System.out.println("Servidor de administrador cerrado");
                }
            });
            adminThread.setDaemon(true);
            adminThread.start();

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
                    System.out.println("No se pudo cerrar el servidor de jugadores");
                }
            }
            if (adminServerSocket != null) {
                try {
                    adminServerSocket.close();
                } catch (Exception e) {
                    System.out.println("No se pudo cerrar el servidor de administrador");
                }
            }
        }
    }
}
