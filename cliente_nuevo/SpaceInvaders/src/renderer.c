// ─────────────────────────────────────────────────────────────────
//  renderer.c — Dibujo de todos los elementos del juego con Raylib
// ─────────────────────────────────────────────────────────────────

#include "../raylib/include/raylib.h"
#include "../include/renderer.h"
#include "../include/game_state.h"
#include "../include/constants.h"

#include <stdio.h>
#include <string.h>

// ── dibujarAlien ────────────────────────────────────────────────
// Sprites pixel-art 8×8/8×6 macro-píxeles con 2 frames de anim.
void dibujarAlien(int px, int py, TipoAlien tipo, int frame, Color color)
{
    int s = 4;

    switch (tipo)
    {
        // CALAMAR — 10 pts
        case TIPO_CALAMAR:
        {
            static const int A[8][8] = {
                {0,0,0,1,1,0,0,0},
                {0,0,1,1,1,1,0,0},
                {0,1,1,1,1,1,1,0},
                {1,1,0,1,1,0,1,1},
                {1,1,1,1,1,1,1,1},
                {0,0,1,0,0,1,0,0},
                {0,1,0,1,1,0,1,0},
                {1,0,1,0,0,1,0,1},
            };
            static const int B[8][8] = {
                {0,0,0,1,1,0,0,0},
                {0,0,1,1,1,1,0,0},
                {0,1,1,1,1,1,1,0},
                {1,1,0,1,1,0,1,1},
                {1,1,1,1,1,1,1,1},
                {0,1,0,0,0,0,1,0},
                {0,0,1,1,1,1,0,0},
                {0,1,0,0,0,0,1,0},
            };
            const int (*g)[8] = (frame == 0) ? A : B;
            for (int r = 0; r < 8; r++)
                for (int c = 0; c < 8; c++)
                    if (g[r][c]) DrawRectangle(px+c*s, py+r*s, s, s, color);
            break;
        }

        // CANGREJO — 20 pts
        case TIPO_CANGREJO:
        {
            static const int A[6][8] = {
                {0,1,0,0,0,0,1,0},
                {0,0,1,0,0,1,0,0},
                {0,1,1,1,1,1,1,0},
                {1,1,0,1,1,0,1,1},
                {1,1,1,1,1,1,1,1},
                {1,0,1,0,0,1,0,1},
            };
            static const int B[6][8] = {
                {0,1,0,0,0,0,1,0},
                {1,0,1,0,0,1,0,1},
                {1,1,1,1,1,1,1,1},
                {0,1,0,1,1,0,1,0},
                {0,0,1,0,0,1,0,0},
                {0,1,0,0,0,0,1,0},
            };
            const int (*g)[8] = (frame == 0) ? A : B;
            for (int r = 0; r < 6; r++)
                for (int c = 0; c < 8; c++)
                    if (g[r][c]) DrawRectangle(px+c*s, py+r*s, s, s, color);
            break;
        }

        // PULPO — 40 pts
        case TIPO_PULPO:
        {
            static const int A[6][8] = {
                {0,0,1,1,1,1,0,0},
                {0,1,1,1,1,1,1,0},
                {1,1,0,1,1,0,1,1},
                {1,1,1,1,1,1,1,1},
                {1,0,1,0,0,1,0,1},
                {0,0,1,0,0,1,0,0},
            };
            static const int B[6][8] = {
                {0,0,1,1,1,1,0,0},
                {0,1,1,1,1,1,1,0},
                {1,0,1,1,1,1,0,1},
                {1,0,0,1,1,0,0,1},
                {0,1,0,0,0,0,1,0},
                {0,0,1,0,0,1,0,0},
            };
            const int (*g)[8] = (frame == 0) ? A : B;
            for (int r = 0; r < 6; r++)
                for (int c = 0; c < 8; c++)
                    if (g[r][c]) DrawRectangle(px+c*s, py+r*s, s, s, color);
            break;
        }
    }
}

