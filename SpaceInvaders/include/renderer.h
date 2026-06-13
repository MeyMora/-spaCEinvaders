#ifndef RENDERER_H
#define RENDERER_H

// ─────────────────────────────────────────────────────────────────
//  renderer.h — Todas las funciones de dibujo con Raylib
// ─────────────────────────────────────────────────────────────────

#include "../include/game_state.h"

void dibujarAlien(int px, int py, TipoAlien tipo, int frame, Color color);
void dibujarBunker(int px, int py, int vida);
void dibujarCanon(float cx, float cy);
void dibujarUFO(int px, int py);
void dibujarVidas(int vidas, int x, int y);
void dibujarHUD(void);
void dibujarPantallaEspera(int frames);
void dibujarGameOver(void);
void dibujarEscena(void);       // dibuja todo el juego activo
void dibujarSinConexion(void);

#endif // RENDERER_H
