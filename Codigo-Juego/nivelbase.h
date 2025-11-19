#ifndef NIVELBASE_H
#define NIVELBASE_H

#include <QObject>
#include <QGraphicsScene>
#include <QList>

// Forward declarations
class Jugador;
class Enemigo;
class Obstaculo;
class Juego;

// Importar el enum desde persona.h
enum class TipoMovimiento;

/*
  Clase base para niveles
*/
class NivelBase : public QObject
{
    Q_OBJECT

public:
    NivelBase(Juego* juego, int numero, QObject* parent = 0);
    virtual ~NivelBase();

    virtual void cargarElementos();
    virtual void actualizar();

    QGraphicsScene* getEscena() { return escena; }

protected:
    // Métodos para crear elementos comunes
    void crearJugador(qreal x, qreal y, TipoMovimiento tipo);
    void configurarEscena();

    // Crear la escena con dimensiones específicas
    virtual void crearEscena(int ancho, int alto);

    // Métodos virtuales para que cada nivel implemente
    virtual void crearEnemigos() {}
    virtual void crearObstaculos() {}
    virtual void configurarNivel() {}

    QGraphicsScene* escena;
    Jugador* jugador;
    QList<Enemigo*> enemigos;
    QList<Obstaculo*> obstaculos;

    Juego* juego;
    int numeroNivel;

    // Variables de dimensiones de la escena
    qreal sceneW;
    qreal sceneH;
};

#endif
