// ============ obstaculo.cpp (CORREGIDO) ============
#include "obstaculo.h"
#include <QBrush>
#include <QPen>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QWidget>
#include <QDebug>

// Usamos Qt::transparent como color por defecto
Obstaculo::Obstaculo(qreal x,
                     qreal y,
                     qreal w,
                     qreal h,
                     QColor color) // Aquí se mantiene el color si se pasa uno
    : QGraphicsRectItem(0, 0, w, h),
    currentColor(color),
    tieneTextura(false),
    repetirTextura(true),
    danoValor(0)
{
    setPos(x, y);
    // Si se pasa un color, se usa. Si no (o se pasa transparente), se evita el relleno sólido.
    setBrush(QBrush(color));
    setPen(QPen(Qt::NoPen));
}

void Obstaculo::setColor(const QColor& color)
{
    currentColor = color;
    setBrush(QBrush(color));
    tieneTextura = false;
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
        // MUY IMPORTANTE: Revisa esta salida si no ves la imagen
        qDebug() << "⚠️ No se pudo cargar la textura:" << rutaImagen;
        tieneTextura = false;
        // Opcional: Volver al color original si falla la textura
        setBrush(QBrush(currentColor));
        return;
    }

    setTextura(pixmap, repetir);
}

void Obstaculo::setTextura(const QPixmap& pixmap, bool repetir)
{
    texturaPixmap = pixmap;
    repetirTextura = repetir;
    tieneTextura = true;

    // Desactivar el relleno sólido (brush) para que solo se vea la textura
    setBrush(Qt::NoBrush);

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
        painter->setPen(pen()); // Asegurarse de que el borde se mantenga si está configurado

        if (repetirTextura) {
            // Lógica de patrón repetido (tiling)
            QBrush texturaBrush(texturaPixmap);
            painter->setBrush(texturaBrush);
            painter->drawRect(rect);
        } else {
            // Lógica de escalado para que cubra todo el rectángulo (Sprite)
            // No usamos setBrush() aquí, solo dibujamos la imagen
            painter->drawPixmap(rect.toRect(), texturaPixmap);

            // Dibujar el borde por encima si existe
            if (pen().style() != Qt::NoPen) {
                painter->setBrush(Qt::NoBrush);
                painter->drawRect(rect);
            }
        }

        painter->restore();
    } else {
        // Dibujo normal si no hay textura (usa el brush y pen configurados)
        QGraphicsRectItem::paint(painter, option, widget);
    }
}
