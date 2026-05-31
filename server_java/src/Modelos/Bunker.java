package Modelos;

public class Bunker {
    private int id; // Variable id para identificar cual bunker es, si el 1, 2,3...
    private int health; //Variable privada que es la vida actual del bunker

    public Bunker(int id){  //Constructor de la clase de modelos.Bunker
        this.id = id; // Se asigna el identificador del bunker.
        this.health = 100; // la vida del bunker siempre inicia en 100
    }
    //Metodo que devuelve el id del bunker
    public int getId() {
        return id;
    }
    // Metodo que devuelve la vida actual del bunker
    public int getHealth() {
        return health;
    }
    // Metodo que disminuye la vida del bunker según el daño recibido.
    public void damage(int damage) {
        health -= damage; // Se resta el daño a la vida actual.
        if (health <= 0) { // Si la vida llega a 0 o menos, se mantiene en 0.
            health = 0;
        }
    }
    //Metodo que devuelve un texto con la informacion del bunker, retorna una cadena de texto
    public String toMessage() {
        return "BUNKER " + id + " " + health;
    }

    public void setHealth(int health) {
        if (health < 0) {
            this.health = 0;
        } else if (health > 100) {
            this.health = 100;
        } else {
            this.health = health;
        }
    }
}
