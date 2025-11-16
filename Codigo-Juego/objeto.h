#ifndef OBJETO_H
#define OBJETO_H

#include <QGraphicsRectItem>
#include <QBrush>
#include <QColor>

class Objeto : public QGraphicsRectItem
{
public:
    // Constructor
    Objeto(qreal x, qreal y, qreal w, qreal h, QColor color = Qt::gray);

    // Setters para personalización
    void setColor(const QColor &color);
    void setBorderColor(const QColor &color, int width = 2);

    // Getters
    QColor getColor() const { return currentColor; }

private:
    QColor currentColor;
};

#endif // OBJETO_H
