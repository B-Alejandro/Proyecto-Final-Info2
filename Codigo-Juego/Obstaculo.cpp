#include "obstaculo.h"
#include <QBrush>
#include <QPen>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QWidget>
#include <QDebug>

/*
 Constructor general
 Parametros:
 x, y      posicion inicial
 w, h      dimensiones
 color     color base del obstaculo
 esMovil   indica si el obstaculo se mueve hacia abajo
*/
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

/*
 Cambia el color del obstaculo
*/
void Obstaculo::setColor(const QColor& color)
{
    currentColor = color;
    setBrush(QBrush(color));
    tieneTextura = false;
}

/*
 Configura color del borde
*/
void Obstaculo::setBorderColor(const QColor& color, int width)
{
    QPen p(color);
    p.setWidth(width);
    setPen(p);
}

/*
 Activa textura a partir de archivo
*/
void Obstaculo::setTextura(const QString& rutaImagen, bool repetir)
{
    QPixmap pixmap(rutaImagen);

    if (pixmap.isNull()) {
        qDebug() << "No se pudo cargar la textura:" << rutaImagen;
        tieneTextura = false;
        return;
    }

    setTextura(pixmap, repetir);
}

/*
 Activa textura a partir de un pixmap
*/
void Obstaculo::setTextura(const QPixmap& pixmap, bool repetir)
{
    texturaPixmap = pixmap;
    repetirTextura = repetir;
    tieneTextura = true;

    setBrush(Qt::NoBrush);
    update();
}

/*
 Elimina la textura y restaura el color base
*/
void Obstaculo::limpiarTextura()
{
    tieneTextura = false;
    texturaPixmap = QPixmap();
    setBrush(QBrush(currentColor));
    update();
}

/*
 Recibe dano, reduce vida y emite senal si muere
*/
void Obstaculo::recibirDanio(int cantidad)
{
    if (!estaVivo()) return;

    vidaActual -= cantidad;
    if (vidaActual < 0) vidaActual = 0;

    qDebug() << "Obstaculo recibio danio:" << cantidad << "Vida restante:" << vidaActual;

    if (vidaActual <= 0) {
        qDebug() << "Obstaculo destruido";
        emit obstaculoMuerto(this);
    }
}

/*
 Actualiza movimiento si es un obstaculo movil
*/
void Obstaculo::actualizar(qreal sceneH)
{
    Q_UNUSED(sceneH);

    if (!esMovil) return;

    setY(y() + velocidad);
}

/*
 Dibujo del obstaculo, con o sin textura
*/
void Obstaculo::paint(QPainter *painter,
                      const QStyleOptionGraphicsItem *option,
                      QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    QRectF rect = this->rect();

    if (tieneTextura && !texturaPixmap.isNull()) {
        painter->save();

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
