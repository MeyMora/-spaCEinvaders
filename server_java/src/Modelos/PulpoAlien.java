package Modelos;

public class PulpoAlien extends Alien {

    public PulpoAlien(int id, int x, int y) {
        super(id, x, y, 40);
    }

    @Override
    public String getTipo() {
        return "Pulpo";
    }
}