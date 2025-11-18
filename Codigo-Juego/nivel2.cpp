#include "nivel2.h"
#include "jugador.h"
#include "enemigo.h"
#include "obstaculo.h"
#include "persona.h"
#include <QColor>

/*
  Nivel con scroll y gravedad
*/
Nivel2::Nivel2(QGraphicsScene* escena, QObject* parent)
    : NivelBase(escena, 2, parent)
{
}

/*
  Configuración específica del nivel 2
*/
void Nivel2::configurarNivel()
{
    // Crear jugador con gravedad
    crearJugador(120, sceneH - sceneH * 0.1 - 40, TipoMovimiento::CON_GRAVEDAD);
}

/*
  Crea enemigos para el nivel 2
*/
void Nivel2::crearEnemigos()
{
    int size = sceneH * 0.12;

    Enemigo* e1 = new Enemigo(size, size, sceneW, sceneH, TipoMovimiento::CON_GRAVEDAD);
    e1->setPos(sceneW * 0.6, sceneH * 0.3);
    e1->setSpeed(2);
    enemigos.append(e1);
    escena->addItem(e1);
}

/*
  Crea obstáculos para el nivel 2
*/
void Nivel2::crearObstaculos()
{
    int sueloAltura = 40;

    // Suelo principal
    Obstaculo* suelo = new Obstaculo(0, sceneH - sueloAltura, sceneW, sueloAltura, QColor(139, 69, 19));
    suelo->setBorderColor(Qt::black, 2);
    obstaculos.append(suelo);
    escena->addItem(suelo);

    // Plataforma flotante
    Obstaculo* plataforma = new Obstaculo(sceneW * 0.3, sceneH * 0.6, sceneW * 0.2, 20, QColor(101, 67, 33));
    plataforma->setBorderColor(Qt::black, 2);
    obstaculos.append(plataforma);
    escena->addItem(plataforma);
}

/*
  Actualización con scroll simple
*/
void Nivel2::actualizar()
{
    if (jugador) {
        // El jugador se mueve automáticamente con teclas
        // Aquí solo controlamos el scroll/límites

        qreal x = jugador->x();

        // Limitar posición del jugador para scroll
        if (x > sceneW * 0.6)
        {
            jugador->setX(sceneW * 0.6);
        }
    }
}
