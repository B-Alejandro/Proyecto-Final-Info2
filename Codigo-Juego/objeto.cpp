#include "objeto.h"
#include <QPen>

Objeto::Objeto(qreal x, qreal y, qreal w, qreal h, QColor color)
    : QGraphicsRectItem(0, 0, w, h), currentColor(color)
{
    // Establecer posición del objeto
    setPos(x, y);

    // Establecer color de relleno
    setBrush(QBrush(color));

    // Sin borde por defecto
    setPen(QPen(Qt::NoPen));

    // Los objetos son estáticos, no necesitan flags especiales
    // pero pueden colisionar con otros items
}

void Objeto::setColor(const QColor &color)
{
    currentColor = color;
    setBrush(QBrush(color));
}

void Objeto::setBorderColor(const QColor &color, int width)
{
    QPen pen(color);
    pen.setWidth(width);
    setPen(pen);
}
