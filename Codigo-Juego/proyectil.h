#ifndef PROYECTIL_H
#define PROYECTIL_H

#pragma once
#include <QGraphicsObject>

class Persona; // forward

class Proyectil : public QGraphicsObject
{
    Q_OBJECT
public:
    // dirY = -1 (sube), +1 (baja)
    explicit Proyectil(qreal width, qreal height,
                       qreal speed, int dirY,
                       QGraphicsItem* parent = nullptr);

    QRectF boundingRect() const override;
    void paint(QPainter* painter,
               const QStyleOptionGraphicsItem* option,
               QWidget* widget) override;

    void setOwner(Persona* owner);

protected:
    void advance(int phase) override;

private:
    qreal m_w;
    qreal m_h;
    qreal m_speed;
    int   m_dirY;    // -1 hacia arriba, +1 hacia abajo
    Persona* m_owner;
};

#endif // PROYECTIL_H
