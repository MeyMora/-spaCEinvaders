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
    private List<Bunker> bunkers;
    private int alienSpeed;
    private boolean gameOver;
    private GameElementFactory gameElementFactory;

    private AlienManager alienManager;
    private UFOManager ufoManager;
    private GameLoop gameLoop;

    public GameState() {
        super();
        players = new ArrayList<>();
        bunkers = new ArrayList<>();

        gameElementFactory = new ClassicGameElementFactory();
        alienManager = new AlienManager(gameElementFactory);
        ufoManager = new UFOManager(gameElementFactory);
        gameLoop = new GameLoop(this);

        alienSpeed = 100;
        gameOver = false;

        createInitialBunkers();
        alienManager.createInitialAliens();
        gameLoop.iniciar();
    }

    private void createInitialBunkers() {
        bunkers.add(gameElementFactory.createBunker(1, 10, 20));
        bunkers.add(gameElementFactory.createBunker(2, 30, 20));
        bunkers.add(gameElementFactory.createBunker(3, 50, 20));
        bunkers.add(gameElementFactory.createBunker(4, 70, 20));
    }

    public synchronized Player addPlayer() {
        Player player = new Player(players.size() + 1);
        players.add(player);
        return player;
    }

    public synchronized boolean createAlien(int x, int y, int points) {
        boolean created = alienManager.createAlien(x, y, points);
        if (created) notifyObservers();
        return created;
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

    public synchronized boolean killAlien(int playerId, int alienId) {
        Player player = getPlayerById(playerId);
        Alien alien = alienManager.getAlienById(alienId);

        if (player != null && alien != null && alien.isAlive() && !gameOver) {
            alien.kill();
            player.addScore(alien.getPoints());

            if (alienManager.allAliensDead()) {
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

        for (Alien alien : alienManager.getAliens()) {
            message.append(alien.toMessage()).append(" ");
        }

        for (Bunker bunker : bunkers) {
            message.append(bunker.toMessage()).append(" ");
        }

        UFO ufo = ufoManager.getUFO();
        if (ufo != null) {
            message.append(ufo.toMessage()).append(" ");
        }

        message.append("SPEED ").append(alienSpeed).append(" ");
        message.append("GAME_OVER ").append(gameOver);

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

    private void resetRound(Player player) {
        player.addLife();
        alienSpeed += 20;

        if (alienSpeed >= 140) {
            gameElementFactory = new HardGameElementFactory();
            alienManager.setFactory(gameElementFactory);
            ufoManager.setFactory(gameElementFactory);
        }

        bunkers.clear();
        alienManager.resetRonda();
        ufoManager.resetRonda();

        for (Player p : players) {
            p.resetPosition();
        }

        createInitialBunkers();
        System.out.println("Ronda completada. Nueva velocidad: " + alienSpeed);
        notifyObservers();
    }

    public synchronized void setAlienSpeed(int alienSpeed) {
        this.alienSpeed = alienSpeed;
        notifyObservers();
    }

    public int getAlienSpeed() {
        return alienSpeed;
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

    public synchronized void createUFO(int x, int y, String direction, int points) {
        ufoManager.createUFO(x, y, direction, points);
        notifyObservers();
    }

    public synchronized boolean destroyUFO(int playerId) {
        Player player = getPlayerById(playerId);
        if (player != null && ufoManager.isUFOActive() && !gameOver) {
            int points = ufoManager.destroyUFO();
            player.addScore(points);
            notifyObservers();
            return true;
        }
        return false;
    }

    public synchronized int getPlayerCount() {
        return players.size();
    }

    public synchronized void aliensLlegaronBase() {
        if (!gameOver) {
            gameOver = true;
            System.out.println("Los aliens llegaron al jugador. Game Over.");
            notifyObservers();
        }
    }

    public synchronized void restartGame() {
        gameOver = false;
        alienSpeed = 100;
        gameElementFactory = new ClassicGameElementFactory();

        alienManager.setFactory(gameElementFactory);
        ufoManager.setFactory(gameElementFactory);

        for (Player player : players) {
            player.resetGame();
        }

        bunkers.clear();
        alienManager.reiniciar();
        ufoManager.reiniciar();
        createInitialBunkers();

        System.out.println("Juego reiniciado.");
        notifyObservers();
        gameLoop.iniciar();
    }

    // Llamado por GameLoop en cada tick del juego
    public synchronized void tick() {
        boolean aliensLlegaron = alienManager.moverAliens();
        if (aliensLlegaron && !gameOver) {
            gameOver = true;
            System.out.println("Los aliens llegaron a la base. Game Over.");
        }
        ufoManager.moverUFO();
        ufoManager.verificarSpawnUFO();
        notifyObservers();
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
}
