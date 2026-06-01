import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.Socket;

public class TestClientInteractive {

    public static void main(String[] args) {

        try {
            Socket socket = new Socket("127.0.0.1", 5000);

            BufferedReader in = new BufferedReader(
                    new InputStreamReader(socket.getInputStream())
            );

            PrintWriter out = new PrintWriter(
                    socket.getOutputStream(),
                    true
            );

            BufferedReader keyboard = new BufferedReader(
                    new InputStreamReader(System.in)
            );

            System.out.println("Cliente interactivo conectado.");
            System.out.println("Escriba comandos como:");
            System.out.println("MOVE LEFT");
            System.out.println("MOVE RIGHT");
            System.out.println("ALIEN KILLED 1");
            System.out.println("PLAYER HIT");
            System.out.println("SPEED 150");
            System.out.println("BUNKERS 70");
            System.out.println();

            Thread listener = new Thread(() -> {
                try {
                    String response;

                    while ((response = in.readLine()) != null) {
                        System.out.println("\n[Servidor] " + response);
                        System.out.print("> ");
                    }

                } catch (Exception e) {
                    System.out.println("Conexión cerrada.");
                }
            });

            listener.start();

            String command;

            while ((command = keyboard.readLine()) != null) {
                out.println(command);
            }

            socket.close();

        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}