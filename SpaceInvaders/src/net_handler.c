// net_handler.c -- Lectura y parseo de mensajes del servidor Java

#include "../include/net_handler.h"
#include "../include/game_state.h"
#include "../include/network.h"
#include "../include/constants.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// Acumulador de datos incompletos entre frames
static char recvAcum[RECV_BUFFER_SIZE * 4];
static int  recvAcumLen = 0;

// parsearEstado
// Actualiza el estado global con el mensaje STATE: del servidor.
// La posicion X del jugador NUNCA se sobreescribe (la maneja el cliente).
// La posicion X/Y de aliens ya existentes tampoco se toca.
static void parsearEstado(const char *linea)
{
    bool nuevoUfo     = false;
    int  nuevosAliens = 0;

    char buf[RECV_BUFFER_SIZE * 4];
    strncpy(buf, linea, sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = '\0';

    char *tok = strtok(buf, " \t\r\n");
    while (tok != NULL)
    {
        // ── PLAYER <id> <x> <lives> <score> ─────────────────────
        if (strcmp(tok, "PLAYER") == 0)
        {
            int id    = atoi(strtok(NULL, " \t\r\n"));
            int lx    = atoi(strtok(NULL, " \t\r\n")); // posicion logica (ignorada)
            int lives = atoi(strtok(NULL, " \t\r\n"));
            int score = atoi(strtok(NULL, " \t\r\n"));
            (void)lx; // suprimir warning de variable sin usar

            // Buscar si este jugador ya existe en el array
            int existeIdx = -1;
            for (int i = 0; i < numJugadores; i++) {
                if (jugadores[i].id == id) {
                    existeIdx = i;
                    break;
                }
            }

            if (existeIdx >= 0) {
                // Jugador conocido: solo actualizar vidas y puntaje
                // La posicion X NUNCA se modifica aqui — es local del cliente
                jugadores[existeIdx].vidas   = lives;
                jugadores[existeIdx].puntaje = score;
            } else if (numJugadores < 2) {
                // Jugador nuevo: inicializar una sola vez
                int idx = numJugadores++;
                jugadores[idx].id      = id;
                jugadores[idx].vidas   = lives;
                jugadores[idx].puntaje = score;
                // Posicion inicial segun si es el jugador local u otro
                if (id == miPlayerId || miPlayerId == -1)
                    jugadores[idx].x = (float)(SCREEN_WIDTH / 2 - CANON_ANCHO / 2);
                else
                    jugadores[idx].x = (float)(SCREEN_WIDTH * 3 / 4 - CANON_ANCHO / 2);
            }
        }

        // ── ALIEN <id> <x> <y> <pts> <alive> ────────────────────
        else if (strcmp(tok, "ALIEN") == 0)
        {
            int  id    = atoi(strtok(NULL, " \t\r\n"));
            int  lx    = atoi(strtok(NULL, " \t\r\n"));
            int  ly    = atoi(strtok(NULL, " \t\r\n"));
            int  pts   = atoi(strtok(NULL, " \t\r\n"));
            char *alv  = strtok(NULL, " \t\r\n");
            bool alive = (alv && strcmp(alv, "true") == 0);

            // Buscar alien existente
            int existeIdx = -1;
            for (int i = 0; i < numAliens; i++) {
                if (aliens[i].id == id) { existeIdx = i; break; }
            }

            if (existeIdx >= 0) {
                // Alien conocido: solo actualizar estado vivo, NO la posicion
                aliens[existeIdx].vivo   = alive;
                aliens[existeIdx].puntos = pts;
                aliens[existeIdx].tipo   = tipoDesdePuntos(pts);
            } else if (numAliens < MAX_ALIENS) {
                // Alien nuevo: inicializar con posicion del servidor
                int idx = numAliens++;
                aliens[idx].id        = id;
                aliens[idx].puntos    = pts;
                aliens[idx].vivo      = alive;
                aliens[idx].tipo      = tipoDesdePuntos(pts);
                aliens[idx].frameAnim = 0;
                aliens[idx].x = (float)(GAME_AREA_X + lx * (GAME_AREA_W - ALIEN_ANCHO) / 100);
                aliens[idx].y = (float)(GAME_AREA_Y + ly * (GAME_AREA_H - ALIEN_ALTO)  / 100);
            }
            nuevosAliens++;
        }

        // ── BUNKER <id> <health> ─────────────────────────────────
        else if (strcmp(tok, "BUNKER") == 0)
        {
            int id     = atoi(strtok(NULL, " \t\r\n"));
            int health = atoi(strtok(NULL, " \t\r\n"));

            int existeIdx = -1;
            for (int i = 0; i < numBunkers; i++) {
                if (bunkers[i].id == id) { existeIdx = i; break; }
            }

            if (existeIdx >= 0) {
                bunkers[existeIdx].vida = health;
            } else if (numBunkers < MAX_BUNKERS) {
                int idx = numBunkers++;
                bunkers[idx].id   = id;
                bunkers[idx].vida = health;
                int sep = GAME_AREA_W / (MAX_BUNKERS + 1);
                bunkers[idx].x = (float)(GAME_AREA_X + sep * id - BUNKER_ANCHO / 2);
                bunkers[idx].y = (float)BUNKER_Y;
            }
        }

        // ── UFO <dir> <pts> <active> ─────────────────────────────
        else if (strcmp(tok, "UFO") == 0)
        {
            char *dir = strtok(NULL, " \t\r\n");
            int   pts = atoi(strtok(NULL, " \t\r\n"));
            char *act = strtok(NULL, " \t\r\n");
            bool  estaActivo = (act && strcmp(act, "true") == 0);
            nuevoUfo = true;

            if (estaActivo && !ufoPresente) {
                ufo.dirIzqDer = (dir && strcmp(dir, "I-D") == 0);
                ufo.x = ufo.dirIzqDer ? (float)GAME_AREA_X
                                       : (float)(GAME_AREA_X + GAME_AREA_W - UFO_ANCHO);
                ufo.y = (float)UFO_Y;
            }
            ufo.puntos  = pts;
            ufo.activo  = estaActivo;
            ufoPresente = estaActivo;
        }

        // ── SPEED <ms> ───────────────────────────────────────────
        else if (strcmp(tok, "SPEED") == 0)
        {
            alienSpeedMs = atoi(strtok(NULL, " \t\r\n"));
            alienVelPx   = velocidadDesdeMs(alienSpeedMs);
        }

        // ── GAME_OVER <bool> ─────────────────────────────────────
        else if (strcmp(tok, "GAME_OVER") == 0)
        {
            char *val = strtok(NULL, " \t\r\n");
            gameOver = (val && strcmp(val, "true") == 0);
        }

        tok = strtok(NULL, " \t\r\n");
    }

    // Si el servidor ya no reporta UFO, ocultarlo
    if (!nuevoUfo) {
        ufoPresente = false;
        ufo.activo  = false;
    }

    // Si el servidor reinicio y ya no hay aliens, limpiar arrays locales
    // pero CONSERVAR los jugadores (no resetear numJugadores)
    if (nuevosAliens == 0 && numAliens > 0) {
        numAliens  = 0;
        numBunkers = 0;
        // NO tocar numJugadores ni las posiciones X de los jugadores
        alienDirX         = 1.0f;
        alienesReportados = false;
        bala.activa       = false;
        for (int i = 0; i < MAX_BALAS_ENEMIGAS; i++)
            balasEnemigas[i].activa = false;
    }

    if (!gameOver) alienesReportados = false;
    alienVelPx = velocidadDesdeMs(alienSpeedMs);
}

// procesarMensaje
static void procesarMensaje(const char *msg)
{
    if (strncmp(msg, "PLAYER_ID ", 10) == 0) {
        miPlayerId  = atoi(msg + 10);
        esperandoId = false;
        printf("[NET] Soy jugador %d\n", miPlayerId);
    }
    else if (strcmp(msg, "SPECTATOR") == 0) {
        miPlayerId  = -1;
        esperandoId = false;
        printf("[NET] Soy espectador\n");
    }
    else if (strncmp(msg, "STATE:", 6) == 0) {
        parsearEstado(msg);
    }
}

// leerRed
// Lee datos del socket, los acumula y procesa linea por linea
void leerRed(void)
{
    char tmpBuf[RECV_BUFFER_SIZE];
    int  bytes = receiveNonBlocking(tmpBuf, sizeof(tmpBuf));

    if (bytes < 0)  { conectado = false; return; }
    if (bytes == 0) return;

    if (recvAcumLen + bytes < (int)sizeof(recvAcum) - 1) {
        memcpy(recvAcum + recvAcumLen, tmpBuf, bytes);
        recvAcumLen += bytes;
        recvAcum[recvAcumLen] = '\0';
    }

    char *ini = recvAcum;
    char *nl;
    while ((nl = strchr(ini, '\n')) != NULL) {
        *nl = '\0';
        int len = (int)(nl - ini);
        if (len > 0 && ini[len-1] == '\r') ini[len-1] = '\0';
        if (strlen(ini) > 0) procesarMensaje(ini);
        ini = nl + 1;
    }

    int resto = recvAcumLen - (int)(ini - recvAcum);
    if (resto > 0) memmove(recvAcum, ini, resto);
    recvAcumLen = (resto > 0) ? resto : 0;
    recvAcum[recvAcumLen] = '\0';
}