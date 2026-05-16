//Clase publica Jugador
public class Player { // Es como el molde para crear jugadores
    private int id; //Variable privada llamada id, para identificar al jugador
    private int lives; //Variable privada llamada lives guarda la cantidad de vidas que tiene el jugador
    private int  score; //Variable privada para guardar el puntaje del jugador.
    private int x; //Variable privada que representa la posicion horizontal del jugador.
    private boolean alive; // Variable booleana privada, que representa si el jugador esta vivo(true) o muerto(false).


    // Constructor de la clase
    //Se ejecuta cuando se crea un nuevo jugador
    public Player(int id){
        this.id = id; //Guarda el id recibido en la variable del objeto
        this.lives = 3; // jugador inicia con 3 vidas
        this.score = 0; //Inicia con 0 puntos
        this.x = 50; //Jugador inica en la posicion horzontal 50.
        this.alive = true; //Inicia como vivo.
    }
    // Metodo que devuelve el id del jugador
    public int getId() {
        return id;
    }

    //Metodo que devuelve la cantidad de vidas actuales del jugador
    public int getLives() {
        return lives;
    }
    //Metodo que devuelve el puntaje actual del jugador
    public int getScore() {
        return score;
    }

    //Metodo que devuelve la posicion horizontal actual del jugador.
    public int getX() {
        return x;
    }

    //Metodo que deveulve si el jugador esta vivo o no.
    public boolean isAlive() {
        return alive;
    }

    //Metodo para mover al jugador hacia la izquierda
    public void moveLeft(){
        if(x>0){ // Revisa si x es mayor que 0, para evitar que el jugador se salga del limite izquierdo.
            x--; //Disminuye la posicion x en 1.
        }
    }

    //Metodo para mover al jugador hacia la derecha.
    public void moveRight(){
        if (x < 100){ //Antes de moverlo, revisa si x es menor que 100 para evitar que el jugador se salga del limite derecho.
           x++; // Aumenta la posicion x en 1.
        }
    }

    //Metodo para sumar puntos al jugador
    // Recibe: Un numero llamado points
    //Devuelve: Puntaje actual
    public void addScore(int points){
        this.score += points; //Suma los puntos recibidos al puntaje actual
    }
    //Metodo que sirve para quitarle una vida al jugador
    public void loseLife(){
        lives--; //Resta una vida
        if (lives <= 0){ //Revisa si las vidas son menores o iguales a 0
            alive = false; // Si las vidas son 0 o son menores que, el jugador pasa a estar muerto.
        }
    }

    // Metodo addLife que sirve para agregar una vida al jugador
    public void addLife(){
        lives++; // Aumenta la vida en 1.
    }
}