// ── dibujarBunker ────────────────────────────────────────────────
// 4 sprites distintos según % de vida (100/75/50/25)
void dibujarBunker(int px, int py, int vida)
{
    if (vida <= 0) return;

    Color c;
    if      (vida > 75) c = COLOR_BUNKER;
    else if (vida > 50) c = COLOR_BUNKER_MED;
    else if (vida > 25) c = COLOR_BUNKER_LOW;
    else                c = COLOR_BUNKER_CRIT;

    int s = 6;

    static const int sp100[7][8] = {
        {0,0,1,1,1,1,0,0},
        {0,1,1,1,1,1,1,0},
        {1,1,1,1,1,1,1,1},
        {1,1,1,1,1,1,1,1},
        {1,1,1,0,0,1,1,1},
        {1,1,0,0,0,0,1,1},
        {1,1,0,0,0,0,1,1},
    };
    static const int sp75[7][8] = {
        {0,0,0,1,1,0,0,0},
        {0,0,1,1,1,1,0,0},
        {0,1,1,1,1,1,1,0},
        {1,1,1,1,1,1,1,1},
        {1,1,1,0,0,1,1,1},
        {1,1,0,0,0,0,1,1},
        {1,1,0,0,0,0,1,1},
    };
    static const int sp50[7][8] = {
        {0,0,0,0,0,0,0,0},
        {0,0,0,1,1,0,0,0},
        {0,0,1,1,1,1,0,0},
        {0,1,1,1,1,1,1,0},
        {1,1,0,0,0,0,1,1},
        {1,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,1},
    };
    static const int sp25[7][8] = {
        {0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0},
        {0,0,0,1,1,0,0,0},
        {0,0,1,0,0,1,0,0},
        {0,1,0,0,0,0,1,0},
        {0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0},
    };

    const int (*sp)[8];
    if      (vida > 75) sp = sp100;
    else if (vida > 50) sp = sp75;
    else if (vida > 25) sp = sp50;
    else                sp = sp25;

    for (int r = 0; r < 7; r++)
        for (int col = 0; col < 8; col++)
            if (sp[r][col]) DrawRectangle(px+col*s, py+r*s, s, s, c);
}

// ── dibujarCanon ────────────────────────────────────────────────
void dibujarCanon(float cx, float cy)
{
    DrawRectangle((int)cx, (int)cy, CANON_ANCHO, CANON_ALTO_BASE, COLOR_CANON);
    DrawRectangle((int)cx + CANON_ANCHO/2 - 4, (int)cy - CANON_ALTO_CANON,
                  8, CANON_ALTO_CANON + 4, COLOR_CANON);
}

// ── dibujarUFO ──────────────────────────────────────────────────
void dibujarUFO(int px, int py)
{
    DrawEllipse(px + UFO_ANCHO/2, py + 14, UFO_ANCHO/2, 8, COLOR_UFO);
    DrawEllipse(px + UFO_ANCHO/2, py + 10, UFO_ANCHO/4, 7, Fade(COLOR_UFO, 0.7f));
    DrawCircle(px + 12,           py + 14, 3, WHITE);
    DrawCircle(px + UFO_ANCHO/2,  py + 14, 3, WHITE);
    DrawCircle(px + UFO_ANCHO-12, py + 14, 3, WHITE);
}

// ── dibujarVidas ────────────────────────────────────────────────
// Muestra hasta 3 iconos de cañón (máximo real del juego)
void dibujarVidas(int vidas, int x, int y)
{
    if (vidas > 3) vidas = 3;   // nunca mostrar más de 3
    for (int v = 0; v < vidas; v++) {
        int ox = x + v * 28;
        DrawRectangle(ox,     y+6, 22, 8,  COLOR_CANON);
        DrawRectangle(ox + 7, y,   8,  10, COLOR_CANON);
    }
}

// ── dibujarHUD ──────────────────────────────────────────────────
void dibujarHUD(void)
{
    int idx = miIdx();

    DrawText("SCORE", 20, 14, 22, COLOR_UI);
    char buf[48];
    snprintf(buf, sizeof(buf), "%d", (idx >= 0) ? jugadores[idx].puntaje : 0);
    DrawText(buf, 100, 14, 22, COLOR_UI);

    DrawText("LIVES", SCREEN_WIDTH - 170, 14, 22, COLOR_UI);
    int lives = (idx >= 0) ? jugadores[idx].vidas : 0;
    dibujarVidas(lives, SCREEN_WIDTH - 100, 10);

    if (numJugadores >= 2) {
        int otro = (idx == 0) ? 1 : 0;
        snprintf(buf, sizeof(buf), "P%d: %d",
                 jugadores[otro].id, jugadores[otro].puntaje);
        DrawText(buf, SCREEN_WIDTH/2 - MeasureText(buf,18)/2, 16, 18, LIGHTGRAY);
    }

    if (miPlayerId < 0)
        DrawText("ESPECTADOR",
                 SCREEN_WIDTH/2 - MeasureText("ESPECTADOR",20)/2,
                 16, 20, YELLOW);

    if (miPlayerId > 0)
        DrawText("A/D: mover  |  ESPACIO: disparar",
                 10, SCREEN_HEIGHT-26, 15, Fade(WHITE, 0.35f));
}

