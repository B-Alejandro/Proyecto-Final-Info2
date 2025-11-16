#ifndef PERSONA_H
#define PERSONA_H

#include <QGraphicsRectItem>
#include <QObject>
#include <QKeyEvent>

class QTimer;
class QGraphicsScene;

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

    // Getters y Setters
    void setTipoMovimiento(TipoMovimiento tipo);
    TipoMovimiento getTipoMovimiento() const { return tipoMovimiento; }
    void setSpeed(qreal newSpeed) { speed = newSpeed; }

    // Métodos de Colisión
    bool checkCollisionWith(Persona* other);
    bool checkCollisionBelow();
    QList<Persona*> getCollidingPersonas();

protected:
    // Virtual para que Enemigo/Jugador pueda sobrescribir la lógica de input/IA
    virtual void handleInput();

private slots:
    void updateMovement();

protected:
    // Variables de Movimiento
    TipoMovimiento tipoMovimiento;
    qreal speed;
    qreal sceneW, sceneH;

    // Variables de Gravedad / Salto
    qreal vy;
    qreal g;
    bool onGround;

    // Timer de actualización de la física
    QTimer *timer;

    // Controles de Input (Usados por Jugador/Enemigo para establecer el estado)
    bool upPressed, downPressed, leftPressed, rightPressed;

    // Funciones de Actualización específicas
    void updateMovementRectilineo();
    void updateMovementConGravedad();

    // Funciones de resolución de colisiones
    bool resolveCollisions(double dx, double dy);
    bool resolveCollisionsHorizontal(double dx);   // AGREGAR ESTA LÍNEA
    bool resolveCollisionsVertical(double dy);     // AGREGAR ESTA LÍNEA
};

#endif // PERSONA_H
