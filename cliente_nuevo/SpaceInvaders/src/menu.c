// ─────────────────────────────────────────────────────────────────
//  menu.c — Pantalla de seleccion de rol
// ─────────────────────────────────────────────────────────────────
#include <string.h>
#include <stdio.h>
#include "../raylib/include/raylib.h"
#include "../include/menu.h"
#include "../include/game_state.h"
#include "../include/network.h"
#include "../include/constants.h"
#include "../include/renderer.h"



EstadoMenu menuEstado   = MENU_ESPERANDO_SLOTS;
bool       p1Libre      = false;
bool       p2Libre      = false;
bool       p1Ocupado    = false;
bool       p2Ocupado    = false;

// ── actualizarMenu ────────────────────────────────────────────────
// Lee las teclas G (jugador 1), H (jugador 2) y E (espectador).
// Solo se permite elegir un slot si esta libre.
void actualizarMenu(void)
{
    if (menuEstado != MENU_SELECCIONANDO) return;

    if (IsKeyPressed(KEY_G) && p1Libre) {
        sendMessage("JOIN PLAYER1\n");
        menuEstado = MENU_ESPERANDO_RESPUESTA;
    }
    else if (IsKeyPressed(KEY_H) && p2Libre) {
        sendMessage("JOIN PLAYER2\n");
        menuEstado = MENU_ESPERANDO_RESPUESTA;
    }
    else if (IsKeyPressed(KEY_E)) {
        sendMessage("JOIN SPECTATOR\n");
        menuEstado = MENU_ESPERANDO_RESPUESTA;
    }
}

// ── dibujarMenu ─────────────────────────────────────────────────────
// Logo "spaCEinvaders" + 3 opciones con su tecla y estado
void dibujarMenu(int frames)
{
    int af = (frames / WAIT_BLINK_RATE) % 2;

    // ── Logo / titulo ───────────────────────────────────────────────
    const char *title = "spaCEinvaders";
    DrawText(title, SCREEN_WIDTH/2 - MeasureText(title, 56)/2, 70, 56, COLOR_UI);

    // Formacion decorativa de aliens bajo el titulo
    for (int i = 0; i < 7; i++) {
        int ax = SCREEN_WIDTH/2 - 7*(ALIEN_ANCHO+10)/2 + i*(ALIEN_ANCHO+10);
        dibujarAlien(ax, 150, (TipoAlien)(i % 3), af, Fade(COLOR_UI, 0.5f));
    }

    // ── Estado de conexion / espera de slots ─────────────────────────
    if (menuEstado == MENU_ESPERANDO_SLOTS)
    {
        const char *msg = "Conectando con el servidor...";
        DrawText(msg, SCREEN_WIDTH/2 - MeasureText(msg, 22)/2,
                 SCREEN_HEIGHT/2 - 10, 22, COLOR_WAIT_TEXT);
        return;
    }

    if (menuEstado == MENU_ESPERANDO_RESPUESTA)
    {
        const char *msg = "Uniendose...";
        DrawText(msg, SCREEN_WIDTH/2 - MeasureText(msg, 22)/2,
                 SCREEN_HEIGHT/2 - 10, 22, COLOR_WAIT_TEXT);
        return;
    }

    // ── Opciones (MENU_SELECCIONANDO) ─────────────────────────────────
    int boxW = 380;
    int boxH = 70;
    int boxX = SCREEN_WIDTH/2 - boxW/2;
    int spacing = 24;
    int startY  = 260;

    // Caja 1: Jugador 1 (tecla G)
    {
        int y = startY;
        Color borde = p1Ocupado ? GRAY : COLOR_UI;
        DrawRectangleLines(boxX, y, boxW, boxH, borde);

        char buf[64];
        if (p1Ocupado) {
            DrawText("OCUPADO", boxX + 20, y + 24, 24, GRAY);
            DrawText("Jugador 1", boxX + boxW - MeasureText("Jugador 1", 24) - 20,
                     y + 24, 24, GRAY);
        } else {
            snprintf(buf, sizeof(buf), "[G]  Unirse como Jugador 1");
            DrawText(buf, boxX + 20, y + 24, 24, COLOR_UI);
        }
    }

    // Caja 2: Jugador 2 (tecla H)
    {
        int y = startY + (boxH + spacing);
        Color borde = p2Ocupado ? GRAY : COLOR_UI;
        DrawRectangleLines(boxX, y, boxW, boxH, borde);

        char buf[64];
        if (p2Ocupado) {
            DrawText("OCUPADO", boxX + 20, y + 24, 24, GRAY);
            DrawText("Jugador 2", boxX + boxW - MeasureText("Jugador 2", 24) - 20,
                     y + 24, 24, GRAY);
        } else {
            snprintf(buf, sizeof(buf), "[H]  Unirse como Jugador 2");
            DrawText(buf, boxX + 20, y + 24, 24, COLOR_UI);
        }
    }

    // Caja 3: Espectador (tecla E) — siempre disponible
    {
        int y = startY + 2 * (boxH + spacing);
        DrawRectangleLines(boxX, y, boxW, boxH, COLOR_UI);
        DrawText("[E]  Unirse como Espectador", boxX + 20, y + 24, 24, COLOR_UI);
    }

    // Pie de pantalla
    const char *hint = "Presiona la tecla correspondiente para unirte";
    DrawText(hint, SCREEN_WIDTH/2 - MeasureText(hint, 16)/2,
             SCREEN_HEIGHT - 40, 16, Fade(WHITE, 0.4f));
}