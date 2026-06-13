#ifndef CONSTANTS_H
#define CONSTANTS_H

// ─────────────────────────────────────────────────────────────────
//  constants.h — Todas las constantes del juego spaCEinvaders
// ─────────────────────────────────────────────────────────────────

// ── Pantalla ────────────────────────────────────────────────────
#define SCREEN_WIDTH      940
#define SCREEN_HEIGHT     680
#define WINDOW_TITLE      "spaCEinvaders"
#define TARGET_FPS        60

// ── Red ─────────────────────────────────────────────────────────
#define SERVER_IP         "127.0.0.1"
#define SERVER_PORT       5000
#define RECV_BUFFER_SIZE  4096
#define SEND_BUFFER_SIZE  256

// ── Área de juego (dentro de la ventana) ────────────────────────
#define GAME_AREA_X       30
#define GAME_AREA_W       880
#define GAME_AREA_Y       60
#define GAME_AREA_H       560

// ── Cañón ────────────────────────────────────────────────────────
#define CANON_ANCHO       38
#define CANON_ALTO_BASE   14
#define CANON_ALTO_CANON  18
#define CANON_Y           (SCREEN_HEIGHT - 80)
#define CANON_VELOCIDAD   4.0f
#define CANON_INICIO_X    (SCREEN_WIDTH / 2 - CANON_ANCHO / 2)

// ── Bala del jugador ────────────────────────────────────────────
#define BALA_ANCHO        4
#define BALA_ALTO         14
#define BALA_VELOCIDAD    9.0f

// ── Balas enemigas ───────────────────────────────────────────────
#define BALA_ENEMIGA_VEL  4.0f
// Frames entre cada disparo de un alien (120 frames = 2 segundos a 60fps)
#define ALIEN_SHOOT_INTERVAL  90

// ── Aliens ──────────────────────────────────────────────────────
#define ALIEN_ANCHO       32
#define ALIEN_ALTO        24
#define MAX_ALIENS        128
#define ALIEN_SPEED_BASE  1.2f
#define ALIEN_SPEED_SCALE 0.012f
#define ALIEN_STEP_DOWN   20
#define ALIEN_ANIM_TICKS  30

// ── Formación inicial de aliens ──────────────────────────────────
// 11 columnas × 5 filas = 55 aliens como en el juego original
#define ALIEN_FILAS       5
#define ALIEN_COLS        11
#define ALIEN_SEP_X       52    // separación horizontal en píxeles
#define ALIEN_SEP_Y       44    // separación vertical en píxeles
#define ALIEN_INICIO_X    (GAME_AREA_X + 40)
#define ALIEN_INICIO_Y    (GAME_AREA_Y + 40)

// ── Bunkers ─────────────────────────────────────────────────────
#define BUNKER_ANCHO      58
#define BUNKER_ALTO       44
#define MAX_BUNKERS       4
#define BUNKER_Y          (SCREEN_HEIGHT - 130)

// ── OVNI ────────────────────────────────────────────────────────
#define UFO_ANCHO         52
#define UFO_ALTO          22
#define UFO_Y             (GAME_AREA_Y + 8)
#define UFO_VELOCIDAD     2.0f

// ── Línea inferior ──────────────────────────────────────────────
#define LINEA_INF_Y       (SCREEN_HEIGHT - 50)

// ── Colores ─────────────────────────────────────────────────────
#define COLOR_FONDO         CLITERAL(Color){ 0,   0,   0,   255 }
#define COLOR_CANON         CLITERAL(Color){ 0,   255, 0,   255 }
#define COLOR_BALA          CLITERAL(Color){ 255, 255, 255, 255 }
#define COLOR_BALA_ENEMIGA  CLITERAL(Color){ 255, 80,  80,  255 }
#define COLOR_UI            CLITERAL(Color){ 0,   255, 0,   255 }
#define COLOR_LINEA         CLITERAL(Color){ 0,   255, 0,   255 }
#define COLOR_UFO           CLITERAL(Color){ 255, 50,  50,  255 }
#define COLOR_ALIEN_1       CLITERAL(Color){ 255, 255, 255, 255 }
#define COLOR_ALIEN_2       CLITERAL(Color){ 255, 255, 255, 255 }
#define COLOR_ALIEN_3       CLITERAL(Color){ 255, 255, 255, 255 }
#define COLOR_BUNKER        CLITERAL(Color){ 0,   255, 0,   255 }
#define COLOR_BUNKER_MED    CLITERAL(Color){ 200, 200, 0,   255 }
#define COLOR_BUNKER_LOW    CLITERAL(Color){ 255, 80,  0,   255 }
#define COLOR_BUNKER_CRIT   CLITERAL(Color){ 180, 0,   0,   255 }
#define COLOR_WAIT_TEXT     CLITERAL(Color){ 0,   220, 0,   255 }

// ── Mensajes hacia el servidor ───────────────────────────────────
#define MSG_MOVE_LEFT     "MOVE LEFT\n"
#define MSG_MOVE_RIGHT    "MOVE RIGHT\n"
#define MSG_SHOOT         "SHOOT\n"
#define MSG_PLAYER_HIT    "PLAYER_HIT\n"
#define MSG_ALIENS_BASE   "ALIENS_REACHED\n"

// ── Pantalla de espera ───────────────────────────────────────────
#define WAIT_BLINK_RATE   45

#endif // CONSTANTS_H
