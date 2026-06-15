import Logica.GameState;
import Patrones.Observer.GameObserver;
import Patrones.Singlenton.ThreadExecutorSingleton;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.Socket;

public class AdminHandler extends GameObserver implements Runnable {

    private Socket socket;
    private BufferedReader in;
    private PrintWriter out;
    private GameState gameState;

    public AdminHandler(Socket socket, GameState gameState) {
        this.socket = socket;
        this.gameState = gameState;

        try {
            in = new BufferedReader(new InputStreamReader(socket.getInputStream()));
            out = new PrintWriter(socket.getOutputStream(), true);

            gameState.addObserver(this);
            out.println("ADMIN_CONNECTED");
            System.out.println("Administrador conectado");

            ThreadExecutorSingleton.getInstance().execute(this);

        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    @Override
    public void run() {
        try {
            sendMessage(gameState.getStateMessage());

            String message;
            while ((message = in.readLine()) != null) {
                System.out.println("[Admin] " + message);
                processAdminMessage(message);
            }

        } catch (Exception e) {
            System.out.println("Administrador desconectado");

        } finally {
            gameState.removeObserver(this);
            closeConnection();
        }
    }

    private void processAdminMessage(String message) {
        if (message == null || message.trim().isEmpty()) return;

        message = message.trim();
        String[] parts = message.split("\\s+");

        if (parts.length == 5 && parts[0].equals("CREATE") && parts[1].equals("ALIEN")) {
            try {
                int x = Integer.parseInt(parts[2]);
                int y = Integer.parseInt(parts[3]);
                int points = Integer.parseInt(parts[4]);
                boolean created = gameState.createAlien(x, y, points);
                if (created) {
                    System.out.println("Admin: Alien creado en (" + x + ", " + y + ") con " + points + " puntos");
                } else {
                    System.out.println("Admin: No es posible crear el alien — la posición (" + x + ", " + y + ") ya está ocupada");
                    sendMessage("ERROR Posición (" + x + ", " + y + ") ya está ocupada por otro alien");
                }
            } catch (NumberFormatException e) {
                System.out.println("Admin: Parámetros de alien inválidos: " + message);
            } catch (IllegalArgumentException e) {
                System.out.println("Admin: Puntos de alien no válidos — use 10, 20 o 40 (modo clásico) / 20, 40 o 60 (modo difícil)");
                sendMessage("ERROR " + e.getMessage());
            }
        }

        else if (parts.length == 4 && parts[0].equals("CREATE") && parts[1].equals("UFO")) {
            try {
                String direction = parts[2];
                int points = Integer.parseInt(parts[3]);
                gameState.createUFO(0, 0, direction, points);
                System.out.println("Admin: OVNI creado con dirección " + direction + " y " + points + " puntos");
            } catch (NumberFormatException e) {
                System.out.println("Admin: Parámetros de OVNI inválidos: " + message);
            }
        }

        else if (parts.length == 2 && parts[0].equals("SPEED")) {
            try {
                int speed = Integer.parseInt(parts[1]);
                gameState.setAlienSpeed(speed);
                System.out.println("Admin: Velocidad cambiada a " + speed);
            } catch (NumberFormatException e) {
                System.out.println("Admin: Valor de velocidad inválido: " + parts[1]);
            }
        }

        else if (parts.length == 2 && parts[0].equals("BUNKERS")) {
            try {
                int health = Integer.parseInt(parts[1].replace("%", ""));
                gameState.setBunkersHealth(health);
                System.out.println("Admin: Estado de bunkers cambiado a " + health + "%");
            } catch (NumberFormatException e) {
                System.out.println("Admin: Valor de bunker inválido: " + parts[1]);
            }
        }

        else if (parts[0].equals("RESTART")) {
            gameState.restartGame();
            System.out.println("Admin: Juego reiniciado");
            sendMessage("RESTART_OK");
        }

        else {
            System.out.println("Admin: Comando no reconocido: " + message);
        }
    }

    public void sendMessage(String message) {
        out.println(message);
    }

    private void closeConnection() {
        try {
            socket.close();
        } catch (Exception e) {
            System.out.println("No se pudo cerrar la conexión del administrador");
        }
    }

    @Override
    public void update(String gameStateMessage) {
        sendMessage(gameStateMessage);
    }
}
