// ─────────────────────────────────────────────────────────────────
//  gameplay.c — Movimiento, disparos de aliens y colisiones
// ─────────────────────────────────────────────────────────────────

#include "../raylib/include/raylib.h"
#include "../include/gameplay.h"
#include "../include/game_state.h"
#include "../include/network.h"
#include "../include/constants.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Timer para el disparo de aliens (en frames)
static int  alienShootTimer = 0;

// ── actualizarAliensLocal ────────────────────────────────────────
void actualizarAliensLocal(void)
{
    // Animación: alterna frame cada ALIEN_ANIM_TICKS frames
    animTimer++;
    if (animTimer >= ALIEN_ANIM_TICKS) {
        animTimer  = 0;
        animFrame ^= 1;
        for (int i = 0; i < numAliens; i++)
            aliens[i].frameAnim = animFrame;
    }

    // Buscar extremos del grupo de aliens vivos
    float xMin =  999999.0f;
    float xMax = -999999.0f;
    bool  hayVivos = false;

    for (int i = 0; i < numAliens; i++) {
        if (!aliens[i].vivo) continue;
        hayVivos = true;
        if (aliens[i].x              < xMin) xMin = aliens[i].x;
        if (aliens[i].x + ALIEN_ANCHO > xMax) xMax = aliens[i].x + ALIEN_ANCHO;
    }

    if (!hayVivos) return;

    // Rebotar en los bordes y bajar un escalón (patrón zig-zag)
    bool rebotar = false;
    if (alienDirX > 0 && xMax >= GAME_AREA_X + GAME_AREA_W) rebotar = true;
    if (alienDirX < 0 && xMin <= GAME_AREA_X)                rebotar = true;

    if (rebotar) {
        alienDirX *= -1.0f;
        for (int i = 0; i < numAliens; i++)
            if (aliens[i].vivo) aliens[i].y += ALIEN_STEP_DOWN;
    } else {
        for (int i = 0; i < numAliens; i++)
            if (aliens[i].vivo) aliens[i].x += alienVelPx * alienDirX;
    }

    // Mover OVNI
    if (ufoPresente && ufo.activo) {
        float velDir = ufo.dirIzqDer ? UFO_VELOCIDAD : -UFO_VELOCIDAD;
        ufo.x += velDir;
        if (ufo.x > GAME_AREA_X + GAME_AREA_W || ufo.x < GAME_AREA_X - UFO_ANCHO) {
            ufoPresente = false;
            ufo.activo  = false;
        }
    }

    // Detectar si los aliens alcanzaron el cañón
    if (!alienesReportados) {
        for (int i = 0; i < numAliens; i++) {
            if (aliens[i].vivo && aliens[i].y + ALIEN_ALTO >= CANON_Y) {
                alienesReportados = true;
                sendMessage(MSG_ALIENS_BASE);
                break;
            }
        }
    }
}

// ── actualizarBalaLocal ──────────────────────────────────────────
void actualizarBalaLocal(void)
{
    if (!bala.activa) return;
    bala.y -= BALA_VELOCIDAD;
    if (bala.y + BALA_ALTO < GAME_AREA_Y) bala.activa = false;
}

// ── actualizarDisparoAliens ──────────────────────────────────────
// Cada ALIEN_SHOOT_INTERVAL frames, un alien vivo aleatorio dispara
void actualizarDisparoAliens(void)
{
    alienShootTimer++;
    if (alienShootTimer < ALIEN_SHOOT_INTERVAL) return;
    alienShootTimer = 0;

    // Recopilar índices de aliens vivos
    int vivos[MAX_ALIENS];
    int totalVivos = 0;
    for (int i = 0; i < numAliens; i++)
        if (aliens[i].vivo) vivos[totalVivos++] = i;

    if (totalVivos == 0) return;

    // Elegir uno al azar
    int elegido = vivos[rand() % totalVivos];

    // Buscar ranura libre de bala enemiga
    for (int i = 0; i < MAX_BALAS_ENEMIGAS; i++) {
        if (!balasEnemigas[i].activa) {
            balasEnemigas[i].activa   = true;
            balasEnemigas[i].x        = aliens[elegido].x + ALIEN_ANCHO / 2.0f - BALA_ANCHO / 2.0f;
            balasEnemigas[i].y        = aliens[elegido].y + ALIEN_ALTO;
            balasEnemigas[i].alienIdx = elegido;
            break;
        }
    }
}

