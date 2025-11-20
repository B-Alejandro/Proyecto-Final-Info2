
// ============ nivel2.cpp ============
#include "nivel2.h"
#include "juego.h"
#include "jugador.h"
#include "enemigo.h"
#include "obstaculo.h"
#include "persona.h"
#include <QGraphicsView>

Nivel2::Nivel2(Juego* juego, QObject* parent)
    : NivelBase(juego, 2, parent)
{
    // Guardar dimensiones de la vista
    vistaAncho = juego->getVistaAncho();
    vistaAlto = juego->getVistaAlto();

    // Crear escena 3 veces más grande que la vista
    // (3x en ancho para scroll horizontal, mantener alto)
    int anchoEscena = vistaAncho * 3;
    int altoEscena = vistaAlto;

    crearEscena(anchoEscena, altoEscena);
}

void Nivel2::configurarNivel()
{
    // Crear jugador al inicio de la escena (izquierda)
    // Usar movimiento horizontal para scroll
    crearJugador(vistaAncho / 2, sceneH - 100, TipoMovimiento::CON_GRAVEDAD);

    // Centrar la vista inicialmente en el jugador
    if (escena && !escena->views().isEmpty()) {
        QGraphicsView* vista = escena->views().first();
        vista->centerOn(jugador);
    }
}

void Nivel2::crearEnemigos()
{
    // Crear enemigos distribuidos a lo largo de toda la escena
    // Algunos fuera de la vista inicial para el efecto de scroll

    // Ejemplo: enemigos en diferentes posiciones X
    // (Implementar según tu clase Enemigo)
}

void Nivel2::crearObstaculos()
{
    // Crear obstáculos distribuidos a lo largo de toda la escena
    // Algunos fuera de la vista inicial

    // Ejemplo: obstáculos en diferentes posiciones X
    // (Implementar según tu clase Obstaculo)
}

void Nivel2::actualizar()
{
    // Llamar actualización base
    NivelBase::actualizar();

    // Implementar scroll infinito siguiendo al jugador
    if (jugador && escena && !escena->views().isEmpty()) {
        QGraphicsView* vista = escena->views().first();

        // Centrar la vista en el jugador (scroll automático)
        vista->centerOn(jugador);

        // Opcional: Lógica para "wrap around" o regeneración de objetos
        // cuando el jugador llega al final de la escena
        qreal posJugadorX = jugador->x();

        // Si el jugador pasa cierta posición, podrías:
        // 1. Teletransportarlo al inicio (wrap around)
        // 2. Generar nuevos obstáculos/enemigos
        // 3. Mover objetos que quedaron atrás hacia adelante

        // Ejemplo de wrap around simple:
        /*
        if (posJugadorX > sceneW - vistaAncho/2) {
            jugador->setPos(vistaAncho/2, jugador->y());
        }
        */
    }
}
