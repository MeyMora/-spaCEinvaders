package Logica;

import Modelos.UFO;
import Patrones.AbstractFactory.GameElementFactory;
import java.util.Random;

public class UFOManager {

    private UFO ufo;
    private int nextUfoId;
    private int ufoTickCounter;
    private int ufoSpawnInterval;
    private final Random random;
    private GameElementFactory factory;

    public UFOManager(GameElementFactory factory) {
        this.factory = factory;
        this.random = new Random();
        this.nextUfoId = 1;
        this.ufoTickCounter = 0;
        this.ufoSpawnInterval = 30 + random.nextInt(31);
        this.ufo = null;
    }

    public void setFactory(GameElementFactory factory) {
        this.factory = factory;
    }

    // Crea un UFO manualmente (comando del admin)
    public void createUFO(int x, int y, String direction, int points) {
        ufo = factory.createUFO(nextUfoId++, x, y, direction, points);
    }

    // Destruye el UFO y retorna los puntos que vale
    public int destroyUFO() {
        if (ufo != null && ufo.isActive()) {
            int points = ufo.getPoints();
            ufo.destroy();
            return points;
        }
        return 0;
    }

    public boolean isUFOActive() {
        return ufo != null && ufo.isActive();
    }

    // Mueve el UFO una posicion segun su direccion
    public void moverUFO() {
        if (ufo == null || !ufo.isActive()) return;
        ufo.move();
    }

    // Verifica si es momento de generar un nuevo UFO automaticamente
    public void verificarSpawnUFO() {
        if (ufo != null && ufo.isActive()) {
            ufoTickCounter = 0;
            return;
        }
        ufoTickCounter++;
        if (ufoTickCounter >= ufoSpawnInterval) {
            generarUFO();
            ufoTickCounter = 0;
            ufoSpawnInterval = 30 + random.nextInt(31);
        }
    }

    // Genera un UFO con direccion y puntos aleatorios
    private void generarUFO() {
        String direction = random.nextBoolean() ? "I-D" : "D-I";
        int points = 50 + random.nextInt(451);
        int startX = direction.equals("I-D") ? 0 : 100;
        ufo = factory.createUFO(nextUfoId++, startX, 0, direction, points);
        System.out.println("OVNI aparecio con direccion " + direction + " y " + points + " puntos");
    }

    // Resetea para nueva ronda
    public void resetRonda() {
        ufo = null;
        ufoTickCounter = 0;
        ufoSpawnInterval = 30 + random.nextInt(31);
    }

    // Reinicio completo
    public void reiniciar() {
        ufo = null;
        nextUfoId = 1;
        ufoTickCounter = 0;
        ufoSpawnInterval = 30 + random.nextInt(31);
    }

    public UFO getUFO() {
        return ufo;
    }
}
