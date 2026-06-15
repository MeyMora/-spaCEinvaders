// net_handler.c -- Lectura y parseo de mensajes del servidor Java

#include "../include/net_handler.h"
#include "../include/game_state.h"
#include "../include/network.h"
#include "../include/constants.h"
#include "../include/menu.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// Acumulador de datos incompletos entre frames
static char recvAcum[RECV_BUFFER_SIZE * 4];
static int  recvAcumLen = 0;

// Velocidad reportada en el STATE: anterior, para detectar cambio de ronda
static int speedAnterior = -1;

// parsearEstado
//
// ESTRATEGIA:
//   - Los ALIEN se reconstruyen SIEMPRE en un array temporal nuevo,
//     en el MISMO ORDEN en que llegan del servidor.
//   - Si el alien con ese ID ya existia en el array viejo Y seguia
//     vivo, conservamos su posicion actual (para no romper la animacion
//     en pleno movimiento).
//   - Si el alien es nuevo, o si CAMBIO DE RONDA (detectado por un
//     cambio en SPEED), se posiciona con las coordenadas que manda
//     el servidor -- asi la formacion nueva aparece igual que al inicio.
//   - La posicion X del jugador NUNCA se sobreescribe (la maneja el cliente).
static void parsearEstado(const char *linea)
{
    bool nuevoUfo = false;

    // Array temporal para reconstruir los aliens de este frame
    static Alien aliensTmp[MAX_ALIENS];
    int numAliensTmp = 0;

    char buf[RECV_BUFFER_SIZE * 4];
    strncpy(buf, linea, sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = '\0';

    // ── Primero detectamos si el SPEED de este mensaje cambio respecto
    //    al anterior. Si cambio, es una nueva ronda y los aliens deben
    //    reposicionarse con las coordenadas del servidor.
    bool rondaNueva = false;
    {
        char bufSpeed[RECV_BUFFER_SIZE * 4];
        strncpy(bufSpeed, linea, sizeof(bufSpeed) - 1);
        bufSpeed[sizeof(bufSpeed) - 1] = '\0';
        char *t = strtok(bufSpeed, " \t\r\n");
        while (t != NULL) {
            if (strcmp(t, "SPEED") == 0) {
                int sp = atoi(strtok(NULL, " \t\r\n"));
                if (speedAnterior != -1 && sp != speedAnterior) {
                    rondaNueva = true;
                }
                speedAnterior = sp;
                break;
            }
            t = strtok(NULL, " \t\r\n");
        }
    }

    char *tok = strtok(buf, " \t\r\n");
    while (tok != NULL)
    {
        // ── PLAYER <id> <x> <lives> <score> ─────────────────────
        if (strcmp(tok, "PLAYER") == 0)
        {
            int id    = atoi(strtok(NULL, " \t\r\n"));
            int lx    = atoi(strtok(NULL, " \t\r\n")); // posicion logica del servidor (0-100)
            int lives = atoi(strtok(NULL, " \t\r\n"));
            int score = atoi(strtok(NULL, " \t\r\n"));

            int existeIdx = -1;
            for (int i = 0; i < numJugadores; i++) {
                if (jugadores[i].id == id) { existeIdx = i; break; }
            }

            // Posicion en pixeles segun la X logica del servidor (0-100)
            float xServidor = (float)(GAME_AREA_X + lx * (GAME_AREA_W - CANON_ANCHO) / 100);

            if (existeIdx >= 0) {
                jugadores[existeIdx].vidas   = lives;
                jugadores[existeIdx].puntaje = score;

                // El ESPECTADOR no tiene movimiento local propio para ningun
                // jugador, asi que siempre sincroniza la X con el servidor.
                // Los jugadores activos manejan su propia X localmente y
                // solo usan la del servidor para el OTRO jugador (compañero).
                if (miPlayerId == -1 || id != miPlayerId) {
                    jugadores[existeIdx].x = xServidor;
                }
            } else if (numJugadores < 2) {
                int idx = numJugadores++;
                jugadores[idx].id      = id;
                jugadores[idx].vidas   = lives;
                jugadores[idx].puntaje = score;
                // Posicion inicial: la que reporte el servidor
                jugadores[idx].x = xServidor;
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

            // Posicion por defecto: la que manda el servidor (coordenadas logicas)
            float px = (float)(GAME_AREA_X + lx * (GAME_AREA_W - ALIEN_ANCHO) / 100);
            float py = (float)(GAME_AREA_Y + ly * (GAME_AREA_H - ALIEN_ALTO)  / 100);
            int   frameAnim = 0;

            // Si NO es ronda nueva, buscar este ID en el array VIEJO para
            // conservar su posicion local (animacion en curso)
            if (!rondaNueva) {
                for (int i = 0; i < numAliens; i++) {
                    if (aliens[i].id == id && aliens[i].vivo) {
                        px = aliens[i].x;
                        py = aliens[i].y;
                        frameAnim = aliens[i].frameAnim;
                        break;
                    }
                }
            }

            if (numAliensTmp < MAX_ALIENS) {
                int idx = numAliensTmp++;
                aliensTmp[idx].id        = id;
                aliensTmp[idx].puntos    = pts;
                aliensTmp[idx].vivo      = alive;
                aliensTmp[idx].tipo      = tipoDesdePuntos(pts);
                aliensTmp[idx].frameAnim = frameAnim;
                aliensTmp[idx].x         = px;
                aliensTmp[idx].y         = py;
            }
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

        // ── SHOT <id> ──────────────────────────────────────────────
        // Un jugador disparo en el ultimo ciclo. Si NO soy yo quien
        // disparo (soy espectador, o es el companero), activo su bala
        // visualmente desde su posicion actual.
        else if (strcmp(tok, "SHOT") == 0)
        {
            int shooterId = atoi(strtok(NULL, " \t\r\n"));

            if (shooterId != miPlayerId) {
                // Buscar el indice de ese jugador en el array local
                for (int i = 0; i < numJugadores; i++) {
                    if (jugadores[i].id == shooterId) {
                        if (!balasJugadores[i].activa) {
                            balasJugadores[i].x = jugadores[i].x + CANON_ANCHO / 2.0f - BALA_ANCHO / 2.0f;
                            balasJugadores[i].y = (float)(CANON_Y - CANON_ALTO_CANON);
                            balasJugadores[i].activa = true;
                        }
                        break;
                    }
                }
            }
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

    // ── Reemplazar el array de aliens con el reconstruido ────────────
    // Esto SIEMPRE refleja exactamente lo que mando el servidor, en su
    // mismo orden, sin clones ni aliens fantasma de rondas anteriores.
    for (int i = 0; i < numAliensTmp; i++) {
        aliens[i] = aliensTmp[i];
    }
    numAliens = numAliensTmp;

    // Si el servidor ya no reporta UFO, ocultarlo
    if (!nuevoUfo) {
        ufoPresente = false;
        ufo.activo  = false;
    }

    // Si hubo ronda nueva: reiniciar direccion/animacion de la formacion
    // y limpiar balas en pantalla, pero conservar jugadores y bunkers
    // (los bunkers se actualizan por su propio bloque BUNKER arriba)
    if (rondaNueva) {
        alienDirX         = 1.0f;
        animFrame         = 0;
        animTimer         = 0;
        alienesReportados = false;
        balasJugadores[0].activa = false;
        balasJugadores[1].activa = false;
        for (int i = 0; i < MAX_BALAS_ENEMIGAS; i++)
            balasEnemigas[i].activa = false;
        printf("[NET] Nueva ronda detectada (SPEED=%d). Formacion reiniciada.\n", alienSpeedMs);
    }

    if (!gameOver) alienesReportados = false;
    alienVelPx = velocidadDesdeMs(alienSpeedMs);
}

// procesarMensaje
static void procesarMensaje(const char *msg)
{
    // SLOTS <p1Free> <p2Free> — el servidor informa que slots estan libres
    if (strncmp(msg, "SLOTS ", 6) == 0) {
        char p1[8], p2[8];
        sscanf(msg + 6, "%7s %7s", p1, p2);
        p1Libre   = (strcmp(p1, "true") == 0);
        p2Libre   = (strcmp(p2, "true") == 0);
        p1Ocupado = !p1Libre;
        p2Ocupado = !p2Libre;
        if (menuEstado == MENU_ESPERANDO_SLOTS || menuEstado == MENU_ESPERANDO_RESPUESTA) {
            menuEstado = MENU_SELECCIONANDO;
        }
        printf("[NET] Slots -> P1:%d P2:%d\n", p1Libre, p2Libre);
    }
    // JOIN_DENIED <slot> — el slot pedido ya estaba ocupado
    else if (strncmp(msg, "JOIN_DENIED", 11) == 0) {
        printf("[NET] %s\n", msg);
        menuEstado = MENU_SELECCIONANDO;
    }
    else if (strncmp(msg, "PLAYER_ID ", 10) == 0) {
        miPlayerId  = atoi(msg + 10);
        esperandoId = false;
        menuEstado  = MENU_LISTO;
        printf("[NET] Soy jugador %d\n", miPlayerId);
    }
    else if (strcmp(msg, "SPECTATOR") == 0) {
        miPlayerId  = -1;
        esperandoId = false;
        menuEstado  = MENU_LISTO;
        printf("[NET] Soy espectador\n");
    }
    else if (strncmp(msg, "STATE:", 6) == 0) {
        // Solo procesar el estado si ya tenemos un rol asignado
        if (menuEstado == MENU_LISTO) {
            parsearEstado(msg);
        }
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
