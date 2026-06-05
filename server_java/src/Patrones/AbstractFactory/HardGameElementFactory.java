package Patrones.AbstractFactory;

import Modelos.Alien;
import Modelos.Bunker;
import Modelos.CalamarAlien;
import Modelos.CangrejoAlien;
import Modelos.PulpoAlien;
import Modelos.UFO;

public class HardGameElementFactory extends GameElementFactory {

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
    public Alien createAlienByPoints(int id, int x, int y, int puntos) {
        switch (puntos) {
            case 20:
                return createCalamarAlien(id, x, y);
            case 40:
                return createCangrejoAlien(id, x, y);
            case 60:
                return createPulpoAlien(id, x, y);
            default:
                throw new IllegalArgumentException("Puntos de alien no válidos: " + puntos);
        }
    }

    @Override
    public UFO createUFO(int id,int x, int y , String direccion, int puntos) {
        // En modo difícil podrían aparecer ovnis con más puntos.
        return new UFO(id, x, y,  direccion, puntos);
    }

    @Override
    public Bunker createBunker(int id, int x, int y) {
        // En modo difícil los bunkers empiezan más dañados.
        return new Bunker(id, x, y, 70);
    }
}