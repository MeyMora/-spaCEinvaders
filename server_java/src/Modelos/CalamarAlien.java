package Modelos;

public class CalamarAlien extends Alien{
    public CalamarAlien(int id, int x, int y){
        super(id, x, y, 10 );
    }

    @Override
    public String getTipo() {
        return "CALAMAR";
    }
}
