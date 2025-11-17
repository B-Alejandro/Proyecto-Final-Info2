#ifndef PERSONA_H
#define PERSONA_H

#include <QGraphicsRectItem>
#include <QObject>
#include <QKeyEvent>

class QTimer;

enum class TipoMovimiento {
    RECTILINEO,
    CON_GRAVEDAD
};

class Persona : public QObject, public QGraphicsRectItem
{
    Q_OBJECT

public:
    Persona(qreal w, qreal h,
            qreal sceneWidth, qreal sceneHeight,
            TipoMovimiento tipo);

    void setTipoMovimiento(TipoMovimiento tipo);
    TipoMovimiento getTipoMovimiento() const { return tipoMovimiento; }

    void setSpeed(qreal newSpeed) { speed = newSpeed; }

    bool checkCollisionWith(Persona* other);
    bool checkCollisionBelow();
    QList<Persona*> getCollidingPersonas();

protected:
    virtual void handleInput();

private slots:
    void updateMovement();

protected:
    void updateMovementRectilineo();
    void updateMovementConGravedad();

    TipoMovimiento tipoMovimiento;
    qreal sceneW;
    qreal sceneH;
    qreal speed;

    qreal vy;
    qreal g;
    bool onGround;

    QTimer* timer;

    bool upPressed;
    bool downPressed;
    bool leftPressed;
    bool rightPressed;
};

#endif
