#ifndef OBSTACULO_H
#define OBSTACULO_H

#include <QGraphicsRectItem>
#include <QColor>

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

private:
    QColor currentColor;
};

#endif
