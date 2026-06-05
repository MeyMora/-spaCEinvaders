package Patrones;

import java.util.ArrayList;
import java.util.List;

public abstract class GameSubject {
    protected List<GameObserver> observers;

    public GameSubject() {
        this.observers = new ArrayList<>();
    }

    public void addObserver(GameObserver observer) {
        observers.add(observer);
    }

    public void removeObserver(GameObserver observer) {
        observers.remove(observer);
    }

    public abstract void notifyObservers();
}
