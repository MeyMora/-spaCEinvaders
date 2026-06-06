package Patrones.AbstractFactory;


import Modelos.Alien;
import Modelos.Bunker;
import Modelos.UFO;

public abstract class GameElementFactory {
    public abstract Alien createCalamarAlien(int id, int x, int y);

    public abstract Alien createCangrejoAlien(int id, int x, int y);

    public abstract Alien createPulpoAlien(int id, int x, int y);

    public abstract UFO createUFO(int id, int x, int y, String direccion, int puntos);

    public abstract Bunker createBunker(int id, int x, int y);

    // Método concreto compartido: delega a los métodos abstractos de cada fábrica concreta
    public Alien createAlienByPoints(int id, int x, int y, int puntos) {
        switch (puntos) {
            case 10: return createCalamarAlien(id, x, y);
            case 20: return createCangrejoAlien(id, x, y);
            case 40: return createPulpoAlien(id, x, y);
            default: throw new IllegalArgumentException("Puntos de alien no válidos: " + puntos);
        }
    }
}
