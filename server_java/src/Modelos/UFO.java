package Modelos;

public class UFO {
    private String direction;
    private int points;
    private boolean active;

    public UFO(String direction, int points) {
        this.direction = direction;
        this.points = points;
        this.active = true;
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

    public void destroy() {
        active = false;
    }

    public String toMessage() {
        return "UFO " + direction + " " + points + " " + active;
    }
}