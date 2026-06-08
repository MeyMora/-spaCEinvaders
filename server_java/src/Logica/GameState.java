package Logica;

import java.util.ArrayList;
import java.util.List;

import Modelos.Alien;
import Modelos.Bunker;
import Modelos.Player;
import Modelos.UFO;
import Patrones.Observer.GameObserver;
import Patrones.Observer.GameSubject;

import Patrones.AbstractFactory.GameElementFactory;
import Patrones.AbstractFactory.ClassicGameElementFactory;
import Patrones.AbstractFactory.HardGameElementFactory;


public class GameState extends GameSubject {
    // Lista que guarda todos los jugadores conectados al juego.
    private List<Player> players;

    // Lista que guarda todos los aliens del juego
    private List<Alien> aliens;

    // Lista que guarda todos los bunkers del juego
    private List<Bunker> bunkers;

    //Velocidad actual de los aliens
    private int alienSpeed;

    //Variable que controla el identificador del sigueinte alien que se cree
    private int nextAlienId;

    //Variable que controla si el jugador perdio o no.
    private boolean gameOver;

    private UFO ufo;

    private int nextUfoId;

    private GameElementFactory gameElementFactory;


    //Constructor de la clase modelos.GameState.
    //Aqui se inicializan las listas y los valores iniciales del juego.
    public GameState() {
        super();  // Inicializa la lista de observers
        players = new ArrayList<>(); // Lista de juegadores
        aliens = new ArrayList<>();  // Lista de Aliens
        bunkers = new ArrayList<>(); // Lista de bunkers


        // Se define la familia inicial de objetos del juego
        gameElementFactory = new ClassicGameElementFactory();

        alienSpeed = 100; // Velocidad inicial de los aliens.
        nextAlienId = 1;  // El primer alien tendra el id 1.
        gameOver = false; // Se inicializa la perdida del juego en false.
        nextUfoId = 1;
        ufo = null;

        createInitialBunkers(); // Se crean los bunkers iniciales.
        createInitialAliens();  // Se crean los aliens iniciales.
    }
    // Metodo privado que crea los bunkers iniciales del juego
    private void createInitialBunkers() {
        // Se crean los 4 bunkers o escudos de proteccion que indica la descripcion de juego
            bunkers.add(gameElementFactory.createBunker(1, 10, 20));
            bunkers.add(gameElementFactory.createBunker(2, 30, 20));
            bunkers.add(gameElementFactory.createBunker(3, 50, 20));
            bunkers.add(gameElementFactory.createBunker(4, 70, 20));

    }
    //Metodo privado que crea los aliens iniciales del juego.
    private void createInitialAliens() {
        aliens.add(gameElementFactory.createCalamarAlien(nextAlienId++, 1, 1));
        aliens.add(gameElementFactory.createCangrejoAlien(nextAlienId++, 2, 1));
        aliens.add(gameElementFactory.createPulpoAlien(nextAlienId++, 3, 1));
    }

    // Metodo sincronizado que agrega un nuevo jugador al juego.
    // synchronized evita problemas si varios clientes intentan modificar el estado al mismo tiempo.
    public synchronized Player addPlayer() {
        Player player = new Player(players.size() +1);  // Se crea un jugador con un id segun la cantidad actual de jugadores.
        players.add(player); // Se agrega un jugador a la lista
        return player; // Se devuelve el jugador creado
    }

    //Metodo sincronizado que crea un nuevo alien
    public synchronized boolean createAlien(int x, int y, int points) {
        for (Alien alien : aliens) {
            if (alien.isAlive() && alien.getX() == x && alien.getY() == y) {
                return false;
            }
        }
        createAlienWithoutNotify(x, y, points);
        notifyObservers();
        return true;
    }

    // Metodo sincronizado que mueve un jugador hacia la izquierda.
    public synchronized void movePlayerLeft(int playerId){
        Player player = getPlayerById(playerId); // Se busca el jugador por su id.

        if (player != null) { // Si el jugador existe, se mueve a la izquierda
            player.moveLeft();
            notifyObservers();
        }
    }

    //Metodo sincronizado que mueve un jugador hacia la derecha.
    public synchronized void movePlayerRight(int playerId){
        Player player = getPlayerById(playerId); // Se busca el jugador por su id.

        if (player != null) { // Si el jugador existe, se mueve a la derecha.
            player.moveRight();
            notifyObservers();
        }
    }

    //Metodo sincronizado que permite eliminar un alien y sume puntos al jugador
    public synchronized boolean killAlien(int playerId, int alienId){
        Player player = getPlayerById(playerId); // Se busca el jugadorp por su id
        Alien alien = getAlienById(alienId); // Se busca el alien por su id.

        // Se verifica que el jugador exista, que el alien exista y que el alien este vivo.
        if(player != null && alien != null && alien.isAlive() && !gameOver) {
            alien.kill(); // Se marca el alien como muerto.
            player.addScore(alien.getPoints()); // Se suma los puntos del alien al jugador.

            if (allAliensDead()){
                resetRound(player);
            }else{
                notifyObservers();
            }
            return true;

        }
        return false;
    }

    // Metodo privado que busca un jugador por su id.
    private Player getPlayerById(int playerId) {
        for (Player player : players) {
            if (player.getId() == playerId) {
                return player; // Si encuentra el jugador, lo devuelve
            }
        }
        return null; // Si no lo encuentra, devuelve null.
    }

