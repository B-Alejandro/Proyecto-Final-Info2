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
                     QColor color,
                     bool esMovil)
    : QGraphicsRectItem(0, 0, w, h),
    currentColor(color),
    tieneTextura(false),
    repetirTextura(true),
    esMovil(esMovil),
    vidaActual(2),
    vidaMaxima(2),
    velocidad(3.0),
    danioAlJugador(20)
{
    setPos(x, y);
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
    update();
}

void Obstaculo::limpiarTextura()
{
    tieneTextura = false;
    texturaPixmap = QPixmap();
    setBrush(QBrush(currentColor));
    update();
}

void Obstaculo::recibirDanio(int cantidad)
{
    if (!estaVivo()) return;

    vidaActual -= cantidad;
    if (vidaActual < 0) vidaActual = 0;

    qDebug() << "Obstáculo recibió daño:" << cantidad << "Vida restante:" << vidaActual;

    if (vidaActual <= 0) {
        qDebug() << "Obstáculo destruido";
        emit obstaculoMuerto(this);
    }
}

void Obstaculo::actualizar(qreal sceneH)
{
    if (!esMovil) return;

    // Mover hacia abajo
    setY(y() + velocidad);

    // Si sale de la pantalla, será eliminado por el nivel
}

void Obstaculo::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    if (tieneTextura && !texturaPixmap.isNull()) {
        painter->save();

        QRectF rect = this->rect();

        if (repetirTextura) {
            QBrush texturaBrush(texturaPixmap);
            painter->setBrush(texturaBrush);
            painter->setPen(pen());
            painter->drawRect(rect);
        } else {
            painter->drawPixmap(rect.toRect(), texturaPixmap);

            if (pen().style() != Qt::NoPen) {
                painter->setPen(pen());
                painter->setBrush(Qt::NoBrush);
                painter->drawRect(rect);
            }
        }

        painter->restore();
    } else {
        QGraphicsRectItem::paint(painter, option, widget);
    }
}
