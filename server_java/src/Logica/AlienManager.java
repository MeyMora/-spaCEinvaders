package Logica;

import Modelos.Alien;
import Patrones.AbstractFactory.GameElementFactory;
import java.util.ArrayList;
import java.util.List;

public class AlienManager {

    private List<Alien> aliens;
    private int nextAlienId;
    private int alienDirection;
    private GameElementFactory factory;

    public AlienManager(GameElementFactory factory) {
        this.aliens = new ArrayList<>();
        this.nextAlienId = 1;
        this.alienDirection = 1;
        this.factory = factory;
    }

    public void setFactory(GameElementFactory factory) {
        this.factory = factory;
    }

    // Crea los aliens iniciales de la ronda usando la fabrica
    public void createInitialAliens() {
        aliens.add(factory.createCalamarAlien(nextAlienId++, 1, 1));
        aliens.add(factory.createCangrejoAlien(nextAlienId++, 2, 1));
        aliens.add(factory.createPulpoAlien(nextAlienId++, 3, 1));
    }

    // Crea un alien en la posicion indicada. Retorna false si la posicion esta ocupada.
    public boolean createAlien(int x, int y, int points) {
        for (Alien alien : aliens) {
            if (alien.isAlive() && alien.getX() == x && alien.getY() == y) {
                return false;
            }
        }
        aliens.add(factory.createAlienByPoints(nextAlienId++, x, y, points));
        return true;
    }

    public Alien getAlienById(int alienId) {
        for (Alien alien : aliens) {
            if (alien.getId() == alienId) return alien;
        }
        return null;
    }

    public boolean allAliensDead() {
        for (Alien alien : aliens) {
            if (alien.isAlive()) return false;
        }
        return true;
    }

    // Mueve todos los aliens. Retorna true si alguno llego a la base (y >= 80).
    public boolean moverAliens() {
        boolean hasAlive = false;
        boolean hitBoundary = false;

        for (Alien alien : aliens) {
            if (!alien.isAlive()) continue;
            hasAlive = true;
            int newX = alien.getX() + alienDirection;
            if (newX <= 0 || newX >= 100) {
                hitBoundary = true;
                break;
            }
        }

        if (!hasAlive) return false;

        if (hitBoundary) {
            alienDirection *= -1;
            for (Alien alien : aliens) {
                if (!alien.isAlive()) continue;
                alien.moveDown();
                if (alien.getY() >= 80) return true;
            }
        } else {
            for (Alien alien : aliens) {
                if (alien.isAlive()) alien.moveHorizontal(alienDirection);
            }
        }
        return false;
    }

    // Resetea para nueva ronda: limpia aliens y direccion, crea aliens iniciales
    public void resetRonda() {
        aliens.clear();
        alienDirection = 1;
        createInitialAliens();
    }

    // Reinicio completo: incluye el contador de IDs
    public void reiniciar() {
        aliens.clear();
        alienDirection = 1;
        nextAlienId = 1;
        createInitialAliens();
    }

    public List<Alien> getAliens() {
        return aliens;
    }
}
