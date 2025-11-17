#include "Obstaculo.h"
#include <QBrush>
#include <QPen>

Obstaculo::Obstaculo(qreal x, qreal y, qreal w, qreal h, QColor color)
    : QGraphicsRectItem(0, 0, w, h), currentColor(color)
{
    setPos(x, y);
    setBrush(QBrush(color));
    setPen(QPen(Qt::NoPen));
}

void Obstaculo::setColor(const QColor& color)
{
    currentColor = color;
    setBrush(QBrush(color));
}

void Obstaculo::setBorderColor(const QColor& color, int width)
{
    QPen pen(color);
    pen.setWidth(width);
    setPen(pen);
}
