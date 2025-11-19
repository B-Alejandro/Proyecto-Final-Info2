#include "nivelbase.h"
#include "jugador.h"
#include "enemigo.h"
#include "obstaculo.h"
#include "persona.h"
#include <QBrush>
#include <QColor>
#include <QGraphicsView>

/*
  Constructor base
*/
NivelBase::NivelBase(QGraphicsScene* escena, int numero, QObject* parent)
    : QObject(parent),
    escena(escena),
    jugador(0),
    numeroNivel(numero),
    sceneW(0),
    sceneH(0)
{
    // Inicializar dimensiones de la escena
    if (escena) {
        sceneW = escena->sceneRect().width();
        sceneH = escena->sceneRect().height();

        // Si las dimensiones son 0, usar valores por defecto
        if (sceneW == 0) sceneW = 800;
        if (sceneH == 0) sceneH = 600;
    }
}

/*
  Configura la escena (fondo, viewport, etc.)
*/
void NivelBase::configurarEscena()
{
    if (!escena) return;

    escena->clear();

    if (!escena->views().isEmpty())
        escena->views().first()->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);

    escena->setBackgroundBrush(QBrush(Qt::cyan));
}

/*
  Carga todos los elementos del nivel
*/
void NivelBase::cargarElementos()
{
    configurarEscena();
    configurarNivel();    // Cada nivel define su configuración específica
    //crearObstaculos();    // Cada nivel crea sus obstáculos
    crearEnemigos();      // Cada nivel crea sus enemigos
}

/*
  Crea un jugador en la posicion dada con el tipo de movimiento especificado
*/
void NivelBase::crearJugador(qreal x, qreal y, TipoMovimiento tipo)
{
    if (!escena) return;

    // Asegurarse de que las dimensiones son válidas
    if (sceneW == 0 || sceneH == 0) {
        sceneW = escena->sceneRect().width();
        sceneH = escena->sceneRect().height();
        if (sceneW == 0) sceneW = 800;
        if (sceneH == 0) sceneH = 600;
    }

    int size = sceneH * 0.1;
    jugador = new Jugador(size, size, sceneW, sceneH, tipo);
    jugador->setPos(x, y);
    jugador->setSpeed(5);
    escena->addItem(jugador);
    jugador->setFocus();
}

/*
  Actualización por frame (virtual, cada nivel puede sobrescribir)
*/
void NivelBase::actualizar()
{
    // Lógica base de actualización
    // Los niveles pueden sobrescribir este método
}
