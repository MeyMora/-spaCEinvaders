package Modelos;

public class CalamarAlien extends Alien{
    public CalamarAlien(int id, int x, int y, int points){
        super(id, x, y, points);
    }

    @Override
    public String getTipo() {
        return "CALAMAR";
    }
}
