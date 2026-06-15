package Modelos;

public class UFO {

    private int id;
    private int x;
    private int y;
    private String direction;
    private int points;
    private boolean active;

    public UFO(int id, int x, int y, String direction, int points) {
        this.id = id;
        this.x = x;
        this.y = y;
        this.direction = direction;
        this.points = points;
        this.active = true;
    }

    public int getID(){
        return id;
    }

    public int getX(){
        return x;
    }
    public int getY(){
        return y;
    }

    public String getDirection() {
        return direction;
    }

    public int getPoints() {
        return points;
    }

    public boolean isActive() {
        return active;
    }

    public void move(){
        if (direction.equals("I-D")){
            x++;
        }else if(direction.equals("D-I")){
            active = false;
        }
    }

    public void destroy() {
        active = false;
    }

    public String toMessage() {
        return "UFO " + direction + " " + points + " " + active;
    }
}