#include "proyectil.h"
#include "persona.h"
#include <QPainter>
#include <QGraphicsScene>

    Proyectil::Proyectil(qreal width, qreal height,
                         qreal speed, int dirY,
                         QGraphicsItem* parent)
    : QGraphicsObject(parent),
    m_w(width),
    m_h(height),
    m_speed(speed),
    m_dirY(dirY),
    m_owner(nullptr)
{
    if (m_dirY == 0) m_dirY = -1; // por si acaso
    setAcceptedMouseButtons(Qt::NoButton);
    setFlag(ItemIsFocusable, false);
}

QRectF Proyectil::boundingRect() const
{
    return QRectF(-m_w/2.0, -m_h/2.0, m_w, m_h);
}

void Proyectil::paint(QPainter* painter,
                      const QStyleOptionGraphicsItem*,
                      QWidget*)
{
    painter->setBrush(Qt::yellow);
    painter->setPen(Qt::NoPen);
    painter->drawEllipse(boundingRect());
}

void Proyectil::setOwner(Persona* owner)
{
    m_owner = owner;
}

void Proyectil::advance(int phase)
{
    if (!phase) return;

    // mover solo en Y
    moveBy(0, m_speed * m_dirY);

    // colisiones
    QList<QGraphicsItem*> coll = collidingItems(Qt::IntersectsItemShape);
    for (QGraphicsItem* it : coll) {
        Persona* p = dynamic_cast<Persona*>(it);
        if (!p) continue;
        if (p == m_owner) continue;  // no dañar al que disparó

        p->takeDamage(1);

        if (scene()) scene()->removeItem(this);
        deleteLater();
        return;
    }

    // fuera de escena -> borrar
    if (scene()) {
        QRectF sr = scene()->sceneRect();
        QPointF pos = scenePos();
        if (pos.y() < sr.top() - 50 || pos.y() > sr.bottom() + 50) {
            scene()->removeItem(this);
            deleteLater();
        }
    }
}
