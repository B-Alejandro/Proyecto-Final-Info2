#ifndef COLECCIONABLE_H
#define COLECCIONABLE_H

#include <QGraphicsObject>
#include <QColor>   // Necesario para QColor
#include <QString>  // Necesario para QString (el texto/emoji)

class Coleccionable : public QGraphicsObject
{
    Q_OBJECT

public:
    explicit Coleccionable(qreal x,
                           qreal y,
                           qreal tamanio = 25.0,
                           int tipo = 0,
                           QGraphicsItem* parent = nullptr);

    QRectF boundingRect() const override;

    void paint(QPainter* painter,
               const QStyleOptionGraphicsItem* option,
               QWidget* widget) override;

    bool fueRecolectado() const { return recolectado; }
    int getTipo() const { return tipo; }

    // Método corregido para establecer el color (usado para el texto)
    void setFillColor(const QColor& color) { fillColor = color; update(); }

    void verificarColision();

signals:
    void coleccionableRecolectado(Coleccionable* coleccionable);

private:
    void recolectar();

    qreal tamanio;
    int tipo;
    bool recolectado;
    QColor fillColor;       // Color usado para el texto/emoji
    QString textTexture;    // Cadena que contiene el emoji
};

#endif // COLECCIONABLE_H
