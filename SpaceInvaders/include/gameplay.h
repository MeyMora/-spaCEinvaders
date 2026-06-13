#ifndef GAMEPLAY_H
#define GAMEPLAY_H

// ─────────────────────────────────────────────────────────────────
//  gameplay.h — Lógica de movimiento, disparo y colisiones
// ─────────────────────────────────────────────────────────────────

// Mueve los aliens en zig-zag y el OVNI
void actualizarAliensLocal(void);

// Mueve la bala del jugador hacia arriba
void actualizarBalaLocal(void);

// Lógica de disparo de aliens: disparan cada cierto tiempo
void actualizarDisparoAliens(void);

// Mueve las balas enemigas y detecta impactos en jugador/bunkers
void actualizarBalasEnemigas(void);

// Detecta colisiones de la bala del jugador con aliens/UFO/bunkers
void detectarColisiones(void);

#endif // GAMEPLAY_H
