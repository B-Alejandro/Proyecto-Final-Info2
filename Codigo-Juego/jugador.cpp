#include "jugador.h"
#include <QBrush>

Jugador::Jugador(qreal w,
                 qreal h,
                 qreal sceneWidth,
                 qreal sceneHeight,
                 TipoMovimiento tipo)
    : Persona(w, h, sceneWidth, sceneHeight, tipo)
{
    setBrush(QBrush(Qt::green));

    setFlag(QGraphicsItem::ItemIsFocusable);
    setFocus();
}

void Jugador::keyPressEvent(QKeyEvent* event)
{
    if (tipoMovimiento == TipoMovimiento::RECTILINEO) {

        switch (event->key()) {
        case Qt::Key_Up:
        case Qt::Key_W:
            upPressed = true;
            break;

        case Qt::Key_Down:
        case Qt::Key_S:
            downPressed = true;
            break;

        case Qt::Key_Left:
        case Qt::Key_A:
            leftPressed = true;
            break;

        case Qt::Key_Right:
        case Qt::Key_D:
            rightPressed = true;
            break;
        }

    } else {

        switch (event->key()) {
        case Qt::Key_Left:
        case Qt::Key_A:
            leftPressed = true;
            break;

        case Qt::Key_Right:
        case Qt::Key_D:
            rightPressed = true;
            break;

        case Qt::Key_Up:
        case Qt::Key_W:
        case Qt::Key_Space:
            if (onGround) {
                vy = -10;
                onGround = false;
            }
            break;
        }
    }
}

void Jugador::keyReleaseEvent(QKeyEvent* event)
{
    if (tipoMovimiento == TipoMovimiento::RECTILINEO) {

        switch (event->key()) {
        case Qt::Key_Up:
        case Qt::Key_W:
            upPressed = false;
            break;

        case Qt::Key_Down:
        case Qt::Key_S:
            downPressed = false;
            break;

        case Qt::Key_Left:
        case Qt::Key_A:
            leftPressed = false;
            break;

        case Qt::Key_Right:
        case Qt::Key_D:
            rightPressed = false;
            break;
        }

    } else {

        switch (event->key()) {
        case Qt::Key_Left:
        case Qt::Key_A:
            leftPressed = false;
            break;

        case Qt::Key_Right:
        case Qt::Key_D:
            rightPressed = false;
            break;
        }
    }
}
