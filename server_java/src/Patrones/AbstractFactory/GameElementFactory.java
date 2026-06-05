package Patrones.AbstractFactory;


import Modelos.Alien;
import Modelos.Bunker;
import Modelos.UFO;

public abstract class GameElementFactory {
    public abstract Alien createCalamarAlien(int id, int x, int y);

    public abstract Alien createCangrejoAlien(int id, int x, int y);

    public abstract Alien createPulpoAlien(int id, int x, int y);

    public abstract Alien createAlienByPoints(int id, int x, int y, int puntos);

    public abstract UFO createUFO(int id, int x, int y, String direccion, int puntos);

    public abstract Bunker createBunker(int id, int x, int y);
}
