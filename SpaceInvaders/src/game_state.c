// ─────────────────────────────────────────────────────────────────
//  game_state.c — Definición del estado global y funciones básicas
// ─────────────────────────────────────────────────────────────────

#include "../include/game_state.h"
#include "../include/constants.h"
#include <string.h>

// ── Definición de las variables globales ────────────────────────
Jugador     jugadores[2];
int         numJugadores  = 0;

Alien       aliens[MAX_ALIENS];
int         numAliens     = 0;

Bunker      bunkers[MAX_BUNKERS];
int         numBunkers    = 0;

UFO         ufo;
bool        ufoPresente   = false;

int         alienSpeedMs  = 100;
bool        gameOver      = false;

int         miPlayerId    = -1;
bool        conectado     = false;
bool        esperandoId   = true;

Bala        bala          = { .activa = false };
BalaEnemiga balasEnemigas[MAX_BALAS_ENEMIGAS];

float       alienDirX     = 1.0f;
float       alienVelPx    = ALIEN_SPEED_BASE;

int         animTimer     = 0;
int         animFrame     = 0;

bool        alienesReportados = false;

// ── estadoInit ──────────────────────────────────────────────────
// Limpia todo el estado al inicio o al reiniciar
void estadoInit(void)
{
    memset(jugadores,     0, sizeof(jugadores));
    memset(aliens,        0, sizeof(aliens));
    memset(bunkers,       0, sizeof(bunkers));
    memset(balasEnemigas, 0, sizeof(balasEnemigas));

    numJugadores      = 0;
    numAliens         = 0;
    numBunkers        = 0;
    ufoPresente       = false;
    alienSpeedMs      = 100;
    gameOver          = false;
    miPlayerId        = -1;
    conectado         = false;
    esperandoId       = true;
    bala.activa       = false;
    alienDirX         = 1.0f;
    alienVelPx        = ALIEN_SPEED_BASE;
    animTimer         = 0;
    animFrame         = 0;
    alienesReportados = false;
}

// ── miIdx ───────────────────────────────────────────────────────
// Devuelve el índice del jugador local en el array, o -1
int miIdx(void)
{
    for (int i = 0; i < numJugadores; i++)
        if (jugadores[i].id == miPlayerId) return i;
    return -1;
}

// ── velocidadDesdeMs ────────────────────────────────────────────
// Convierte ms del servidor a px/frame para el movimiento local
float velocidadDesdeMs(int ms)
{
    float vel = ALIEN_SPEED_BASE + (1000.0f - (float)ms) * ALIEN_SPEED_SCALE;
    if (vel < 0.5f) vel = 0.5f;
    if (vel > 8.0f) vel = 8.0f;
    return vel;
}

// ── tipoDesdePuntos ─────────────────────────────────────────────
TipoAlien tipoDesdePuntos(int pts)
{
    if (pts <= 10) return TIPO_CALAMAR;
    if (pts <= 20) return TIPO_CANGREJO;
    return TIPO_PULPO;
}
