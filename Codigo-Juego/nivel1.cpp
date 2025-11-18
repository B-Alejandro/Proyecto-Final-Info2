#include "nivel1.h"
#include "jugador.h"
#include "enemigo.h"
#include "obstaculo.h"
#include <QColor>

/*
  Constructor
*/
Nivel1::Nivel1(QGraphicsScene* escena, QObject* parent)
    : NivelBase(escena, 1, parent)
{
}

/*
  Configuración específica del nivel 1
*/
void Nivel1::configurarNivel()
{
    // Crear jugador con movimiento rectilíneo
    crearJugador(50, sceneH - sceneH * 0.1 - 40, TipoMovimiento::RECTILINEO);
}

/*
  Crea enemigos para el nivel 1
*/
void Nivel1::crearEnemigos()
{
    int size = sceneH * 0.12;

    Enemigo* e1 = new Enemigo(size, size, sceneW, sceneH, TipoMovimiento::RECTILINEO);
    e1->setPos(sceneW * 0.4, sceneH * 0.5);
    e1->setSpeed(3);
    enemigos.append(e1);
    escena->addItem(e1);
}

/*
  Crea obstáculos para el nivel 1
*/
void Nivel1::crearObstaculos()
{
    int sueloAltura = 40;

    Obstaculo* suelo = new Obstaculo(0, sceneH - sueloAltura, sceneW, sueloAltura, QColor(139, 69, 19));
    suelo->setBorderColor(Qt::black, 2);
    obstaculos.append(suelo);
    escena->addItem(suelo);
}

/*
  Actualización por frame del nivel 1
*/
void Nivel1::actualizar()
{
    // El jugador se actualiza automáticamente con eventos de teclado
    // Aquí puedes agregar lógica adicional del nivel (colisiones, etc.)
}
