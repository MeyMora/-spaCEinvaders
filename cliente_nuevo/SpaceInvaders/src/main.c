// ═══════════════════════════════════════════════════════════════════════════
//  main.c — spaCEinvaders · Cliente jugador en C (paradigma imperativo)
//  CE3104 · Lenguajes, Compiladores e Intérpretes · TEC
//
//  Modulos:
//    main.c        — game loop principal e input
//    game_state.c  — structs, variables globales y funciones base
//    menu.c        — pantalla de seleccion de rol (P1/P2/Espectador)
//    net_handler.c — lectura y parseo de mensajes del servidor Java
//    gameplay.c    — movimiento de aliens, disparos y colisiones
//    renderer.c    — dibujo de todos los elementos con Raylib
//    network.c     — comunicacion TCP con el servidor (WinSock2)
//
//  Flujo:
//    1) Conectar al servidor
//    2) Esperar SLOTS <p1> <p2>  -> mostrar menu
//    3) Usuario presiona G/H/E   -> enviar JOIN PLAYER1/PLAYER2/SPECTATOR
//    4) Servidor responde PLAYER_ID <n> o SPECTATOR (o JOIN_DENIED)
//    5) menuEstado = MENU_LISTO  -> iniciar el juego normalmente
// ═══════════════════════════════════════════════════════════════════════════

#include "../raylib/include/raylib.h"
#include "../include/constants.h"
#include "../include/game_state.h"
#include "../include/menu.h"
#include "../include/net_handler.h"
#include "../include/gameplay.h"
#include "../include/renderer.h"
#include "../include/network.h"
#include "../include/pico_input.h"
extern int picoUltimoComando;

#include <stdlib.h>
#include <time.h>

int main(void)
{
    srand((unsigned int)time(NULL));  // semilla para disparos aleatorios

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, WINDOW_TITLE);
    SetTargetFPS(TARGET_FPS);

    estadoInit();
    conectado = (connectToServer() == 1);
    picoConectar();

    int waitFrames = 0;

    // ── Game loop ────────────────────────────────────────────────
    while (!WindowShouldClose())
    {
        picoLeer();

        waitFrames++;

        // ── LEER RED ─────────────────────────────────────────────
        if (conectado) leerRed();

        // ── MENU DE SELECCION DE ROL ────────────────────────────────
        if (conectado && menuEstado != MENU_LISTO)
        {
            actualizarMenu();
        }

        bool juegoActivo = (conectado && menuEstado == MENU_LISTO && !gameOver);

        // ── INPUT ─────────────────────────────────────────────────
        if (juegoActivo)
        {
            int idx = miIdx();
            if (miPlayerId > 0 && idx >= 0)
            {
                if (picoUltimoComando == 1 &&
    jugadores[idx].x > GAME_AREA_X)
{
    jugadores[idx].x -= 15.0f;
    sendMessage(MSG_MOVE_LEFT);
    picoUltimoComando = 0;
}

if (picoUltimoComando == 2 &&
    jugadores[idx].x + CANON_ANCHO < GAME_AREA_X + GAME_AREA_W)
{
    jugadores[idx].x += 15.0f;
    sendMessage(MSG_MOVE_RIGHT);
    picoUltimoComando = 0;
}

if (picoUltimoComando == 3 &&
    !balasJugadores[idx].activa)
{
    balasJugadores[idx].x =
        jugadores[idx].x + CANON_ANCHO / 2.0f - BALA_ANCHO / 2.0f;

    balasJugadores[idx].y =
        (float)(CANON_Y - CANON_ALTO_CANON);

    balasJugadores[idx].activa = true;

    sendMessage(MSG_SHOOT);

    picoUltimoComando = 0;
}



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

                if (IsKeyPressed(KEY_SPACE) && !balasJugadores[idx].activa)
                {
                    balasJugadores[idx].x      = jugadores[idx].x + CANON_ANCHO / 2.0f - BALA_ANCHO / 2.0f;
                    balasJugadores[idx].y      = (float)(CANON_Y - CANON_ALTO_CANON);
                    balasJugadores[idx].activa = true;
                    sendMessage(MSG_SHOOT);
                }
            }
        }

        // ── UPDATE ────────────────────────────────────────────────
        if (juegoActivo)
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

        if (!conectado)
            dibujarSinConexion();
        else if (menuEstado != MENU_LISTO)
            dibujarMenu(waitFrames);
        else if (gameOver)
            dibujarGameOver();
        else
            dibujarEscena();

        DrawText(TextFormat("PICO=%d", picoUltimoComando),
         10, 10, 20, RED);
            

        EndDrawing();
    }

    closeConnection();
    CloseWindow();
    return 0;
}
