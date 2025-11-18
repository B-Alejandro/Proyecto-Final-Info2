#ifndef NIVELBASE_H
#define NIVELBASE_H

#include <QObject>
#include <QGraphicsScene>
#include <QList>

// Forward declarations
class Jugador;
class Enemigo;
class Obstaculo;

// Importar el enum desde persona.h
enum class TipoMovimiento;

/*
  Clase base para niveles
*/
class NivelBase : public QObject
{
    Q_OBJECT

public:
    NivelBase(QGraphicsScene* escena, int numero, QObject* parent = 0);
    virtual ~NivelBase() {}

    virtual void cargarElementos();
    virtual void actualizar();

protected:
    // Métodos para crear elementos comunes
    void crearJugador(qreal x, qreal y, TipoMovimiento tipo);
    void configurarEscena();

    // Métodos virtuales para que cada nivel implemente
    virtual void crearEnemigos() {}
    virtual void crearObstaculos() {}
    virtual void configurarNivel() {}

    QGraphicsScene* escena;
    Jugador* jugador;
    QList<Enemigo*> enemigos;
    QList<Obstaculo*> obstaculos;
    int numeroNivel;

    // Variables de dimensiones de la escena
    qreal sceneW;
    qreal sceneH;
};

#endif
