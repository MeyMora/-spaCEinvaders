import Logica.GameState;
import Patrones.Singlenton.ThreadExecutorSingleton;

import java.net.ServerSocket;
import java.net.Socket;
import java.util.ArrayList;
import java.util.List;
import java.util.Collections;
import java.util.Random;

public class Main {

    public static final int PORT       = 5000;
    public static final int ADMIN_PORT = 5001;

    // Tiempo entre apariciones del OVNI (milisegundos)
    // 20000 = 20 segundos
    private static final int UFO_INTERVAL_MS = 20000;

    private static GameState gameState = new GameState();
    private static List<ClientHandler> clients =
            Collections.synchronizedList(new ArrayList<>());

    public static void main(String[] args) {

        ServerSocket serverSocket      = null;
        ServerSocket adminServerSocket = null;

        try {
            serverSocket      = new ServerSocket(PORT);
            adminServerSocket = new ServerSocket(ADMIN_PORT);

            System.out.println("Servidor de jugadores iniciado en puerto " + PORT);
            System.out.println("Servidor de administrador iniciado en puerto " + ADMIN_PORT);
            System.out.println("OVNI automatico cada " + (UFO_INTERVAL_MS / 1000) + " segundos");

            // ── Hilo del administrador ──────────────────────────────────────
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

            // ── Hilo del OVNI automatico ────────────────────────────────────
            // Aparece cada UFO_INTERVAL_MS ms con direccion y puntos aleatorios
            Thread ufoThread = new Thread(() -> {
                Random random = new Random();
                while (true) {
                    try {
                        Thread.sleep(UFO_INTERVAL_MS);

                        // Solo lanzar si el juego esta activo
                        if (!gameState.isGameOver()) {
                            // Direccion aleatoria: I-D o D-I
                            String direccion = random.nextBoolean() ? "I-D" : "D-I";

                            // Puntos aleatorios entre 50 y 300, multiplo de 50
                            int puntos = (random.nextInt(6) + 1) * 50;

                            gameState.createUFO(0, 0, direccion, puntos);
                            System.out.println("[OVNI] Aparecio automaticamente: " +
                                               direccion + " | " + puntos + " pts");
                        }
                    } catch (InterruptedException e) {
                        System.out.println("[OVNI] Hilo interrumpido");
                        break;
                    }
                }
            });
            ufoThread.setDaemon(true);
            ufoThread.start();

            // ── Aceptar clientes jugadores ──────────────────────────────────
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
                try { serverSocket.close(); }
                catch (Exception e) { System.out.println("No se pudo cerrar el servidor"); }
            }
            if (adminServerSocket != null) {
                try { adminServerSocket.close(); }
                catch (Exception e) { System.out.println("No se pudo cerrar el servidor admin"); }
            }
        }
    }
}