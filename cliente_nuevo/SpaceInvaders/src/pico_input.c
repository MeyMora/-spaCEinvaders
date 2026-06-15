// ─────────────────────────────────────────────────────────────────
//  pico_input.c — Interfaz serial con Raspberry Pi Pico
//
//  Función:
//    Lee comandos enviados por una Raspberry Pi Pico mediante
//    comunicación serial USB (COMx).
//
//  Flujo:
//    1) Abrir el puerto serial configurado.
//    2) Configurar velocidad y parámetros de comunicación.
//    3) Leer caracteres recibidos de forma no bloqueante.
//    4) Reconstruir líneas terminadas en '\n'.
//    5) Interpretar comandos:
//         LEFT
//         RIGHT
//         SHOOT
//    6) Actualizar picoUltimoComando para que main.c procese
//       la acción correspondiente.
//
//  Integración:
//    Raspberry Pi Pico
//          │
//          ▼
//    puerto COM (USB Serial)
//          │
//          ▼
//      pico_input.c
//          │
//          ▼
//        main.c
//          │
//          ▼
//     lógica del juego
// ─────────────────────────────────────────────────────────────────

#include <windows.h>
#include <stdio.h>
#include <string.h>

#include "../include/pico_input.h"
#include "../include/network.h"
#include "../include/constants.h"

static HANDLE hSerial = INVALID_HANDLE_VALUE;

int picoUltimoComando = 0;
// ─────────────────────────────────────────────────────────────────
//  picoConectar
//
//  Abre el puerto COM8 y configura la comunicación serial
//  a 115200 baudios, 8 bits de datos, sin paridad y
//  1 bit de parada.
//
//  Retorna:
//      1 -> conexión exitosa
//      0 -> error al abrir/configurar el puerto
// ─────────────────────────────────────────────────────────────────

int picoConectar(void)
{
    printf("ENTRE A picoConectar\n");

    // Abrir puerto serial de la Pico.
    hSerial = CreateFileA(
        "\\\\.\\COM8",
        GENERIC_READ,
        0,
        NULL,
        OPEN_EXISTING,
        0,
        NULL
    );

    // Verificar si el puerto pudo abrirse.
    if (hSerial == INVALID_HANDLE_VALUE)
    {
        printf("No se pudo abrir COM8\n");
        return 0;
    }

    // Estructura de configuración del puerto serial.
    DCB dcb = {0};
    dcb.DCBlength = sizeof(dcb);

    if (!GetCommState(hSerial, &dcb))
    {
        printf("GetCommState fallo\n");
        return 0;
    }

    // Configuración de la comunicación serial.
    dcb.BaudRate = CBR_115200;
    dcb.ByteSize = 8;
    dcb.Parity   = NOPARITY;
    dcb.StopBits = ONESTOPBIT;

    if (!SetCommState(hSerial, &dcb))
    {
        printf("SetCommState fallo\n");
        return 0;
    }

    // Configuración de timeouts para lectura no bloqueante.
    COMMTIMEOUTS timeouts = {0};

    timeouts.ReadIntervalTimeout        = 1;
    timeouts.ReadTotalTimeoutConstant   = 1;
    timeouts.ReadTotalTimeoutMultiplier = 0;

    SetCommTimeouts(hSerial, &timeouts);

    // Obtener estado actual del buffer serial.
    DWORD errores;
    COMSTAT estado;

    ClearCommError(hSerial, &errores, &estado);

    printf("Pico conectada en COM8\n");
    printf("Bytes pendientes al abrir = %lu\n", estado.cbInQue);

    // Valor de prueba para confirmar conexión.
    picoUltimoComando = 99;

    return 1;
}
// ─────────────────────────────────────────────────────────────────
//  picoLeer
//
//  Lee caracteres enviados por la Raspberry Pi Pico.
//  Los caracteres se almacenan hasta encontrar '\n'.
//
//  Comandos válidos:
//      LEFT
//      RIGHT
//      SHOOT
//
//  Al recibir un comando:
//      - Actualiza picoUltimoComando.
//      - Envía la acción correspondiente al servidor.
// ─────────────────────────────────────────────────────────────────
void picoLeer(void)
{
     // Si el puerto no está abierto no se realiza ninguna lectura.
    if (hSerial == INVALID_HANDLE_VALUE)
        return;

    char c;
    DWORD leidos;

    // Buffer donde se almacena temporalmente cada línea recibida.
    static char buffer[64];

    // Posición actual dentro del buffer.
    static int pos = 0;

    // Leer todos los caracteres disponibles.
    while (ReadFile(hSerial, &c, 1, &leidos, NULL) && leidos > 0)
    {
        if (c == '\r')
            continue;

        if (c == '\n')
        {
             // Convertir a string válido.
            buffer[pos] = '\0';

            // Comando: LEFT
            if (strcmp(buffer, "LEFT") == 0)
            {
                picoUltimoComando = 1;
                sendMessage(MSG_MOVE_LEFT);
            }

            // Comando: RIGHT
            else if (strcmp(buffer, "RIGHT") == 0)
            {
                picoUltimoComando = 2;
                sendMessage(MSG_MOVE_RIGHT);
            }

            // Comando: SHOOT
            else if (strcmp(buffer, "SHOOT") == 0)
            {
                picoUltimoComando = 3;
                sendMessage(MSG_SHOOT);
            }

            pos = 0;
        }
        else
        {
            if (pos < sizeof(buffer) - 1)
                buffer[pos++] = c;
        }
    }
}