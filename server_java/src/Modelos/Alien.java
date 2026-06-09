package Modelos;

// Clase modelos.Alien que nos permite crear los alien, que son los enemigos del juego
public abstract class Alien {
    private int id; //Variable privada. Sirve para identificar el alien. Ejemplo: alien1, alien2
    private int x;  // Se declara variable privada llamada x, es la posicion horizontal del alien.
    private int y;  // Se declara una variable privada llamada y. Posicion vertical del alien.
    private int points; //Representa cuantos puntos gana el jugador al eliminar ese alien.
    private boolean alive; //Variable privada booleana que sirve para ver si los aliens estan vivos o muertos.

    public Alien(int id, int x, int y, int points) { // Constructor de la clase alien
        this.id = id; // Guarda el id recibido en la variable id del alien
        this.x = x; // Guarda la posicion horizontal recibida en el atributo x
        this.y = y; // Guarda la posicion vertical recibida en el atributo y.
        this.points = points; //Guarda la cantidad de puntos que vale el alien.
        this.alive = true; //Cuando el alien se crea, se inicia como vivo.
    }

    // Metodo que devuelve el id del alien
    public int getId() {
        return id;
    }

    // Metodo que devuelve la posicion horizontal del alien
    public int getX() {
        return x;
    }

    // Metodo que devuelve la posicion vertical del alien
    public int getY() {
        return y;
    }

    // Metodo que devuelve los puntos que vale el alien
    public int getPoints() {
        return points;
    }
    // Metodo que devuelve si el alien esta vivo o no.
    public boolean isAlive() {
        return alive;
    }
    // Metodo que sirve para matar o eliminar el alien.
    public void kill(){
        this.alive = false; // Cambia el estado del alien a muerto
    }

    // Metodo que mueve el alien horizontalmente
    public void moveHorizontal(int direction) {
        x += direction;
    }

    // Metodo que mueve el alien hacia abajo
    public void moveDown() {
        y++;
    }
    //Metodo que devuelve un texto con la informacion del alien, retorna una cadena de texto
    public String toMessage() {
        return "ALIEN " + id +" "+x + " " + y + " " +points + " " + alive ;
    }

    public abstract  String getTipo();
}
