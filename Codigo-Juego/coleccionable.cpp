#include "coleccionable.h"
#include "jugador.h"
#include <QPainter>
#include <QGraphicsScene>
#include <QDebug>
#include <QFont>      // Necesario para usar QFont
#include <QRandomGenerator> // Para variar el emoji si es necesario

Coleccionable::Coleccionable(qreal x,
                             qreal y,
                             qreal tamanio,
                             int tipo,
                             QGraphicsItem* parent)
    : QGraphicsObject(parent)
    , tamanio(tamanio)
    , tipo(tipo)
    , recolectado(false)
    , fillColor(Qt::gray) // Color por defecto (se cambiará en Nivel3)
{
    // Usamos el emoji de moneda 💰
    textTexture = "📃";

    // Opcional: Variar el emoji según el tipo, si lo deseas
    // if (tipo == 1) textTexture = "✨";
    // if (tipo == 2) textTexture = "💎";

    setPos(x, y);
    setZValue(50);
    setAcceptedMouseButtons(Qt::NoButton);
    setFlag(ItemIsFocusable, false);

    qDebug() << "Coleccionable creado en:" << x << y;
}

QRectF Coleccionable::boundingRect() const
{
    // Aumentamos el margen para que el emoji quepa bien
    qreal margen = tamanio * 0.5;
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
        painter->setRenderHint(QPainter::TextAntialiasing);

        // 1. Dibujar el fondo del coleccionable (e.g., un círculo para un borde)
        painter->setPen(QPen(Qt::darkGray, 3));
        painter->setBrush(QBrush(QColor(40, 40, 40))); // Fondo oscuro para que el emoji destaque
        painter->drawEllipse(QRectF(-tamanio + 2, -tamanio + 2, tamanio * 2 - 4, tamanio * 2 - 4));


        // 2. Dibujar el emoji/texto de textura
        QFont font;
        // El tamaño de la fuente se ajusta al tamaño del coleccionable
        font.setPointSize(static_cast<int>(tamanio * 1.5));
        font.setBold(true);
        painter->setFont(font);

        // Usamos el color establecido por setFillColor() (dorado en Nivel3)
        painter->setPen(QPen(fillColor));
        painter->setBrush(Qt::NoBrush);

        // Crear el rectángulo donde se dibujará el texto (centrado en el ítem)
        QRectF textRect(-tamanio, -tamanio, tamanio * 2, tamanio * 2);

        // Dibujar el texto centrado
        painter->drawText(textRect, Qt::AlignCenter, textTexture);

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
