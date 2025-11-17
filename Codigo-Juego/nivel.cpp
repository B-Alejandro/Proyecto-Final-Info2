#include "Nivel.h"
#include "Jugador.h"
#include "Enemigo.h"
#include "Obstaculo.h"

#include <QBrush>
#include <QColor>
#include <QGraphicsView>

Nivel::Nivel(QGraphicsScene* s, QObject* parent)
    : QObject(parent), escena(s), jugador(0)
{
}

void Nivel::cargar()
{
    escena->clear();

    // Asegura que el modo de actualizacion sea correcto
    // Esto evita rastros en la pantalla
    escena->views().first()->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);

    int w = escena->width();
    int h = escena->height();

    escena->setBackgroundBrush(QBrush(Qt::cyan));

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

    jugador = new Jugador(size, size, w, h, TipoMovimiento::CON_GRAVEDAD);
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

    Enemigo* e1 = new Enemigo(size, size, w, h, TipoMovimiento::CON_GRAVEDAD);
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

    Obstaculo* suelo = new Obstaculo(0, h - sueloAltura, w, sueloAltura, QColor(139,69,19));
    suelo->setBorderColor(Qt::black, 2);

    obstaculos.append(suelo);
    escena->addItem(suelo);
}
