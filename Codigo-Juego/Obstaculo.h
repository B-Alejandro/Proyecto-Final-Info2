// ============ obstaculo.h ============
#ifndef OBSTACULO_H
#define OBSTACULO_H

#include <QGraphicsRectItem>
#include <QColor>
#include <QPixmap>
#include <QPainter>

// Identificador para clasificar el tipo de objeto fisico
enum class TipoFisico {
    PERSONA,
    PROYECTIL,
    OBSTACULO
};

class Obstaculo : public QGraphicsRectItem
{
public:
    Obstaculo(qreal x,
              qreal y,
              qreal w,
              qreal h,
              QColor color);

    void setColor(const QColor& color);
    void setBorderColor(const QColor& color, int width);

    // Metodo para obtener el tipo fisico
    TipoFisico getTipoFisico() const { return TipoFisico::OBSTACULO; }

    // NUEVO: Métodos para agregar textura/imagen
    void setTextura(const QString& rutaImagen, bool repetir = true);
    void setTextura(const QPixmap& pixmap, bool repetir = true);
    void limpiarTextura(); // Volver a usar solo color

protected:
    // NUEVO: Sobrescribir paint para dibujar la textura
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:
    QColor currentColor;
    bool tieneTextura;
    QPixmap texturaPixmap;
    bool repetirTextura;
};

#endif // OBSTACULO_H
