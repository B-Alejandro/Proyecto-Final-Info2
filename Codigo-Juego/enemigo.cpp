#include "enemigo.h"
#include "obstaculo.h"
#include <QBrush>
#include <QRandomGenerator>
#include <QGraphicsScene>

Enemigo::Enemigo(qreal w,
                 qreal h,
                 qreal sceneWidth,
                 qreal sceneHeight,
                 TipoMovimiento tipo,
                 int nivel)
    : Persona(w, h, sceneWidth, sceneHeight, tipo)
    ,   numeroNivel(nivel)
{
    setBrush(QBrush(Qt::red));
    speed = 3;
    changeDirectionTime = 2000;
    canJump = true;
    aiTimer = new QTimer(this);
    connect(aiTimer, &QTimer::timeout, this, &Enemigo::changeDirection);
    aiTimer->start(changeDirectionTime);
    randomizeDirection();
}

void Enemigo::handleInput()
{
    if (tipoMovimiento == TipoMovimiento::CON_GRAVEDAD && canJump) {
        tryJumpIfObstacleAhead();
    }
}

void Enemigo::changeDirection()
{
    randomizeDirection();
}

void Enemigo::randomizeDirection()
{
    QRandomGenerator* rng = QRandomGenerator::global();

    if (numeroNivel == 1) {
        upPressed = false;
        leftPressed = false;
        rightPressed = false;
        downPressed = true;
        speed = 0.0001;
        return;
    }

    if (tipoMovimiento == TipoMovimiento::RECTILINEO) {
        upPressed    = rng->bounded(4) == 0;
        downPressed  = rng->bounded(4) == 0;
        leftPressed  = rng->bounded(4) == 0;
        rightPressed = rng->bounded(4) == 0;

        if (upPressed && downPressed) {
            if (rng->bounded(2)) upPressed = false;
            else downPressed = false;
        }

        if (leftPressed && rightPressed) {
            if (rng->bounded(2)) leftPressed = false;
            else rightPressed = false;
        }
    } else {
        leftPressed = false;
        rightPressed = false;
    }
}

void Enemigo::tryJumpIfObstacleAhead()
{
    if (!onGround) return;

    bool obstaculoDetectado = detectarObstaculoAdelante();

    if (obstaculoDetectado) {
        vy = -10;
        onGround = false;
    }
}

bool Enemigo::detectarObstaculoAdelante()
{
    if (!scene()) return false;

    qreal distanciaDeteccion = rect().width() * 1.5;

    QRectF zonaDeteccion(
        x() + rect().width(),
        y(),
        distanciaDeteccion,
        rect().height()
        );

    QList<QGraphicsItem*> itemsAdelante = scene()->items(zonaDeteccion, Qt::IntersectsItemShape);

    for (QGraphicsItem* item : itemsAdelante) {
        if (item == this) continue;

        Obstaculo* obs = dynamic_cast<Obstaculo*>(item);
        if (obs) {
            if (obs->x() > x()) {
                return true;
            }
        }
    }

    return false;
}

void Enemigo::tryJump()
{
    if (onGround && QRandomGenerator::global()->bounded(100) < 2) {
        vy = -10;
        onGround = false;
    }
}
