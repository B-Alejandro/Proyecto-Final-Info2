#ifndef NIVEL_H
#define NIVEL_H

#include <QObject>
#include <QGraphicsScene>
#include "jugador.h"
#include "enemigo.h"
#include "obstaculo.h"

class Nivel : public QObject
{
    Q_OBJECT

public:
    Nivel(QGraphicsScene* s, int nivel, QObject* parent = nullptr);

    void cargar();
    void actualizar();

private:
    QGraphicsScene* escena;
    Jugador* jugador;
    QList<Enemigo*> enemigos;
    QList<Obstaculo*> obstaculos;

    int numeroNivel;
    TipoMovimiento modoMov;

private:
    void crearJugador();
    void crearEnemigos();
    void crearObstaculos();

    // Metodos para cada nivel
    void setUpLevel1();
    void setUpLevel2();
    void setUpLevel3();
};

#endif
