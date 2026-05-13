#ifndef CONSTANTS_H
#define CONSTANTS_H

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 5000

// -- Ventana --
#define SCREEN_WIDTH    800
#define SCREEN_HEIGHT   600
#define TARGET_FPS      60
#define WINDOW_TITLE    "Mi primer juego con Raylib"

// -- Colores personalizados (R, G, B, A) --
#define COLOR_FONDO     CLITERAL(Color){ 15,  15,  30, 255 }
#define COLOR_CANON     CLITERAL(Color){ 80, 200, 120, 255 }
#define COLOR_BALA      CLITERAL(Color){ 255, 220,  50, 255 }
#define COLOR_ALIEN     CLITERAL(Color){ 200,  80,  80, 255 }
#define COLOR_UI        CLITERAL(Color){ 200, 200, 200, 255 }

// -- Cañon --
#define CANON_ANCHO     60
#define CANON_ALTO      30
#define CANON_VELOCIDAD 5.0f
#define CANON_Y         (SCREEN_HEIGHT - 60)

// -- Bala --
#define BALA_ANCHO      6
#define BALA_ALTO       18
#define BALA_VELOCIDAD  8.0f

// -- Aliens --
#define ALIEN_FILAS     3
#define ALIEN_COLS      8
#define ALIEN_ANCHO     40
#define ALIEN_ALTO      30
#define ALIEN_SEP_X     60
#define ALIEN_SEP_Y     55
#define ALIEN_INICIO_X  80
#define ALIEN_INICIO_Y  60
#define ALIEN_VELOCIDAD 1.5f
#define MAX_ALIENS      (ALIEN_FILAS * ALIEN_COLS)

#endif // CONSTANTS_H