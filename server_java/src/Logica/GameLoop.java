package Logica;

public class GameLoop implements Runnable {

    private final GameState gameState;
    private Thread gameLoopThread;

    public GameLoop(GameState gameState) {
        this.gameState = gameState;
    }

    // Inicia el loop. Si ya habia uno corriendo, lo detiene primero.
    public void iniciar() {
        if (gameLoopThread != null && gameLoopThread.isAlive()) {
            gameLoopThread.interrupt();
        }
        gameLoopThread = new Thread(this);
        gameLoopThread.setDaemon(true);
        gameLoopThread.start();
    }

    public void detener() {
        if (gameLoopThread != null) {
            gameLoopThread.interrupt();
        }
    }

    @Override
    public void run() {
        while (!gameState.isGameOver()) {
            try {
                int delay = Math.max(50, 1000 - gameState.getAlienSpeed() * 5);
                Thread.sleep(delay);
                if (!gameState.isGameOver()) {
                    gameState.tick();
                }
            } catch (InterruptedException e) {
                break;
            }
        }
    }
}
