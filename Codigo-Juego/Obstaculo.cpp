// ============ obstaculo.cpp ============
#include "obstaculo.h"
#include <QBrush>
#include <QPen>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QWidget>
#include <QDebug>

Obstaculo::Obstaculo(qreal x,
                     qreal y,
                     qreal w,
                     qreal h,
                     QColor color)
    : QGraphicsRectItem(0, 0, w, h),
    currentColor(color),
    tieneTextura(false),
    repetirTextura(true)
{
    setPos(x, y);
    setBrush(QBrush(color));
    setPen(QPen(Qt::NoPen));
}

void Obstaculo::setColor(const QColor& color)
{
    currentColor = color;
    setBrush(QBrush(color));
    tieneTextura = false; // Si cambia el color, desactivar textura
}

void Obstaculo::setBorderColor(const QColor& color, int width)
{
    QPen pen(color);
    pen.setWidth(width);
    setPen(pen);
}

void Obstaculo::setTextura(const QString& rutaImagen, bool repetir)
{
    QPixmap pixmap(rutaImagen);

    if (pixmap.isNull()) {
        qDebug() << "⚠️ No se pudo cargar la textura:" << rutaImagen;
        tieneTextura = false;
        return;
    }

    setTextura(pixmap, repetir);
}

void Obstaculo::setTextura(const QPixmap& pixmap, bool repetir)
{
    texturaPixmap = pixmap;
    repetirTextura = repetir;
    tieneTextura = true;

    // Forzar redibujado
    update();
}

void Obstaculo::limpiarTextura()
{
    tieneTextura = false;
    texturaPixmap = QPixmap();
    setBrush(QBrush(currentColor));
    update();
}

void Obstaculo::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    if (tieneTextura && !texturaPixmap.isNull()) {
        painter->save();

        QRectF rect = this->rect();

        if (repetirTextura) {
            // Crear un patrón repetido (tiling)
            QBrush texturaBrush(texturaPixmap);
            painter->setBrush(texturaBrush);
            painter->setPen(pen());
            painter->drawRect(rect);
        } else {
            // Escalar la imagen para que cubra todo el rectángulo
            painter->drawPixmap(rect.toRect(), texturaPixmap);

            // Dibujar el borde si existe
            if (pen().style() != Qt::NoPen) {
                painter->setPen(pen());
                painter->setBrush(Qt::NoBrush);
                painter->drawRect(rect);
            }
        }

        painter->restore();
    } else {
        // Dibujo normal si no hay textura
        QGraphicsRectItem::paint(painter, option, widget);
    }
}
