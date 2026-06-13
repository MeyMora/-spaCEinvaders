// ─────────────────────────────────────────────────────────────────
//  network.c  —  Comunicación TCP con el servidor Java (WinSock2)
// ─────────────────────────────────────────────────────────────────
#include "../include/network.h"
#include "../include/constants.h"

#include <stdio.h>
#include <string.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

// Socket global del cliente
static SOCKET clientSocket = INVALID_SOCKET;

// ─── connectToServer ────────────────────────────────────────────
// Inicializa WinSock, crea el socket y conecta al servidor.
// Devuelve 1 si la conexión fue exitosa, 0 en caso contrario.
int connectToServer(void)
{
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("[RED] Error al inicializar WinSock\n");
        return 0;
    }

    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        printf("[RED] No se pudo crear el socket\n");
        WSACleanup();
        return 0;
    }

    struct sockaddr_in server;
    server.sin_family      = AF_INET;
    server.sin_port        = htons(SERVER_PORT);
    server.sin_addr.s_addr = inet_addr(SERVER_IP);

    if (connect(clientSocket, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR) {
        printf("[RED] No se pudo conectar al servidor %s:%d\n", SERVER_IP, SERVER_PORT);
        closesocket(clientSocket);
        clientSocket = INVALID_SOCKET;
        WSACleanup();
        return 0;
    }

    // Poner el socket en modo NO bloqueante para el game loop
    u_long modo = 1;
    ioctlsocket(clientSocket, FIONBIO, &modo);

    printf("[RED] Conectado al servidor %s:%d\n", SERVER_IP, SERVER_PORT);
    return 1;
}

// ─── sendMessage ────────────────────────────────────────────────
// Envía el texto al servidor. Si el mensaje no termina en '\n'
// la función añade uno automáticamente.
void sendMessage(const char *message)
{
    if (clientSocket == INVALID_SOCKET || message == NULL) return;

    char buf[SEND_BUFFER_SIZE];
    int  len = (int)strlen(message);

    // Copiar con límite de buffer
    if (len >= SEND_BUFFER_SIZE - 2) len = SEND_BUFFER_SIZE - 2;
    memcpy(buf, message, len);

    // Asegurar salto de línea al final
    if (buf[len - 1] != '\n') {
        buf[len]     = '\n';
        buf[len + 1] = '\0';
        len++;
    } else {
        buf[len] = '\0';
    }

    send(clientSocket, buf, len, 0);
}

// ─── receiveNonBlocking ─────────────────────────────────────────
// Lee datos disponibles del socket sin bloquear.
// Devuelve la cantidad de bytes copiados en 'buf',
// 0 si no había nada, o -1 si la conexión fue cerrada/hubo error.
int receiveNonBlocking(char *buf, int buf_size)
{
    if (clientSocket == INVALID_SOCKET || buf == NULL || buf_size <= 0) return -1;

    int bytes = recv(clientSocket, buf, buf_size - 1, 0);

    if (bytes > 0) {
        buf[bytes] = '\0';
        return bytes;
    }

    if (bytes == 0) {
        // El servidor cerró la conexión
        return -1;
    }

    // bytes < 0
    int err = WSAGetLastError();
    if (err == WSAEWOULDBLOCK) {
        // Sin datos disponibles, es normal en modo no bloqueante
        buf[0] = '\0';
        return 0;
    }

    // Error real de red
    printf("[RED] Error al recibir datos: %d\n", err);
    return -1;
}

// ─── closeConnection ────────────────────────────────────────────
void closeConnection(void)
{
    if (clientSocket != INVALID_SOCKET) {
        closesocket(clientSocket);
        clientSocket = INVALID_SOCKET;
    }
    WSACleanup();
}