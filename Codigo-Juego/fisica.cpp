#include "fisica.h"
#include <QGraphicsRectItem>

const double EPSILON = 9; // margen pequeño para superposicion
const double MARGIN = 0.5;
bool Fisica::colisionan(QGraphicsItem* a, QGraphicsItem* b)
{
    if (!a || !b || a == b) return false;
    return a->sceneBoundingRect().intersects(b->sceneBoundingRect());
}

QList<QGraphicsItem*> Fisica::obtenerColisiones(QGraphicsItem* item)
{
    if (!item) return QList<QGraphicsItem*>();
    return item->collidingItems();
}

bool Fisica::colisionDebajo(QGraphicsItem* item)
{
    if (!item) return false;

    QRectF myRect = item->sceneBoundingRect();
    QList<QGraphicsItem*> colisiones = item->collidingItems();

    for (QGraphicsItem* otro : colisiones) {
        if (otro->type() == QGraphicsRectItem::Type) {
            QRectF r = otro->sceneBoundingRect();

            if (myRect.bottom() <= r.top() + EPSILON &&
                myRect.bottom() >= r.top() - EPSILON) {
                return true;
            }
        }
    }

    return false;
}



bool Fisica::hayColisionConObstaculos(QGraphicsItem* item)
{
    if (!item) return false;

    QList<QGraphicsItem*> cols = obtenerColisiones(item);

    for (QGraphicsItem* otro : cols) {
        if (otro->type() == QGraphicsRectItem::Type) {
            return true;
        }
    }

    return false;
}

void Fisica::aplicarMovimientoRectilineo(QGraphicsItem* item,
                                         double dx,
                                         double dy,
                                         double sceneW,
                                         double sceneH)
{
    if (!item || (dx == 0 && dy == 0)) return;

    double prevX = item->x();
    double prevY = item->y();

    QRectF box = item->boundingRect();
    double w = box.width();
    double h = box.height();

    double newX = prevX + dx;
    double newY = prevY + dy;

    if (newX < 0) newX = 0;
    if (newX + w > sceneW) newX = sceneW - w;
    if (newY < 0) newY = 0;
    if (newY + h > sceneH) newY = sceneH - h;

    item->setPos(newX, newY);

    if (hayColisionConObstaculos(item)) {
        item->setPos(prevX, prevY);
    }
}
// margen minúsculo para superposicion

double Fisica::resolverColisionVertical(QGraphicsItem* item,
                                        double prevY,
                                        double sceneH,
                                        double& vy,
                                        bool& onGround)
{
    QRectF box = item->boundingRect();
    double h = box.height();

    onGround = false;

    if (item->y() < 0) {
        vy = 0;
        return 0;
    }

    if (item->y() + h > sceneH) {
        vy = 0;
        onGround = true;
        return sceneH - h + MARGIN;
    }

    QList<QGraphicsItem*> colisiones = item->collidingItems();

    for (QGraphicsItem* otro : colisiones) {
        if (otro->type() != QGraphicsRectItem::Type) continue;

        QRectF r = otro->sceneBoundingRect();

        // Caida
        if (vy > 0 && prevY + h <= r.top() + MARGIN) {
            vy = 0;
            onGround = true;
            return r.top() - h + MARGIN;
        }

        // Salto
        if (vy < 0 && prevY >= r.bottom() - MARGIN) {
            vy = 0;
            return r.bottom() - MARGIN;
        }
    }

    // onGround basado en colisionDebajo
    if (colisionDebajo(item)) onGround = true;

    return item->y();
}

double Fisica::resolverColisionHorizontal(QGraphicsItem* item,
                                          double prevX,
                                          double sceneW)
{
    QRectF box = item->boundingRect();
    double w = box.width();

    if (item->x() < 0) return 0;
    if (item->x() + w > sceneW) return sceneW - w + MARGIN;

    QList<QGraphicsItem*> colisiones = item->collidingItems();
    for (QGraphicsItem* otro : colisiones) {
        if (otro->type() != QGraphicsRectItem::Type) continue;

        QRectF r = otro->sceneBoundingRect();

        // Superposicion minima horizontal
        if (item->x() + w > r.left() && item->x() < r.right()) {
            if (prevX + w <= r.left() + MARGIN) {
                return r.left() - w + MARGIN;
            } else if (prevX >= r.right() - MARGIN) {
                return r.right() - MARGIN;
            }
        }
    }

    return item->x();
}

void Fisica::aplicarMovimientoConGravedad(QGraphicsItem* item,
                                          double dx,
                                          double& vy,
                                          double gravedad,
                                          bool& onGround,
                                          double sceneW,
                                          double sceneH)
{
    if (!item) return;

    double prevX = item->x();
    double prevY = item->y();

    // Movimiento horizontal
    if (dx != 0) {
        item->setX(item->x() + dx);
        double fixedX = resolverColisionHorizontal(item, prevX, sceneW);
        item->setX(fixedX);
    }

    // Movimiento vertical con gravedad
    vy += gravedad;
    item->setY(item->y() + vy);

    double fixedY = resolverColisionVertical(item, prevY, sceneH, vy, onGround);
    item->setY(fixedY);
}
