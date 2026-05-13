#include "../include/network.h"

#include <stdio.h>
#include <string.h>

#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

SOCKET clientSocket;

void connectToServer()
{
    WSADATA wsa;

    WSAStartup(MAKEWORD(2, 2), &wsa);

    clientSocket = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server;

    server.sin_family = AF_INET;
    server.sin_port = htons(5000);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    int connectionResult = connect(
        clientSocket,
        (struct sockaddr*)&server,
        sizeof(server)
    );

    if (connectionResult == SOCKET_ERROR)
    {
        printf("Error al conectar\n");
        return;
    }

    printf("Conectado al servidor\n");

    printf("Conectado al servidor\n");
}

void sendMessage(const char* message)
{
    send(
        clientSocket,
        message,
        strlen(message),
        0
    );
}

void receiveMessage()
{
    char buffer[1024];

    int bytesReceived = recv(
        clientSocket,
        buffer,
        sizeof(buffer) - 1,
        0
    );

    if (bytesReceived > 0)
    {
        buffer[bytesReceived] = '\0';

        printf("Servidor: %s\n", buffer);
    }
}