package Patrones.AbstractFactory;

import Modelos.Alien;
import Modelos.Bunker;
import Modelos.CalamarAlien;
import Modelos.CangrejoAlien;
import Modelos.PulpoAlien;
import Modelos.UFO;

public class ClassicGameElementFactory extends GameElementFactory {

    @Override
    public Alien createCalamarAlien(int id, int x, int y) {
        return new CalamarAlien(id, x, y);
    }

    @Override
    public Alien createCangrejoAlien(int id, int x, int y) {
        return new CangrejoAlien(id, x, y);
    }

    @Override
    public Alien createPulpoAlien(int id, int x, int y) {
        return new PulpoAlien(id, x, y);
    }

    @Override
    public UFO createUFO(int id, int x, int y, String direccion, int puntos) {
        return new UFO(id, x, y, direccion, puntos);
    }

    @Override
    public Bunker createBunker(int id, int x, int y) {
        return new Bunker(id, x, y, 100);
    }
}