    // Metodo privado que busca un alien por su id.
    private Alien getAlienById(int alienId) {
        for (Alien alien : aliens) {
            if (alien.getId() == alienId) {
                return alien; // Si encuentra el alien, lo devuelve.
            }
        }
        return null; // Si no lo encuentra, devuelve nulo.
    }

    // Metodo sincronizado que genera un mensaje con el estado actual del juego.
    public synchronized String getStateMessage() {
        StringBuilder message = new StringBuilder(); //Se usa para construir el mensaje de forma eficiente.
        message.append("STATE: "); // Indica que el mensaje contiene el estado del juego.

        // Se agrega la informacion de cada jugador al mensaje
        for (Player player : players) {
            message.append("PLAYER ") // Mensaje del jugador
                    .append(player.getId()).append(" ") // Id del jugador
                    .append(player.getX()).append(" ")  // Posicion del jugador
                    .append(player.getLives()).append(" ") // Vidas del jugador
                    .append(player.getScore()).append(" "); //Puntaje del jugador

        }

        // Se agrega la informacion de cada alien al mensaje.
        for (Alien alien : aliens) {
            message.append(alien.toMessage()).append(" ");
        }

        // Se agrega la informacion de cada bunker al mensaje
        for (Bunker bunker : bunkers) {
            message.append(bunker.toMessage()).append(" ");
        }
        if (ufo != null) {
            message.append(ufo.toMessage()).append(" ");
        }


        // Se agrega la velocidad actual de los aliens
        message.append("SPEED ").append(alienSpeed).append(" ");
        message.append("GAME_OVER ").append(gameOver);

        // Se devuelve el mensaje como texto.
        return message.toString();

    }
    public synchronized void playerHit(int playerId) {
        Player player = getPlayerById(playerId);

        if (player != null && !gameOver) {
            player.loseLife();
            if (player.getLives() <= 0) {
                gameOver = true;
            }
            notifyObservers();
        }
    }
    private boolean allAliensDead() {
        for (Alien alien : aliens) {
            if (alien.isAlive()) {
                return false;
            }
        }

        return true;
    }
    private void resetRound(Player player) {
        player.addLife();

        alienSpeed += 20;

        // A partir de cierta velocidad se cambia la familia de objetos del juego.
        if (alienSpeed >= 140) {
            gameElementFactory = new HardGameElementFactory();
        }

        aliens.clear();
        bunkers.clear();
        ufo = null;

        for (Player p : players) {
            p.resetPosition();
        }

        createInitialAliens();
        createInitialBunkers();

        System.out.println(
                "Ronda completada. Nueva velocidad: " + alienSpeed +
                        ". Nueva vida de bunkers: " + getBunkersHealthMessage()
        );

        notifyObservers();
    }
    public synchronized void setAlienSpeed(int alienSpeed) {
        this.alienSpeed = alienSpeed;
        notifyObservers();
    }
    public synchronized void setBunkersHealth(int health) {
        for (Bunker bunker : bunkers) {
            bunker.setHealth(health);
        }
        notifyObservers();
    }

    public synchronized boolean hitBunker(int bunkerId) {
        for (Bunker bunker : bunkers) {
            if (bunker.getId() == bunkerId && bunker.getHealth() > 0) {
                bunker.damage(10);
                notifyObservers();
                return true;
            }
        }
        return false;
    }
    public synchronized boolean isGameOver(){
        return gameOver;
    }

    // Se activa cuando el cliente C detecta que los aliens llegaron al nivel del cañón
    public synchronized void aliensLlegaronBase() {
        if (!gameOver) {
            gameOver = true;
            System.out.println("Los aliens llegaron al jugador. Game Over.");
            notifyObservers();
        }
    }

    // Reinicia el juego completamente al estado inicial
    public synchronized void restartGame() {
        gameOver = false;
        alienSpeed = 100;
        nextAlienId = 1;
        nextUfoId = 1;
        ufo = null;
        gameElementFactory = new ClassicGameElementFactory();

        for (Player player : players) {
            player.resetPosition();
            player.resetGame();
        }

        aliens.clear();
        bunkers.clear();

        createInitialAliens();
        createInitialBunkers();

        System.out.println("Juego reiniciado.");
        notifyObservers();
    }
    public synchronized void createUFO(int x, int y, String direction, int points) {
        ufo = gameElementFactory.createUFO(nextUfoId++, x, y, direction, points);
        notifyObservers();
    }

    public synchronized boolean destroyUFO(int playerId) {
        Player player = getPlayerById(playerId);

        if (player != null && ufo != null && ufo.isActive() && !gameOver) {
            ufo.destroy();
            player.addScore(ufo.getPoints());

            notifyObservers();
            return true;
        }

        return false;
    }

    public synchronized int getPlayerCount() {
        return players.size();
    }


    @Override
    public void notifyObservers() {
        String stateMessage;
        List<GameObserver> observersCopy;

        synchronized (this) {
            stateMessage = getStateMessage();
            observersCopy = new ArrayList<>(observers);
        }

        for (GameObserver observer : observersCopy) {
            observer.update(stateMessage);
        }
    }

    private String getBunkersHealthMessage() {
        StringBuilder message = new StringBuilder();

        for (Bunker bunker : bunkers) {
            message.append("Bunker ")
                    .append(bunker.getId())
                    .append(": ")
                    .append(bunker.getHealth())
                    .append(" | ");
        }

        return message.toString();
    }

    private Alien createAlienWithoutNotify(int x, int y, int points) {
        Alien alien = gameElementFactory.createAlienByPoints(nextAlienId++, x, y, points);
        aliens.add(alien);
        return alien;
    }


}
