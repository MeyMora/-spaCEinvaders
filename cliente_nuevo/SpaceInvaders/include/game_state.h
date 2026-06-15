#ifndef GAME_STATE_H
#define GAME_STATE_H

// ─────────────────────────────────────────────────────────────────
//  game_state.h — Structs y estado global compartido entre módulos
// ─────────────────────────────────────────────────────────────────

#include <stdbool.h>

// ── Tipos de alien según puntos ──────────────────────────────────
typedef enum {
    TIPO_CALAMAR = 0,   // 10 pts  — fila superior
    TIPO_CANGREJO,      // 20 pts  — filas medias
    TIPO_PULPO          // 40 pts  — filas inferiores
} TipoAlien;

// ── Structs del juego ────────────────────────────────────────────

typedef struct {
    int       id;
    float     x, y;        // posición en píxeles (movida localmente)
    int       puntos;
    bool      vivo;
    TipoAlien tipo;
    int       frameAnim;
} Alien;

typedef struct {
    int   id;
    float x, y;
    int   vida;            // 0-100
} Bunker;

typedef struct {
    float x, y;
    int   puntos;
    bool  activo;
    bool  dirIzqDer;       // true = izq→der
} UFO;

typedef struct {
    int   id;
    float x;               // posición local en píxeles
    int   vidas;
    int   puntaje;
} Jugador;

typedef struct {
    float x, y;
    bool  activa;
} Bala;

// Bala enemiga disparada por un alien
typedef struct {
    float x, y;
    bool  activa;
    int   alienIdx;        // qué alien la disparó
} BalaEnemiga;

// ── Límite de balas enemigas simultáneas ────────────────────────
#define MAX_BALAS_ENEMIGAS  8

// ── Estado global (declarado extern, definido en game_state.c) ──
#include "../include/constants.h"

extern Jugador      jugadores[2];
extern int          numJugadores;

extern Alien        aliens[MAX_ALIENS];
extern int          numAliens;

extern Bunker       bunkers[MAX_BUNKERS];
extern int          numBunkers;

extern UFO          ufo;
extern bool         ufoPresente;

extern int          alienSpeedMs;
extern bool         gameOver;

extern int          miPlayerId;
extern bool         conectado;
extern bool         esperandoId;  // true mientras estamos en el menu de seleccion de rol

extern Bala         balasJugadores[2];   // una bala por slot de jugador (0 y 1)
extern BalaEnemiga  balasEnemigas[MAX_BALAS_ENEMIGAS];

extern float        alienDirX;
extern float        alienVelPx;

extern int          animTimer;
extern int          animFrame;

extern bool         alienesReportados;

// ── Funciones de estado ──────────────────────────────────────────
void  estadoInit(void);
int   miIdx(void);
float velocidadDesdeMs(int ms);
TipoAlien tipoDesdePuntos(int pts);

#endif // GAME_STATE_H
