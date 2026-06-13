// ═══════════════════════════════════════════════════════════════════════════
//  main.c — spaCEinvaders · Cliente jugador en C (paradigma imperativo)
//  CE3104 · Lenguajes, Compiladores e Intérpretes · TEC
//
//  Módulos:
//    main.c        — game loop principal e input
//    game_state.c  — structs, variables globales y funciones base
//    net_handler.c — lectura y parseo de mensajes del servidor Java
//    gameplay.c    — movimiento de aliens, disparos y colisiones
//    renderer.c    — dibujo de todos los elementos con Raylib
//    network.c     — comunicación TCP con el servidor (WinSock2)
// ═══════════════════════════════════════════════════════════════════════════

#include "../raylib/include/raylib.h"
#include "../include/constants.h"
#include "../include/game_state.h"
#include "../include/net_handler.h"
#include "../include/gameplay.h"
#include "../include/renderer.h"
#include "../include/network.h"

#include <stdlib.h>
#include <time.h>

int main(void)
{
    srand((unsigned int)time(NULL));  // semilla para disparos aleatorios

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, WINDOW_TITLE);
    SetTargetFPS(TARGET_FPS);

    estadoInit();
    conectado = (connectToServer() == 1);

    int waitFrames = 0;

    // ── Game loop ────────────────────────────────────────────────
    while (!WindowShouldClose())
    {
        waitFrames++;

        // ── LEER RED ─────────────────────────────────────────────
        if (conectado) leerRed();

        // ── INPUT ─────────────────────────────────────────────────
        if (conectado && !esperandoId && !gameOver)
        {
            int idx = miIdx();
            if (miPlayerId > 0 && idx >= 0)
            {
                if ((IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A))
                    && jugadores[idx].x > GAME_AREA_X)
                {
                    jugadores[idx].x -= CANON_VELOCIDAD;
                    sendMessage(MSG_MOVE_LEFT);
                }
                if ((IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D))
                    && jugadores[idx].x + CANON_ANCHO < GAME_AREA_X + GAME_AREA_W)
                {
                    jugadores[idx].x += CANON_VELOCIDAD;
                    sendMessage(MSG_MOVE_RIGHT);
                }

                if (IsKeyPressed(KEY_SPACE) && !bala.activa)
                {
                    bala.x      = jugadores[idx].x + CANON_ANCHO / 2.0f - BALA_ANCHO / 2.0f;
                    bala.y      = (float)(CANON_Y - CANON_ALTO_CANON);
                    bala.activa = true;
                    sendMessage(MSG_SHOOT);
                }
            }
        }

        // ── UPDATE ────────────────────────────────────────────────
        if (conectado && !esperandoId && !gameOver)
        {
            actualizarAliensLocal();
            actualizarBalaLocal();
            actualizarDisparoAliens();
            actualizarBalasEnemigas();
            detectarColisiones();
        }

        // ── DRAW ──────────────────────────────────────────────────
        BeginDrawing();
        ClearBackground(COLOR_FONDO);

        if (!conectado)         dibujarSinConexion();
        else if (esperandoId)   dibujarPantallaEspera(waitFrames);
        else if (gameOver)      dibujarGameOver();
        else                    dibujarEscena();

        EndDrawing();
    }

    closeConnection();
    CloseWindow();
    return 0;
}
