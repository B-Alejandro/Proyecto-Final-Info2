#include "persona.h"
#include "fisica.h"
#include <QGraphicsRectItem>
#include <QTimer>
#include <QPen>
#include <QBrush>

Persona::Persona(qreal w, qreal h,
                 qreal sceneWidth, qreal sceneHeight,
                 TipoMovimiento tipo)
    : QGraphicsRectItem(0, 0, w, h),
    tipoMovimiento(tipo),
    sceneW(sceneWidth),
    sceneH(sceneHeight)
{
    setPen(QPen(Qt::NoPen));
    setBrush(QBrush(Qt::blue));

    speed = 5.0;

    upPressed = false;
    downPressed = false;
    leftPressed = false;
    rightPressed = false;

    vy = 0.0;
    g = 0.5;
    onGround = false;

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Persona::updateMovement);
    timer->start(16);

    setFlag(QGraphicsItem::ItemIsFocusable);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges);

    setCacheMode(QGraphicsItem::DeviceCoordinateCache);
}

void Persona::handleInput()
{
    // Método virtual para que las subclases puedan sobreescribir
}

void Persona::updateMovement()
{
    handleInput();

    if (tipoMovimiento == TipoMovimiento::RECTILINEO) {
        updateMovementRectilineo();
    } else {
        updateMovementConGravedad();
    }
}

void Persona::updateMovementRectilineo()
{
    double dx = 0;
    double dy = 0;

    if (upPressed) dy -= speed + 3;
    if (downPressed) dy += speed + 3;
    if (leftPressed) dx -= speed + 3;
    if (rightPressed) dx += speed + 3;

    // Delegar todo el manejo de colisiones a Fisica
    Fisica::aplicarMovimientoRectilineo(this, dx, dy, sceneW, sceneH);
}

void Persona::updateMovementConGravedad()
{
    double dx = 0;
    if (leftPressed) dx -= speed + 6;
    if (rightPressed) dx += speed + 6;

    // Delegar todo el manejo de colisiones y gravedad a Fisica
    Fisica::aplicarMovimientoConGravedad(this, dx, vy, g, onGround, sceneW, sceneH);
}

void Persona::setTipoMovimiento(TipoMovimiento tipo)
{
    tipoMovimiento = tipo;
}
