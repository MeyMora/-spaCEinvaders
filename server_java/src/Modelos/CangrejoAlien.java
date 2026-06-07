package Modelos;

public class CangrejoAlien extends Alien {

    public CangrejoAlien(int id, int x, int y, int points) {
        super(id, x, y, points);
    }

    @Override
    public String getTipo() {
        return "Cangrejo";
    }
}