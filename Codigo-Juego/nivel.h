#ifndef NIVEL_H
#define NIVEL_H

#include <QObject>
#include <QGraphicsScene>

class Jugador;
class Enemigo;
class Obstaculo;

class Nivel : public QObject
{
    Q_OBJECT

public:
    Nivel(QGraphicsScene* s, QObject* parent = 0);

    void cargar();
    void actualizar();

private:
    void crearJugador();
    void crearEnemigos();
    void crearObstaculos();

private:
    QGraphicsScene* escena;

    Jugador* jugador;
    QList<Enemigo*> enemigos;
    QList<Obstaculo*> obstaculos;
};

#endif
