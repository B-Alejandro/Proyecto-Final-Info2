#include "enemigo.h"
#include <QBrush>
#include <QRandomGenerator>

Enemigo::Enemigo(qreal w,
                 qreal h,
                 qreal sceneWidth,
                 qreal sceneHeight,
                 TipoMovimiento tipo)
    : Persona(w, h, sceneWidth, sceneHeight, tipo)
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
        tryJump();
    }
}

void Enemigo::changeDirection()
{
    randomizeDirection();
}

void Enemigo::randomizeDirection()
{
    QRandomGenerator* rng = QRandomGenerator::global();

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
        int direction = rng->bounded(3);

        leftPressed  = (direction == 0);
        rightPressed = (direction == 1);
    }
}

void Enemigo::tryJump()
{
    if (onGround && QRandomGenerator::global()->bounded(100) < 2) {
        vy = -10;
        onGround = false;
    }
}
