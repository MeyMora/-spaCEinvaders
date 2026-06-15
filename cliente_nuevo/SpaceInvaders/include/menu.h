#ifndef MENU_H
#define MENU_H

#include <stdbool.h>

// ─────────────────────────────────────────────────────────────────
//  menu.h — Pantalla de seleccion de rol (Jugador 1 / Jugador 2 / Espectador)
// ─────────────────────────────────────────────────────────────────

// Estados posibles del menu
typedef enum {
    MENU_ESPERANDO_SLOTS,   // Conectado, esperando que el servidor diga SLOTS
    MENU_SELECCIONANDO,     // Mostrando opciones, esperando que el usuario elija
    MENU_ESPERANDO_RESPUESTA, // JOIN enviado, esperando PLAYER_ID/SPECTATOR/JOIN_DENIED
    MENU_LISTO              // Rol asignado, se puede iniciar el juego
} EstadoMenu;

extern EstadoMenu menuEstado;
extern bool        p1Libre;
extern bool        p2Libre;
extern bool        p1Ocupado;   // true si JOIN_DENIED PLAYER1 (alguien mas se nos adelanto)
extern bool        p2Ocupado;

// Procesa el input de teclado del menu (G/H/E) y envia el JOIN correspondiente
void actualizarMenu(void);

// Dibuja la pantalla de menu con logo y las 3 opciones
void dibujarMenu(int frames);

#endif // MENU_H
