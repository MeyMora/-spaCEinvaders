package Modelos;

public class PulpoAlien extends Alien {

    public PulpoAlien(int id, int x, int y, int points) {
        super(id, x, y, points);
    }

    @Override
    public String getTipo() {
        return "Pulpo";
    }
}