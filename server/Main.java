import java.net.ServerSocket;
import java.net.Socket;

public class Main {

    public static final int PORT = 5000;

    public static void main(String[] args) {

        try {
            ServerSocket serverSocket = new ServerSocket(PORT);

            System.out.println("Servidor iniciado en puerto " + PORT);

            while (true) {

                Socket clientSocket = serverSocket.accept();

                System.out.println("Cliente conectado");

                ClientHandler clientHandler =
                        new ClientHandler(clientSocket);

                clientHandler.start();
            }

        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
