#ifndef NETWORK_H
#define NETWORK_H

// ─────────────────────────────────────────────────────────────────
//  Módulo de red — conexión TCP con el servidor Java
// ─────────────────────────────────────────────────────────────────

// Intenta conectar al servidor. Devuelve 1 si tuvo éxito, 0 si falló.
int  connectToServer(void);

// Envía un mensaje de texto al servidor (incluye '\n' al final si no lo tiene).
void sendMessage(const char *message);

// Intenta leer datos disponibles del socket de forma no bloqueante.
// Escribe en 'buf' (máx. buf_size bytes). Devuelve bytes leídos (>=0) o -1 en error.
int  receiveNonBlocking(char *buf, int buf_size);

// Cierra la conexión.
void closeConnection(void);

#endif // NETWORK_H