// ── actualizarBalasEnemigas ──────────────────────────────────────
// Mueve balas enemigas hacia abajo; detecta impactos en jugador y bunkers
void actualizarBalasEnemigas(void)
{
    int idx = miIdx();

    for (int i = 0; i < MAX_BALAS_ENEMIGAS; i++)
    {
        if (!balasEnemigas[i].activa) continue;

        balasEnemigas[i].y += BALA_ENEMIGA_VEL;

        // Salió de la pantalla
        if (balasEnemigas[i].y > LINEA_INF_Y) {
            balasEnemigas[i].activa = false;
            continue;
        }

        Rectangle rBE = { balasEnemigas[i].x, balasEnemigas[i].y,
                          (float)BALA_ANCHO, (float)BALA_ALTO };

        // Impacto en bunker
        bool impacto = false;
        for (int b = 0; b < numBunkers && !impacto; b++) {
            if (bunkers[b].vida <= 0) continue;
            Rectangle rBk = { bunkers[b].x, bunkers[b].y,
                              (float)BUNKER_ANCHO, (float)BUNKER_ALTO };
            if (CheckCollisionRecs(rBE, rBk)) {
                balasEnemigas[i].activa = false;
                bunkers[b].vida -= 10;
                if (bunkers[b].vida < 0) bunkers[b].vida = 0;
                char msg[SEND_BUFFER_SIZE];
                snprintf(msg, sizeof(msg), "BUNKER HIT %d\n", bunkers[b].id);
                sendMessage(msg);
                impacto = true;
            }
        }
        if (impacto) continue;

        // Impacto en jugador local
        if (miPlayerId > 0 && idx >= 0) {
            Rectangle rP = { jugadores[idx].x, (float)CANON_Y,
                             (float)CANON_ANCHO, (float)(CANON_ALTO_BASE + CANON_ALTO_CANON) };
            if (CheckCollisionRecs(rBE, rP)) {
                balasEnemigas[i].activa = false;
                jugadores[idx].vidas--;
                if (jugadores[idx].vidas < 0) jugadores[idx].vidas = 0;
                sendMessage(MSG_PLAYER_HIT);
            }
        }
    }
}

// ── detectarColisiones ───────────────────────────────────────────
// Colisiones de la bala del jugador contra aliens, UFO y bunkers
void detectarColisiones(void)
{
    if (!bala.activa) return;

    Rectangle rBala = { bala.x, bala.y, (float)BALA_ANCHO, (float)BALA_ALTO };

    // Bala vs Alien
    for (int i = 0; i < numAliens; i++) {
        if (!aliens[i].vivo) continue;
        Rectangle rA = { aliens[i].x, aliens[i].y,
                         (float)ALIEN_ANCHO, (float)ALIEN_ALTO };
        if (CheckCollisionRecs(rBala, rA)) {
            aliens[i].vivo = false;
            bala.activa    = false;
            char msg[SEND_BUFFER_SIZE];
            snprintf(msg, sizeof(msg), "ALIEN_KILLED %d\n", aliens[i].id);
            sendMessage(msg);
            return;
        }
    }

    // Bala vs UFO
    if (ufoPresente && ufo.activo) {
        Rectangle rU = { ufo.x, (float)UFO_Y, (float)UFO_ANCHO, (float)UFO_ALTO };
        if (CheckCollisionRecs(rBala, rU)) {
            ufo.activo  = false;
            ufoPresente = false;
            bala.activa = false;
            sendMessage("UFO KILLED\n");
            return;
        }
    }

    // Bala vs Bunker
    for (int b = 0; b < numBunkers; b++) {
        if (bunkers[b].vida <= 0) continue;
        Rectangle rBk = { bunkers[b].x, bunkers[b].y,
                          (float)BUNKER_ANCHO, (float)BUNKER_ALTO };
        if (CheckCollisionRecs(rBala, rBk)) {
            bala.activa = false;
            bunkers[b].vida -= 10;
            if (bunkers[b].vida < 0) bunkers[b].vida = 0;
            char msg[SEND_BUFFER_SIZE];
            snprintf(msg, sizeof(msg), "BUNKER HIT %d\n", bunkers[b].id);
            sendMessage(msg);
            return;
        }
    }
}