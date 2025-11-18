#include "nivel3.h"
#include "jugador.h"
#include "enemigo.h"
#include "obstaculo.h"
#include <QColor>

/*
  Constructor
*/
Nivel3::Nivel3(QGraphicsScene* escena, QObject* parent)
    : NivelBase(escena, 3, parent)
{
}

/*
  Configuración específica del nivel 3
*/
void Nivel3::configurarNivel()
{
    // Crear jugador en el suelo, no en el aire
    crearJugador(sceneW * 0.5, sceneH - sceneH * 0.1 - 40, TipoMovimiento::RECTILINEO);
    //                          ^^^^^^^^^^^^^^^^^^^^^^^^^
    //                          Misma posición Y que en Nivel 1
}
/*
  Crea enemigos para el nivel 3
*/
void Nivel3::crearEnemigos()
{
    int size = sceneH * 0.12;

    // Enemigo 1
    Enemigo* e1 = new Enemigo(size, size, sceneW, sceneH, TipoMovimiento::RECTILINEO);
    e1->setPos(sceneW * 0.2, sceneH * 0.3);
    e1->setSpeed(4);
    enemigos.append(e1);
    escena->addItem(e1);

    // Enemigo 2
    Enemigo* e2 = new Enemigo(size, size, sceneW, sceneH, TipoMovimiento::RECTILINEO);
    e2->setPos(sceneW * 0.8, sceneH * 0.7);
    e2->setSpeed(3);
    enemigos.append(e2);
    escena->addItem(e2);
}

/*
  Crea obstáculos para el nivel 3
*/
void Nivel3::crearObstaculos()
{
    int sueloAltura = 40;

    // Suelo principal
    Obstaculo* suelo = new Obstaculo(0, sceneH - sueloAltura, sceneW, sueloAltura, QColor(139, 69, 19));
    suelo->setBorderColor(Qt::black, 2);
    obstaculos.append(suelo);
    escena->addItem(suelo);

    // Obstáculo central
    Obstaculo* muro = new Obstaculo(sceneW * 0.45, sceneH * 0.4, 100, sceneH * 0.3, QColor(128, 128, 128));
    muro->setBorderColor(Qt::black, 2);
    obstaculos.append(muro);
    escena->addItem(muro);
}

/*
  Actualización del nivel 3
*/
void Nivel3::actualizar()
{
    // El jugador se actualiza automáticamente con eventos de teclado
    // Aquí puedes agregar lógica adicional del nivel
}
