#include "Nivel.h"
#include "Jugador.h"
#include "Enemigo.h"
#include "Obstaculo.h"

#include <QBrush>
#include <QColor>
#include <QGraphicsView>

Nivel::Nivel(QGraphicsScene* s, int nivel, QObject* parent)
    : QObject(parent), escena(s), jugador(0), numeroNivel(nivel)
{
    if (numeroNivel == 1)
        modoMov = TipoMovimiento::RECTILINEO;
    else if (numeroNivel == 2)
        modoMov = TipoMovimiento::CON_GRAVEDAD;
    else if (numeroNivel == 3)
        modoMov = TipoMovimiento::RECTILINEO;
    else
        modoMov = TipoMovimiento::RECTILINEO;
}

void Nivel::cargar()
{
    escena->clear();

    if (!escena->views().isEmpty())
        escena->views().first()->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);

    int w = escena->width();
    int h = escena->height();

    escena->setBackgroundBrush(QBrush(Qt::cyan));

    // Llamada al metodo del nivel segun corresponda
    if (numeroNivel == 1)
        setUpLevel1();
    else if (numeroNivel == 2)
        setUpLevel2();
    else if (numeroNivel == 3)
        setUpLevel3();

    crearJugador();
    crearEnemigos();
    crearObstaculos();
}

void Nivel::actualizar()
{
}

void Nivel::crearJugador()
{
    int w = escena->width();
    int h = escena->height();

    int size = h * 0.1;

    jugador = new Jugador(size, size, w, h, modoMov);
    jugador->setPos(50, h - size - 40);
    jugador->setSpeed(5);

    escena->addItem(jugador);
    jugador->setFocus();
}

void Nivel::crearEnemigos()
{
    int w = escena->width();
    int h = escena->height();

    int size = h * 0.12;

    Enemigo* e1 = new Enemigo(size, size, w, h, modoMov);
    e1->setPos(w * 0.4, h * 0.5);
    e1->setSpeed(3);

    enemigos.append(e1);
    escena->addItem(e1);
}

void Nivel::crearObstaculos()
{
    int w = escena->width();
    int h = escena->height();

    int sueloAltura = 40;

    Obstaculo* suelo = new Obstaculo(0, h - sueloAltura, w, sueloAltura, QColor(139, 69, 19));
    suelo->setBorderColor(Qt::black, 2);

    obstaculos.append(suelo);
    escena->addItem(suelo);
}

// ===========================================================
// Niveles separados (aun no implementados)
// ===========================================================

void Nivel::setUpLevel1()
{
    // Aqui va lo que tenga el nivel 1
}

void Nivel::setUpLevel2()
{
    // Aqui va lo que tenga el nivel 2
}

void Nivel::setUpLevel3()
{
    // Aqui va lo que tenga el nivel 3
}
