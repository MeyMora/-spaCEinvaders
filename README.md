# spaCEinvaders

Proyecto desarrollado para el curso **Paradigmas de Programación ,  (CE3104)** del Instituto Tecnológico de Costa Rica.

El proyecto consiste en una implementación del videojuego **Space Invaders**, utilizando dos paradigmas de programación:

- **Programación orientada a objetos** en Java para el servidor.
- **Programación imperativa** en C para el cliente jugador y el control físico.

El sistema utiliza comunicación cliente-servidor mediante **sockets**. El servidor mantiene la lógica completa del juego, mientras que los clientes reciben el estado de la partida, muestran la interfaz gráfica y envían las acciones realizadas por el jugador.

---

## Integrantes

| Nombre | Rol principal |
|---|---|
| Integrante 1 | Servidor Java y lógica del juego |
| Integrante 2 | Cliente jugador en C e interfaz gráfica |
| Integrante 3 | Control físico con Raspberry Pi Pico, cliente espectador e integración |

---

## Objetivo general

Desarrollar una aplicación que permita reafirmar el conocimiento de los paradigmas de programación imperativo y orientado a objetos mediante la implementación de un videojuego tipo Space Invaders.

---

## Objetivos específicos

- Implementar el servidor del juego en Java.
- Implementar el cliente jugador en C.
- Aplicar el paradigma orientado a objetos en el servidor.
- Aplicar el paradigma imperativo en el cliente.
- Utilizar sockets para la comunicación entre cliente y servidor.
- Crear y manipular estructuras de datos como listas.
- Implementar un control físico mediante Raspberry Pi Pico.
- Permitir la conexión de al menos dos clientes jugadores.
- Permitir la conexión de clientes espectadores.

---

## Descripción general del juego

El jugador controla un cañón láser que puede moverse a la izquierda o a la derecha y disparar contra extraterrestres invasores. Los enemigos avanzan progresivamente y el jugador debe eliminarlos antes de que lleguen hasta su posición.

Existen diferentes tipos de enemigos, cada uno con una puntuación distinta:

| Tipo de enemigo | Puntos |
|---|---:|
| Calamar | 10 |
| Cangrejo | 20 |
| Pulpo | 40 |
| OVNI | Puntuación aleatoria |

El jugador inicia con **3 vidas**. Cada vez que una bala enemiga impacta al jugador, pierde una vida. Si las vidas llegan a cero, el juego termina.

Cuando el jugador elimina todos los extraterrestres, se le asigna una vida adicional y el juego reinicia con una velocidad mayor.

---

## Arquitectura del sistema

El proyecto se divide en tres componentes principales:

```text
+----------------------+
|      Servidor Java   |
|  Lógica del juego    |
+----------+-----------+
           |
           | Sockets
           |
+----------+-----------+        +----------------------+
| Cliente jugador C    |        | Cliente espectador   |
| Interfaz gráfica     |        | Solo visualización   |
+----------+-----------+        +----------------------+
           |
           | UART / I2C
           |
+----------+-----------+
| Raspberry Pi Pico    |
| Control físico       |
+----------------------+
