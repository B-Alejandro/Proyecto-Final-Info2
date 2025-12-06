#ifndef COLECCIONABLE_H
#define COLECCIONABLE_H

#include <QGraphicsObject>

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

    void verificarColision();

signals:
    void coleccionableRecolectado(Coleccionable* coleccionable);

private:
    void recolectar();

    qreal tamanio;
    int tipo;
    bool recolectado;
};

#endif // COLECCIONABLE_H
