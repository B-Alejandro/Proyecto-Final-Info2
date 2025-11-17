#ifndef OBSTACULO_H
#define OBSTACULO_H

#include <QGraphicsRectItem>
#include <QColor>

class Obstaculo : public QGraphicsRectItem
{
public:
    Obstaculo(qreal x, qreal y, qreal w, qreal h, QColor color = Qt::gray);

    void setColor(const QColor& color);
    void setBorderColor(const QColor& color, int width = 2);

    QColor getColor() const { return currentColor; }

private:
    QColor currentColor;
};

#endif
