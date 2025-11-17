#include "persona.h"
#include <QBrush>
#include <QPen>
#include <QTimer>
#include <QKeyEvent>
#include <QGraphicsScene>
#include <QDebug>
Persona::Persona(qreal w, qreal h,
                 qreal sceneWidth, qreal sceneHeight,
                 TipoMovimiento tipo)
    : QGraphicsRectItem(0, 0, w, h),
    tipoMovimiento(tipo),
    sceneW(sceneWidth),
    sceneH(sceneHeight)
{
    setPen(QPen(Qt::NoPen));
    setBrush(QBrush(Qt::blue));

    speed = 5.0;

    upPressed = false;
    downPressed = false;
    leftPressed = false;
    rightPressed = false;

    vy = 0.0;
    g = 0.5;
    onGround = false;

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Persona::updateMovement);
    timer->start(16);

    setFlag(QGraphicsItem::ItemIsFocusable);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges);

    setCacheMode(QGraphicsItem::DeviceCoordinateCache);
}

void Persona::setTipoMovimiento(TipoMovimiento tipo)
{
    tipoMovimiento = tipo;
    vy = 0.0;
    onGround = false;
}

void Persona::handleInput()
{
}

void Persona::updateMovement()
{
    handleInput();

    if (tipoMovimiento == TipoMovimiento::RECTILINEO) {
        updateMovementRectilineo();
    } else {
        updateMovementConGravedad();
    }
}

void Persona::updateMovementRectilineo()
{
    double dx = 0.0;
    double dy = 0.0;

    if (upPressed) dy -= speed+3;
    if (downPressed) dy += speed+3;
    if (leftPressed) dx -= speed+3;
    if (rightPressed) dx += speed+3;

    if (dx == 0 && dy == 0) return;

    double w = rect().width();
    double h = rect().height();

    double prevX = x();
    double prevY = y();

    double newX = x() + dx;
    double newY = y() + dy;

    if (newX < 0) newX = 0;
    if (newX + w > sceneW) newX = sceneW - w;
    if (newY < 0) newY = 0;
    if (newY + h > sceneH) newY = sceneH - h;

    setPos(newX, newY);

    QList<QGraphicsItem*> collisions = collidingItems();

    for (QGraphicsItem* item : collisions) {
        if (item->type() == QGraphicsRectItem::Type) {
            setPos(prevX, prevY);
            return;
        }
    }
}

void Persona::updateMovementConGravedad()
{
    double w = rect().width();
    double h = rect().height();

    double dx = 0.0;
    if (leftPressed) dx -= speed+6;
    if (rightPressed) dx += speed+6;

    if (dx != 0.0) {
        double prevX = x();
        setX(x() + dx);

        if (x() < 0) {
            setX(0);
        } else if (x() + w > sceneW) {
            setX(sceneW - w);
        } else {
            QList<QGraphicsItem*> collisions = collidingItems();

            for (QGraphicsItem* item : collisions) {
                if (item->type() == QGraphicsRectItem::Type) {
                    setX(prevX);
                    break;
                }
            }
        }
    }

    double prevY = y();

    vy += g;
    setY(y() + vy);

    onGround = false;

    if (y() < 0) {
        setY(0);
        vy = 0;
    } else if (y() + h > sceneH) {
        setY(sceneH - h);
        vy = 0;
        onGround = true;
    } else {
        QList<QGraphicsItem*> collisions = collidingItems();

        for (QGraphicsItem* item : collisions) {
            if (item->type() != QGraphicsRectItem::Type) continue;

            QRectF otherRect = item->sceneBoundingRect();
            QRectF myRect = sceneBoundingRect();

            if (vy > 0) {
                if (prevY + h <= otherRect.top() + 2) {
                    setY(otherRect.top() - h);
                    vy = 0;
                    onGround = true;
                    break;
                }
            } else if (vy < 0) {
                if (prevY >= otherRect.bottom() - 2) {
                    setY(otherRect.bottom());
                    vy = 0;
                    break;
                }
            }
        }
    }
}

bool Persona::checkCollisionWith(Persona* other)
{
    if (!other || other == this) return false;
    return sceneBoundingRect().intersects(other->sceneBoundingRect());
}

bool Persona::checkCollisionBelow()
{
    for (QGraphicsItem* item : collidingItems()) {
        if (item != this && item->type() == QGraphicsRectItem::Type) {
            QRectF otherRect = item->sceneBoundingRect();
            QRectF myRect = sceneBoundingRect();

            if (myRect.bottom() <= otherRect.top() + 5 &&
                myRect.bottom() >= otherRect.top()) {
                return true;
            }
        }
    }
    return false;
}

QList<Persona*> Persona::getCollidingPersonas()
{
    QList<Persona*> list;
    QList<QGraphicsItem*> collisions = collidingItems();

    for (QGraphicsItem* item : collisions) {
        Persona* p = dynamic_cast<Persona*>(item);
        if (p && p != this) list.append(p);
    }

    return list;
}
