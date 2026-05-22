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

            // Leer mensaje inicial del servidor
            String mensajeInicial = in.readLine();
            System.out.println("Servidor dice: " + mensajeInicial);
            System.out.println();

            enviarYRecibir(out, in, "MOVE LEFT");
            enviarYRecibir(out, in, "MOVE RIGHT");
            enviarYRecibir(out, in, "SHOOT");
            enviarYRecibir(out, in, "PLAYER_HIT");
            enviarYRecibir(out, in, "PLAYER_HIT");
            enviarYRecibir(out, in, "PLAYER_HIT");
            enviarYRecibir(out, in, "ALIEN_KILLED 2");

            socket.close();

        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    private static void enviarYRecibir(PrintWriter out, BufferedReader in, String mensaje) throws Exception {
        System.out.println("====================================");
        System.out.println("Enviando: " + mensaje);

        out.println(mensaje);

        String respuesta = in.readLine();

        System.out.println("Respuesta del servidor:");
        imprimirRespuestaBonita(respuesta);

        System.out.println();
    }

    private static void imprimirRespuestaBonita(String respuesta) {

        if (respuesta == null) {
            System.out.println("No se recibió respuesta del servidor.");
            return;
        }

        if (respuesta.startsWith("PLAYER_ID")) {
            System.out.println(respuesta);
            return;
        }

        if (!respuesta.startsWith("STATE")) {
            System.out.println(respuesta);
            return;
        }


        String[] partes = respuesta.split("\\s+");

        System.out.println("----- ESTADO DEL JUEGO -----");

        int i = 1;

        while (i < partes.length) {

            String tipo = partes[i];

            if (tipo.equals("PLAYER")) {
                int id = Integer.parseInt(partes[i + 1]);
                int x = Integer.parseInt(partes[i + 2]);
                int vidas = Integer.parseInt(partes[i + 3]);
                int puntaje = Integer.parseInt(partes[i + 4]);

                System.out.println("Jugador:");
                System.out.println("  ID: " + id);
                System.out.println("  X: " + x);
                System.out.println("  Vidas: " + vidas);
                System.out.println("  Puntaje: " + puntaje);

                i += 5;
            }

            else if (tipo.equals("ALIEN")) {
                int id = Integer.parseInt(partes[i + 1]);
                int x = Integer.parseInt(partes[i + 2]);
                int y = Integer.parseInt(partes[i + 3]);
                int puntos = Integer.parseInt(partes[i + 4]);
                boolean vivo = Boolean.parseBoolean(partes[i + 5]);

                System.out.println("Alien:");
                System.out.println("  ID: " + id);
                System.out.println("  Posicion: (" + x + ", " + y + ")");
                System.out.println("  Puntos: " + puntos);
                System.out.println("  Vivo: " + vivo);

                i += 6;
            }

            else if (tipo.equals("BUNKER")) {
                int id = Integer.parseInt(partes[i + 1]);
                int vida = Integer.parseInt(partes[i + 2]);

                System.out.println("Bunker:");
                System.out.println("  ID: " + id);
                System.out.println("  Vida: " + vida + "%");

                i += 3;
            }

            else if (tipo.equals("SPEED")) {
                int velocidad = Integer.parseInt(partes[i + 1]);

                System.out.println("Velocidad aliens: " + velocidad);

                i += 2;
            }

            else if (tipo.equals("GAME_OVER")) {
                boolean gameOver = Boolean.parseBoolean(partes[i + 1]);

                System.out.println("Game over: " + gameOver);
                i += 2;
            }

            else {
                System.out.println("Dato desconocido: " + tipo);
                i++;
            }
        }

        System.out.println("----------------------------");
    }
}