// ── dibujarEscena ───────────────────────────────────────────────
// Dibuja todos los elementos cuando el juego está activo
void dibujarEscena(void)
{
    // Línea verde inferior
    DrawRectangle(0, LINEA_INF_Y, SCREEN_WIDTH, 3, COLOR_LINEA);

    // Bunkers
    for (int b = 0; b < numBunkers; b++)
        dibujarBunker((int)bunkers[b].x, (int)bunkers[b].y, bunkers[b].vida);

    // Aliens (colores por tipo)
    Color colAlien[3] = { COLOR_ALIEN_1, COLOR_ALIEN_2, COLOR_ALIEN_3 };
    for (int a = 0; a < numAliens; a++) {
        if (!aliens[a].vivo) continue;
        dibujarAlien((int)aliens[a].x, (int)aliens[a].y,
                     aliens[a].tipo, aliens[a].frameAnim,
                     colAlien[(int)aliens[a].tipo]);
    }

    // UFO
    if (ufoPresente && ufo.activo)
        dibujarUFO((int)ufo.x, UFO_Y);

    // Cañones de todos los jugadores
    for (int p = 0; p < numJugadores; p++)
        dibujarCanon(jugadores[p].x, (float)CANON_Y);

    // Balas de los jugadores (propia y/o del companero/observado)
    for (int p = 0; p < 2; p++) {
        if (!balasJugadores[p].activa) continue;
        DrawRectangle((int)balasJugadores[p].x, (int)balasJugadores[p].y,
                      BALA_ANCHO, BALA_ALTO, COLOR_BALA);
    }

    // Balas enemigas (color rojo claro)
    for (int i = 0; i < MAX_BALAS_ENEMIGAS; i++) {
        if (!balasEnemigas[i].activa) continue;
        DrawRectangle((int)balasEnemigas[i].x, (int)balasEnemigas[i].y,
                      BALA_ANCHO, BALA_ALTO, COLOR_BALA_ENEMIGA);
    }

    dibujarHUD();
}

// ── dibujarSinConexion ───────────────────────────────────────────
void dibujarSinConexion(void)
{
    const char *err = "No se pudo conectar al servidor";
    DrawText(err, SCREEN_WIDTH/2 - MeasureText(err,26)/2,
             SCREEN_HEIGHT/2 - 20, 26, RED);
    DrawText("Asegurate de que el servidor Java este corriendo",
             SCREEN_WIDTH/2 - MeasureText("Asegurate de que el servidor Java este corriendo",18)/2,
             SCREEN_HEIGHT/2 + 20, 18, GRAY);
}

// ── dibujarPantallaEspera ────────────────────────────────────────
void dibujarPantallaEspera(int frames)
{
    int af = (frames / WAIT_BLINK_RATE) % 2;

    dibujarAlien(SCREEN_WIDTH/2 - ALIEN_ANCHO/2, SCREEN_HEIGHT/2 - 90,
                 TIPO_CALAMAR, af, COLOR_UI);

    const char *title = "spaCEinvaders";
    DrawText(title, SCREEN_WIDTH/2 - MeasureText(title,46)/2,
             SCREEN_HEIGHT/2 - 40, 46, COLOR_UI);

    if (af == 0) {
        const char *msg = "Esperando al servidor...";
        DrawText(msg, SCREEN_WIDTH/2 - MeasureText(msg,22)/2,
                 SCREEN_HEIGHT/2 + 30, 22, COLOR_WAIT_TEXT);
    }

    for (int i = 0; i < 5; i++) {
        int ax = SCREEN_WIDTH/2 - 5*(ALIEN_ANCHO+8)/2 + i*(ALIEN_ANCHO+8);
        dibujarAlien(ax, SCREEN_HEIGHT/2 + 70,
                     (TipoAlien)(i % 3), af, Fade(COLOR_UI, 0.3f));
    }

    DrawText("Conectado a " SERVER_IP,
             10, SCREEN_HEIGHT-26, 16, Fade(WHITE, 0.4f));
}

// ── dibujarGameOver ─────────────────────────────────────────────
void dibujarGameOver(void)
{
    int idx     = miIdx();
    int puntaje = (idx >= 0) ? jugadores[idx].puntaje : 0;

    const char *go = "GAME OVER";
    DrawText(go, SCREEN_WIDTH/2 - MeasureText(go,64)/2,
             SCREEN_HEIGHT/2 - 80, 64, RED);

    char pbuf[64];
    snprintf(pbuf, sizeof(pbuf), "Puntaje: %d", puntaje);
    DrawText(pbuf, SCREEN_WIDTH/2 - MeasureText(pbuf,30)/2,
             SCREEN_HEIGHT/2 + 10, 30, COLOR_UI);

    const char *hint = "Esperando que el administrador reinicie el juego...";
    DrawText(hint, SCREEN_WIDTH/2 - MeasureText(hint,18)/2,
             SCREEN_HEIGHT/2 + 60, 18, Fade(WHITE, 0.5f));
}
