#ifndef PERSONA_H
#define PERSONA_H

#include <QObject>
#include <QGraphicsRectItem>
#include <QKeyEvent>

class QTimer;

enum class TipoMovimiento {
    RECTILINEO,
    CON_GRAVEDAD
};

/*
Clase Persona
Representa un objeto rectangular que puede moverse con o sin gravedad.
Toda la logica de colision esta delegada a la clase Fisica.
*/

class Persona : public QObject, public QGraphicsRectItem
{
    Q_OBJECT

public:
    Persona(qreal w,
            qreal h,
            qreal sceneWidth,
            qreal sceneHeight,
            TipoMovimiento tipo);

    void setTipoMovimiento(TipoMovimiento tipo);
    TipoMovimiento getTipoMovimiento() const { return tipoMovimiento; }

    void setSpeed(qreal newSpeed) { speed = newSpeed; }

protected:
    virtual void handleInput();

    // Variables ahora protected para que las clases hijas puedan acceder
    TipoMovimiento tipoMovimiento;
    qreal sceneW;
    qreal sceneH;
    qreal speed;

    qreal vy;
    qreal g;
    bool onGround;

    bool upPressed;
    bool downPressed;
    bool leftPressed;
    bool rightPressed;

private slots:
    void updateMovement();

private:
    void updateMovementRectilineo();
    void updateMovementConGravedad();

private:
    QTimer* timer;
};

#endif
