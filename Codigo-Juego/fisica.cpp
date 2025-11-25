// ============ fisica.cpp (CORREGIDO) ============
#include "fisica.h"
#include <QGraphicsRectItem>
#include <QRectF>
#include <QDebug>

/*
  Funcion: colisionan
  Verifica si dos items se intersectan.
*/
bool Fisica::colisionan(QGraphicsItem* a, QGraphicsItem* b)
{
    if (!a || !b || a == b) return false;
    return a->sceneBoundingRect().intersects(b->sceneBoundingRect());
}

/*
  Funcion: obtenerColisiones
  Retorna la lista de items que colisionan con otro.
*/
QList<QGraphicsItem*> Fisica::obtenerColisiones(QGraphicsItem* item)
{
    if (!item) return QList<QGraphicsItem*>();
    return item->collidingItems();
}

/*
  Funcion: colisionDebajo
  Verifica si hay un obstaculo justo debajo del item.
*/
bool Fisica::colisionDebajo(QGraphicsItem* item)
{
    if (!item) return false;

    QRectF myRect = item->sceneBoundingRect();
    QList<QGraphicsItem*> colisiones = item->collidingItems();

    for (QGraphicsItem* otro : colisiones) {
        if (otro->type() == QGraphicsRectItem::Type) {
            QRectF r = otro->sceneBoundingRect();

            if (myRect.bottom() <= r.top() + 5 &&
                myRect.bottom() >= r.top()) {
                return true;
            }
        }
    }

    return false;
}

/*
  Funcion: corregirColisionHorizontal
  Aplica un empuje para evitar que el item quede dentro del obstaculo.
*/
void Fisica::corregirColisionHorizontal(QGraphicsItem* item, QGraphicsItem* otro, double prevX)
{
    if (!item || !otro) return;

    QRectF itemRect = item->sceneBoundingRect();
    QRectF otroRect = otro->sceneBoundingRect();

    // Calcular el offset entre pos() y scenePos()
    double delta = item->scenePos().x() - item->pos().x();

    // Determinar de qué lado ocurrió la colisión
    double overlapIzq = itemRect.right() - otroRect.left();
    double overlapDer = otroRect.right() - itemRect.left();

    // Empujar hacia el lado con menor overlap
    if (overlapIzq < overlapDer) {
        // Empujar hacia la izquierda
        item->setX(otroRect.left() - itemRect.width() - delta);
    } else {
        // Empujar hacia la derecha
        item->setX(otroRect.right() - delta);
    }
}

/*
  Funcion: resolverColisionHorizontal
  Ajusta la posicion horizontal si hay colision con obstaculos.
*/
double Fisica::resolverColisionHorizontal(QGraphicsItem* item,
                                          double prevX,
                                          double sceneW)
{
    if (!item) return prevX;

    QRectF box = item->boundingRect();
    double w = box.width();

    // Limites de escena
    if (item->x() < 0)
        return 0;

    if (item->x() + w > sceneW)
        return sceneW - w;

    // Verificar colisiones con obstáculos
    QList<QGraphicsItem*> colisiones = item->collidingItems();

    for (QGraphicsItem* otro : colisiones) {
        if (otro->type() == QGraphicsRectItem::Type) {
            QRectF itemRect = item->sceneBoundingRect();
            QRectF otroRect = otro->sceneBoundingRect();

            // Solo corregir si hay overlap horizontal significativo
            double overlapIzq = itemRect.right() - otroRect.left();
            double overlapDer = otroRect.right() - itemRect.left();

            // Verificar que la colisión es principalmente lateral (no vertical)
            double overlapVertical = qMin(itemRect.bottom(), otroRect.bottom()) -
                                     qMax(itemRect.top(), otroRect.top());

            // Solo corregir si hay suficiente overlap vertical (colisión lateral real)
            if (overlapVertical > itemRect.height() * 0.3) {
                corregirColisionHorizontal(item, otro, prevX);
                return item->x();
            }
        }
    }

    return item->x();
}

/*
  Funcion: resolverColisionVertical
  Ajusta la posicion vertical del item con gravedad.
*/
double Fisica::resolverColisionVertical(QGraphicsItem* item,
                                        double prevY,
                                        double sceneH,
                                        double& vy,
                                        bool& onGround)
{
    if (!item) return prevY;

    QRectF box = item->boundingRect();
    double h = box.height();

    onGround = false;

    // Limite superior
    if (item->y() < 0) {
        vy = 0;
        return 0;
    }

    // Limite inferior (suelo de escena)
    if (item->y() + h >= sceneH) {
        vy = 0;
        onGround = true;
        return sceneH - h;
    }

    QList<QGraphicsItem*> colisiones = item->collidingItems();

    for (QGraphicsItem* otro : colisiones) {
        if (otro->type() != QGraphicsRectItem::Type) continue;

        QRectF itemRect = item->sceneBoundingRect();
        QRectF otroRect = otro->sceneBoundingRect();

        // Calcular overlaps
        double overlapArriba = itemRect.bottom() - otroRect.top();
        double overlapAbajo = otroRect.bottom() - itemRect.top();

        // Colisión desde arriba (aterrizando)
        if (vy >= 0 && overlapArriba > 0 && overlapArriba < overlapAbajo) {
            // Verificar que realmente estamos cayendo sobre el obstáculo
            if (prevY + h <= otroRect.top() + vy + 2) {
                vy = 0;
                onGround = true;
                double delta = item->scenePos().y() - item->pos().y();
                return otroRect.top() - h - delta;
            }
        }
        // Colisión desde abajo (golpeando techo)
        else if (vy < 0 && overlapAbajo > 0 && overlapAbajo < overlapArriba) {
            if (prevY >= otroRect.bottom() + vy - 2) {
                vy = 0;
                double delta = item->scenePos().y() - item->pos().y();
                return otroRect.bottom() - delta;
            }
        }
    }

    return item->y();
}

/*
  Funcion: hayColisionConObstaculos
  Devuelve true si el item toca un obstaculo rectangular.
*/
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

/*
  Funcion: aplicarMovimientoRectilineo
  Mueve el item en dx,dy con limites de escena.
*/
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

    // Limites de escena
    if (newX < 0) newX = 0;
    if (newX + w > sceneW) newX = sceneW - w;
    if (newY < 0) newY = 0;
    if (newY + h > sceneH) newY = sceneH - h;

    item->setPos(newX, newY);

    // Si hay colisión, volver a posición anterior
    if (hayColisionConObstaculos(item)) {
        item->setPos(prevX, prevY);
    }
}

/*
  Funcion: aplicarMovimientoConGravedad
  Mueve un item aplicando dx y gravedad vertical.
  CORREGIDO: Mejorada la detección de colisiones para permitir movimiento fluido.
*/
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

    QRectF box = item->boundingRect();
    double w = box.width();

    // === MOVIMIENTO HORIZONTAL ===
    if (dx != 0) {
        double newX = prevX + dx;

        // Limites de escena horizontal
        if (newX < 0) {
            newX = 0;
        } else if (newX + w > sceneW) {
            newX = sceneW - w;
        }

        item->setX(newX);

        // Resolver colisiones horizontales
        double fixedX = resolverColisionHorizontal(item, prevX, sceneW);
        item->setX(fixedX);
    }

    // === MOVIMIENTO VERTICAL (GRAVEDAD) ===
    vy += gravedad;

    // Limitar velocidad de caída máxima
    if (vy > 15) {
        vy = 15;
    }

    item->setY(item->y() + vy);

    // Resolver colisiones verticales
    double fixedY = resolverColisionVertical(item, prevY, sceneH, vy, onGround);
    item->setY(fixedY);
}
