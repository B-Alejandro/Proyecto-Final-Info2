#include "coleccionable.h"
#include "jugador.h"
#include <QPainter>
#include <QGraphicsScene>
#include <QDebug>

Coleccionable::Coleccionable(qreal x,
                             qreal y,
                             qreal tamanio,
                             int tipo,
                             QGraphicsItem* parent)
    : QGraphicsObject(parent)
    , tamanio(tamanio)
    , tipo(tipo)
    , recolectado(false)
{
    setPos(x, y);
    setZValue(50);
    setAcceptedMouseButtons(Qt::NoButton);
    setFlag(ItemIsFocusable, false);

    qDebug() << "Coleccionable creado en:" << x << y;
}

QRectF Coleccionable::boundingRect() const
{
    qreal margen = tamanio * 0.2;
    return QRectF(-tamanio - margen,
                  -tamanio - margen,
                  (tamanio + margen) * 2,
                  (tamanio + margen) * 2);
}

void Coleccionable::paint(QPainter* painter,
                          const QStyleOptionGraphicsItem*,
                          QWidget*)
{
    if (!recolectado) {
        painter->save();
        painter->setRenderHint(QPainter::Antialiasing);

        painter->setPen(QPen(Qt::darkGray, 2));
        painter->setBrush(QBrush(Qt::gray));
        painter->drawRect(QRectF(-tamanio, -tamanio, tamanio * 2, tamanio * 2));

        painter->restore();
    }
}

void Coleccionable::verificarColision()
{
    if (recolectado) return;

    QList<QGraphicsItem*> colisiones = collidingItems(Qt::IntersectsItemShape);

    for (QGraphicsItem* item : colisiones) {
        Jugador* jugador = dynamic_cast<Jugador*>(item);

        if (jugador && jugador->estaVivo()) {
            recolectar();
            break;
        }
    }
}

void Coleccionable::recolectar()
{
    if (recolectado) return;

    recolectado = true;

    qDebug() << "✨ ¡Coleccionable recolectado!";

    emit coleccionableRecolectado(this);

}
