// ─────────────────────────────────────────────────────────────────
//  pico_input.h — Lectura de comandos desde Raspberry Pi Pico
//
//  Responsabilidad:
//    - Declarar las funciones de comunicación con la Pico.
//    - Exponer el último comando recibido.
//    - Permitir que el game loop consulte acciones físicas
//      generadas por los botones conectados a la Pico.
//
//  Comandos soportados:
//    LEFT   -> mover jugador a la izquierda
//    RIGHT  -> mover jugador a la derecha
//    SHOOT  -> disparar
// ─────────────────────────────────────────────────────────────────



#ifndef PICO_INPUT_H
#define PICO_INPUT_H
// Último comando recibido desde la Pico.
extern int picoUltimoComando;

// Inicializa la comunicación serial con la Pico.
int picoConectar(void);

// Lee y procesa datos recibidos desde la Pico.
void picoLeer(void);

#endif