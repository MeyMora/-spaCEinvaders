#include "raylib.h"
#include "constants.h"
#include <stdbool.h>
#include <stdio.h>
#include "../include/network.h"




// ─────────────────────────────────────────────
//  STRUCTS
// ─────────────────────────────────────────────

typedef struct {
    float x, y;
    bool  activa;
} Bala;

typedef struct {
    float x, y;
    bool  vivo;
} Alien;

typedef struct {
    float x, y;
    int   vidas;
    int   puntaje;
} Canon;

// ─────────────────────────────────────────────
//  INICIALIZAR ALIENS
// ─────────────────────────────────────────────

void iniciar_aliens(Alien aliens[]) {
    int idx = 0;
    for (int fila = 0; fila < ALIEN_FILAS; fila++) {
        for (int col = 0; col < ALIEN_COLS; col++) {
            aliens[idx].x    = ALIEN_INICIO_X + col * ALIEN_SEP_X;
            aliens[idx].y    = ALIEN_INICIO_Y + fila * ALIEN_SEP_Y;
            aliens[idx].vivo = true;
            idx++;
        }
    }
}

// ─────────────────────────────────────────────
//  MAIN
// ─────────────────────────────────────────────

int main(void) {
    connectToServer();

    sendMessage("MOVE LEFT\n");

    receiveMessage();
    // ── Inicializar ventana ──────────────────
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, WINDOW_TITLE);
    SetTargetFPS(TARGET_FPS);

    // ── Estado inicial ───────────────────────
    Canon canon = {
        .x       = SCREEN_WIDTH / 2.0f - CANON_ANCHO / 2.0f,
        .y       = CANON_Y,
        .vidas   = 3,
        .puntaje = 0
    };

    Bala bala = { .activa = false };

    Alien aliens[MAX_ALIENS];
    iniciar_aliens(aliens);

    float dir_alien    = 1.0f;   // 1 = derecha, -1 = izquierda
    bool  juego_activo = true;

    // ─────────────────────────────────────────
    //  GAME LOOP
    // ─────────────────────────────────────────
    while (!WindowShouldClose()) {

        // ── INPUT ────────────────────────────
        if (juego_activo) {

            // Mover cañón
            if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) {
                if (canon.x > 0)
                    canon.x -= CANON_VELOCIDAD;
            }
            if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) {
                if (canon.x + CANON_ANCHO < SCREEN_WIDTH)
                    canon.x += CANON_VELOCIDAD;
            }

            // Disparar (solo una bala a la vez)
            if (IsKeyPressed(KEY_SPACE) && !bala.activa) {
                bala.x      = canon.x + CANON_ANCHO / 2.0f - BALA_ANCHO / 2.0f;
                bala.y      = canon.y;
                bala.activa = true;
            }
        }

        // Reiniciar con R
        if (IsKeyPressed(KEY_R)) {
            canon.x       = SCREEN_WIDTH / 2.0f - CANON_ANCHO / 2.0f;
            canon.puntaje = 0;
            canon.vidas   = 3;
            bala.activa   = false;
            dir_alien     = 1.0f;
            juego_activo  = true;
            iniciar_aliens(aliens);
        }

        // ── UPDATE ───────────────────────────
        if (juego_activo) {

            // Mover bala
            if (bala.activa) {
                bala.y -= BALA_VELOCIDAD;
                if (bala.y + BALA_ALTO < 0)
                    bala.activa = false;
            }

            // Mover aliens (rebotan en los bordes)
            bool rebotar = false;
            for (int i = 0; i < MAX_ALIENS; i++) {
                if (!aliens[i].vivo) continue;
                aliens[i].x += ALIEN_VELOCIDAD * dir_alien;
                if (aliens[i].x + ALIEN_ANCHO >= SCREEN_WIDTH || aliens[i].x <= 0)
                    rebotar = true;
            }
            if (rebotar) {
                dir_alien *= -1.0f;
                for (int i = 0; i < MAX_ALIENS; i++) {
                    if (aliens[i].vivo) aliens[i].y += 15;
                }
            }

            // Colisión bala - alien
            if (bala.activa) {
                Rectangle r_bala = { bala.x, bala.y, BALA_ANCHO, BALA_ALTO };
                for (int i = 0; i < MAX_ALIENS; i++) {
                    if (!aliens[i].vivo) continue;
                    Rectangle r_alien = { aliens[i].x, aliens[i].y,
                                          ALIEN_ANCHO,  ALIEN_ALTO };
                    if (CheckCollisionRecs(r_bala, r_alien)) {
                        aliens[i].vivo = false;
                        bala.activa    = false;
                        canon.puntaje += 10;
                        // 👉 Aquí después enviarás el mensaje al servidor Java
                        break;
                    }
                }
            }

            // ¿Aliens llegaron abajo? → perder vida
            for (int i = 0; i < MAX_ALIENS; i++) {
                if (aliens[i].vivo && aliens[i].y + ALIEN_ALTO >= CANON_Y) {
                    canon.vidas--;
                    iniciar_aliens(aliens);
                    dir_alien = 1.0f;
                    if (canon.vidas <= 0) juego_activo = false;
                    break;
                }
            }

            // ¿Todos los aliens muertos? → ganaste
            bool todos_muertos = true;
            for (int i = 0; i < MAX_ALIENS; i++) {
                if (aliens[i].vivo) { todos_muertos = false; break; }
            }
            if (todos_muertos) juego_activo = false;
        }

        // ── DRAW ─────────────────────────────
        BeginDrawing();
            ClearBackground(COLOR_FONDO);

            if (juego_activo) {

                // Cañón (triángulo + base)
                DrawRectangle((int)canon.x, (int)canon.y,
                              CANON_ANCHO, CANON_ALTO, COLOR_CANON);
                DrawRectangle((int)canon.x + CANON_ANCHO/2 - 5,
                              (int)canon.y - 15, 10, 18, COLOR_CANON);

                // Bala
                if (bala.activa) {
                    DrawRectangle((int)bala.x, (int)bala.y,
                                  BALA_ANCHO, BALA_ALTO, COLOR_BALA);
                }

                // Aliens
                for (int i = 0; i < MAX_ALIENS; i++) {
                    if (!aliens[i].vivo) continue;
                    DrawRectangle((int)aliens[i].x, (int)aliens[i].y,
                                  ALIEN_ANCHO, ALIEN_ALTO, COLOR_ALIEN);
                    // Ojitos del alien 👾
                    DrawRectangle((int)aliens[i].x + 8,
                                  (int)aliens[i].y + 8, 8, 8, WHITE);
                    DrawRectangle((int)aliens[i].x + 24,
                                  (int)aliens[i].y + 8, 8, 8, WHITE);
                }

                // UI — puntaje y vidas
                char hud[64];
                sprintf(hud, "Puntaje: %d", canon.puntaje);
                DrawText(hud, 10, 10, 22, COLOR_UI);

                sprintf(hud, "Vidas: %d", canon.vidas);
                DrawText(hud, SCREEN_WIDTH - 120, 10, 22, COLOR_UI);

                // Línea separadora
                DrawLine(0, CANON_Y - 10, SCREEN_WIDTH, CANON_Y - 10,
                         Fade(WHITE, 0.1f));

                // Controles
                DrawText("Mover: A/D o flechas  |  Disparar: ESPACIO  |  Reiniciar: R",
                         10, SCREEN_HEIGHT - 25, 16, Fade(WHITE, 0.4f));

            } else {
                // Pantalla de fin
                bool gano = (canon.vidas > 0);
                const char *msg = gano ? "¡GANASTE!" : "GAME OVER";
                Color color_msg = gano ? GREEN : RED;
                int ancho_msg   = MeasureText(msg, 60);

                DrawText(msg,
                         SCREEN_WIDTH/2 - ancho_msg/2,
                         SCREEN_HEIGHT/2 - 60, 60, color_msg);

                char puntaje_final[64];
                sprintf(puntaje_final, "Puntaje final: %d", canon.puntaje);
                int ancho_p = MeasureText(puntaje_final, 28);
                DrawText(puntaje_final,
                         SCREEN_WIDTH/2 - ancho_p/2,
                         SCREEN_HEIGHT/2 + 20, 28, COLOR_UI);

                DrawText("Presiona R para reiniciar",
                         SCREEN_WIDTH/2 - MeasureText("Presiona R para reiniciar", 20)/2,
                         SCREEN_HEIGHT/2 + 70, 20, Fade(WHITE, 0.5f));
            }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}