import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.Socket;

public class TestClient {

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

            System.out.println("Cliente de prueba conectado al servidor.");

            enviarYRecibir(out, in, "MOVE LEFT");
            enviarYRecibir(out, in, "MOVE RIGHT");
            enviarYRecibir(out, in, "SHOOT");
            enviarYRecibir(out, in, "ALIEN KILLED 1");

            socket.close();

        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    private static void enviarYRecibir(PrintWriter out, BufferedReader in, String mensaje) throws Exception {
        System.out.println("Enviando: " + mensaje);

        out.println(mensaje);

        String respuesta = in.readLine();

        System.out.println("Servidor dice: " + respuesta);
        System.out.println();
    }
}