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

    private List<Player> players;
    private List<Alien>  aliens;
    private List<Bunker> bunkers;

    private int alienSpeed;
    private int nextAlienId;
    private boolean gameOver;

    private UFO ufo;
    private int nextUfoId;

    private GameElementFactory gameElementFactory;

    // Pulso de disparo: marca que un jugador disparo en este ultimo
    // ciclo, para que los espectadores y el companero vean la bala.
    // Se limpia automaticamente despues de incluirse en un STATE:
    private java.util.Set<Integer> shotPulse = new java.util.HashSet<>();

    // Distribucion de la formacion: 5 filas x 11 columnas = 55 aliens
    // Igual al Space Invaders original
    // Fila 0: pulpo (40 pts) — los mas valiosos arriba
    // Filas 1-2: cangrejo (20 pts)
    // Filas 3-4: calamar (10 pts) — los mas baratos abajo
    private static final int ALIEN_FILAS = 5;
    private static final int ALIEN_COLS  = 11;

    // Separacion en coordenadas logicas (0-100)
    // El area de juego es 100 unidades ancho x 100 alto
    // Dejamos margen: empezamos en x=3, separacion 8 => 3 + 10*8 = 83 (entra bien)
    // Empezamos en y=5, separacion 10 => 5 + 4*10 = 45 (parte superior)
    private static final int ALIEN_START_X  = 3;
    private static final int ALIEN_START_Y  = 4;
    private static final int ALIEN_SEP_X    = 8;
    private static final int ALIEN_SEP_Y    = 10;

    public GameState() {
        super();
        players = new ArrayList<>();
        aliens  = new ArrayList<>();
        bunkers = new ArrayList<>();

        gameElementFactory = new ClassicGameElementFactory();

        alienSpeed  = 1000;
        nextAlienId = 1;
        gameOver    = false;
        nextUfoId   = 1;
        ufo         = null;

        createInitialBunkers();
        createInitialAliens();
    }

    // Crea la formacion inicial: 5 filas x 11 columnas bien distribuidas
    private void createInitialAliens() {
        for (int fila = 0; fila < ALIEN_FILAS; fila++) {
            for (int col = 0; col < ALIEN_COLS; col++) {
                int x = ALIEN_START_X + col * ALIEN_SEP_X;
                int y = ALIEN_START_Y + fila * ALIEN_SEP_Y;

                Alien alien;
                if (fila == 0) {
                    // Fila superior: pulpo (40 pts)
                    alien = gameElementFactory.createPulpoAlien(nextAlienId++, x, y);
                } else if (fila <= 2) {
                    // Filas medias: cangrejo (20 pts)
                    alien = gameElementFactory.createCangrejoAlien(nextAlienId++, x, y);
                } else {
                    // Filas inferiores: calamar (10 pts)
                    alien = gameElementFactory.createCalamarAlien(nextAlienId++, x, y);
                }
                aliens.add(alien);
            }
        }
        System.out.println("Formacion creada: " + aliens.size() + " aliens (" +
                           ALIEN_FILAS + " filas x " + ALIEN_COLS + " columnas)");
    }

    private void createInitialBunkers() {
        bunkers.add(gameElementFactory.createBunker(1, 15, 75));
        bunkers.add(gameElementFactory.createBunker(2, 35, 75));
        bunkers.add(gameElementFactory.createBunker(3, 60, 75));
        bunkers.add(gameElementFactory.createBunker(4, 80, 75));
    }

    // ── Sistema de slots fijos para jugadores ────────────────────────
    // slotOcupado[0] = true si el PLAYER_ID 1 esta en uso
    // slotOcupado[1] = true si el PLAYER_ID 2 esta en uso
    private boolean[] slotOcupado = new boolean[]{ false, false };

    // Intenta reservar el slot solicitado (1 o 2).
    // Devuelve el Player asignado, o null si el slot ya estaba ocupado.
    public synchronized Player joinAsPlayer(int slot) {
        if (slot != 1 && slot != 2) return null;
        if (slotOcupado[slot - 1]) return null;

        slotOcupado[slot - 1] = true;

        // Buscar si ya existe un Player con ese id (de una conexion anterior)
        for (Player p : players) {
            if (p.getId() == slot) {
                p.resetGame();
                return p;
            }
        }

        // Crear un Player nuevo con el id = slot
        Player player = new Player(slot);
        players.add(player);
        return player;
    }

    // Libera el slot cuando el cliente se desconecta
    public synchronized void leaveSlot(int slot) {
        if (slot == 1 || slot == 2) {
            slotOcupado[slot - 1] = false;
        }
    }

    // Devuelve true si el slot (1 o 2) esta libre
    public synchronized boolean isSlotFree(int slot) {
        if (slot != 1 && slot != 2) return false;
        return !slotOcupado[slot - 1];
    }

    public synchronized Player addPlayer() {
        Player player = new Player(players.size() + 1);
        players.add(player);
        return player;
    }

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

    public synchronized void movePlayerLeft(int playerId) {
        Player player = getPlayerById(playerId);
        if (player != null) {
            player.moveLeft();
            notifyObservers();
        }
    }

    public synchronized void movePlayerRight(int playerId) {
        Player player = getPlayerById(playerId);
        if (player != null) {
            player.moveRight();
            notifyObservers();
        }
    }

    // Marca que el jugador disparo en este instante.
    // Se incluira como "SHOT <id>" en el siguiente STATE: y luego se limpia.
    public synchronized void registerShot(int playerId) {
        if (!gameOver) {
            shotPulse.add(playerId);
            notifyObservers();
        }
    }

    public synchronized boolean killAlien(int playerId, int alienId) {
        Player player = getPlayerById(playerId);
        Alien  alien  = getAlienById(alienId);

        if (player != null && alien != null && alien.isAlive() && !gameOver) {
            alien.kill();
            player.addScore(alien.getPoints());

            if (allAliensDead()) {
                resetRound(player);
            } else {
                notifyObservers();
            }
            return true;
        }
        return false;
    }

    private Player getPlayerById(int playerId) {
        for (Player player : players) {
            if (player.getId() == playerId) return player;
        }
        return null;
    }

    private Alien getAlienById(int alienId) {
        for (Alien alien : aliens) {
            if (alien.getId() == alienId) return alien;
        }
        return null;
    }

    public synchronized String getStateMessage() {
        StringBuilder message = new StringBuilder();
        message.append("STATE: ");

        for (Player player : players) {
            message.append("PLAYER ")
                   .append(player.getId()).append(" ")
                   .append(player.getX()).append(" ")
                   .append(player.getLives()).append(" ")
                   .append(player.getScore()).append(" ");
        }

        for (Alien alien : aliens) {
            message.append(alien.toMessage()).append(" ");
        }

        for (Bunker bunker : bunkers) {
            message.append(bunker.toMessage()).append(" ");
        }

        if (ufo != null) {
            message.append(ufo.toMessage()).append(" ");
        }

        // Pulso de disparos: cada jugador que disparo desde el ultimo
        // STATE: aparece como "SHOT <id>". Se limpia despues de incluirlo
        // para que sea un evento de un solo frame (no persistente).
        for (Integer shooterId : shotPulse) {
            message.append("SHOT ").append(shooterId).append(" ");
        }
        shotPulse.clear();

        message.append("SPEED ").append(alienSpeed).append(" ");
        message.append("GAME_OVER ").append(gameOver);

        return message.toString();
    }

    public synchronized void playerHit(int playerId) {
        Player player = getPlayerById(playerId);
        if (player != null && !gameOver) {
            player.loseLife();
            if (player.getLives() <= 0) gameOver = true;
            notifyObservers();
        }
    }

    private boolean allAliensDead() {
        for (Alien alien : aliens) {
            if (alien.isAlive()) return false;
        }
        return true;
    }

    private void resetRound(Player player) {
        player.addLife();
        alienSpeed += 20;

        if (alienSpeed >= 140) {
            gameElementFactory = new HardGameElementFactory();
        }

        aliens.clear();
        bunkers.clear();
        // El OVNI NO se reinicia aqui: si esta activo, debe permanecer
        // en pantalla hasta que el jugador lo destruya o se vaya solo.

        // Resetear nextAlienId para que los IDs comiencen desde 1 en cada ronda
        // Esto permite que el cliente los reconozca como frescos y limpie su estado
        nextAlienId = 1;
        nextUfoId   = 1;

        for (Player p : players) {
            p.resetPosition();
        }

        createInitialAliens();
        createInitialBunkers();

        System.out.println("Ronda completada. Nueva velocidad: " + alienSpeed +
                           ". Bunkers: " + getBunkersHealthMessage());
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

    public synchronized boolean isGameOver() {
        return gameOver;
    }

    public synchronized void aliensLlegaronBase() {
        if (!gameOver) {
            gameOver = true;
            System.out.println("Los aliens llegaron al jugador. Game Over.");
            notifyObservers();
        }
    }

    public synchronized void restartGame() {
        gameOver    = false;
        alienSpeed  = 100;
        nextAlienId = 1;
        nextUfoId   = 1;
        ufo         = null;
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
            stateMessage  = getStateMessage();
            observersCopy = new ArrayList<>(observers);
        }

        for (GameObserver observer : observersCopy) {
            observer.update(stateMessage);
        }
    }

    private String getBunkersHealthMessage() {
        StringBuilder message = new StringBuilder();
        for (Bunker bunker : bunkers) {
            message.append("Bunker ").append(bunker.getId())
                   .append(": ").append(bunker.getHealth()).append(" | ");
        }
        return message.toString();
    }

    private Alien createAlienWithoutNotify(int x, int y, int points) {
        Alien alien = gameElementFactory.createAlienByPoints(nextAlienId++, x, y, points);
        aliens.add(alien);
        return alien;
    }
}
