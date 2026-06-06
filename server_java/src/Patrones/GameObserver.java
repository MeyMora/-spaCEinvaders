package Patrones;

// Convertimos el Observer en una clase abstracta para cumplir el requisito
// de no usar interfaces en la implementación de patrones solicitada por el profesor.
public abstract class GameObserver {
    // Método que deben implementar los observadores concretos
    public abstract void update(String gameStateMessage);
}