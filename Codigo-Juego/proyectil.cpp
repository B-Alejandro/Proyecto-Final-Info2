#include "proyectil.h"
#include "persona.h"
#include "obstaculo.h"  // *** NUEVO ***
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
    if (m_dirY == 0) m_dirY = -1;
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

    moveBy(0, m_speed * m_dirY);

    // Colisiones
    QList<QGraphicsItem*> coll = collidingItems(Qt::IntersectsItemShape);
    for (QGraphicsItem* it : coll) {
        Persona* p = dynamic_cast<Persona*>(it);
        if (p) {
            if (p == m_owner) continue;

            // Aplicar daño según quién disparó
            if (m_dirY < 0) {
                // Proyectil hacia arriba = del jugador
                p->recibirDanio(1);
            } else {
                // Proyectil hacia abajo = de enemigo
                p->recibirDanio(1);  // *** Cambiado a 1 vida ***
            }

            if (scene()) scene()->removeItem(this);
            deleteLater();
            return;
        }

        // *** NUEVO: Daño a obstáculos móviles ***
        Obstaculo* obs = dynamic_cast<Obstaculo*>(it);
        if (obs && obs->isMovil() && m_dirY < 0) {  // Solo proyectiles del jugador
            obs->recibirDanio(1);

            if (scene()) scene()->removeItem(this);
            deleteLater();
            return;
        }
    }

    // Fuera de escena -> borrar
    if (scene()) {
        QRectF sr = scene()->sceneRect();
        QPointF pos = scenePos();
        if (pos.y() < sr.top() - 50 || pos.y() > sr.bottom() + 50) {
            scene()->removeItem(this);
            deleteLater();
        }
    }
}
