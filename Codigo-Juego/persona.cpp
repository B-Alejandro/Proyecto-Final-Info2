#include "persona.h"
#include <QBrush>
#include <QPen>
#include <QTimer>
#include <QKeyEvent>
#include <QGraphicsScene>
#include <QDebug>

Persona::Persona(qreal w, qreal h,
                 qreal sceneWidth, qreal sceneHeight,
                 TipoMovimiento tipo)
    : QGraphicsRectItem(-w/2, -h/2, w, h),
    tipoMovimiento(tipo),
    sceneW(sceneWidth),
    sceneH(sceneHeight)
{
    setPen(QPen(Qt::NoPen));

    // Inicializar controles y velocidad
    upPressed = downPressed = leftPressed = rightPressed = false;
    speed = 5.0;

    // Variables de gravedad
    vy = 0.0;
    g = 0.5;
    onGround = false;

    setBrush(QBrush(Qt::blue));

    // Timer de actualización de la física (~60 FPS)
    timer = new QTimer(this);
    QObject::connect(timer, &QTimer::timeout, this, &Persona::updateMovement);
    timer->start(16);

    // Configurar el item para recibir eventos de teclado si es necesario, aunque la lógica esté en Jugador
    setFlag(QGraphicsItem::ItemIsFocusable);
}

void Persona::setTipoMovimiento(TipoMovimiento tipo)
{
    tipoMovimiento = tipo;

    // Reiniciar variables de movimiento si se cambia de modo
    if (tipo == TipoMovimiento::RECTILINEO) {
        vy = 0;
        onGround = false;
    } else {
        upPressed = false;
        downPressed = false;
        onGround = false;
    }
}

// **handleInput() es ahora virtual y contiene la lógica base o se deja vacío**
void Persona::handleInput()
{
    // Las subclases (Jugador, Enemigo) sobrescribirán esta función
    // para implementar la lógica de control o IA (salto, dirección, etc.).
    // La lógica de salto ha sido eliminada de aquí.
}

// **keyPressEvent y keyReleaseEvent han sido eliminados de Persona**
// Estas funciones DEBEN ser implementadas en la clase Jugador.

void Persona::updateMovement()
{
    // Llama a la lógica de control/IA de la subclase
    handleInput();

    if (tipoMovimiento == TipoMovimiento::RECTILINEO) {
        updateMovementRectilineo();
    } else {
        updateMovementConGravedad();
    }
}

void Persona::updateMovementRectilineo()
{
    double dx = 0.0, dy = 0.0;

    // Usa las variables de control establecidas por handleInput()
    if (upPressed)     dy -= speed;
    if (downPressed)   dy += speed;
    if (leftPressed)   dx -= speed;
    if (rightPressed)  dx += speed;

    double newX = x() + dx;
    double newY = y() + dy;

    double halfW = rect().width() / 2;
    double halfH = rect().height() / 2;

    // Colisiones con los límites de la escena
    if (newX < halfW) newX = halfW;
    if (newX > sceneW - halfW) newX = sceneW - halfW;
    if (newY < halfH) newY = halfH;
    if (newY > sceneH - halfH) newY = sceneH - halfH;

    setPos(newX, newY);
}

void Persona::updateMovementConGravedad()
{
    double halfW = rect().width() / 2;
    double halfH = rect().height() / 2;

    double dx = 0.0;

    // 1. Movimiento Horizontal (Input del usuario)
    if (leftPressed) dx -= speed;
    if (rightPressed) dx += speed;

    setX(x() + dx);

    // Colisiones con los límites de la escena (Horizontal)
    if (x() < halfW) setX(halfW);
    if (x() > sceneW - halfW) setX(sceneW - halfW);

    // 2. Resolver colisiones horizontales (Plataformas/Muros)
    resolveCollisions(dx, 0.0);

    // 3. Movimiento Vertical (Gravedad y Salto)
    vy += g;

    setY(y() + vy);

    // 4. Colisiones con los límites de la escena (Vertical)
    bool wasOnGround = onGround;
    if (y() < halfH) { // Techo de la escena
        setY(halfH);
        vy = 0.0;
    }

    if (y() >= sceneH - halfH) { // Suelo de la escena
        setY(sceneH - halfH);
        vy = 0.0;
        onGround = true;
    } else if (y() < sceneH - halfH && wasOnGround) {
        onGround = false;
    }

    // 5. Resolver colisiones verticales (Plataformas/Techos)
    resolveCollisions(0.0, vy);
}

void Persona::resolveCollisions(double dx, double dy)
{
    QList<QGraphicsItem*> collisions = collidingItems();

    double halfW = rect().width() / 2;
    double halfH = rect().height() / 2;

    for (QGraphicsItem* item : collisions) {
        if (item == this || item->type() != QGraphicsRectItem::Type) {
            continue;
        }

        QRectF otherRect = item->sceneBoundingRect();
        QRectF myRect = sceneBoundingRect();

        // Resolución de Colisión Horizontal
        if (dx != 0.0) {
            if (dx > 0.0 && myRect.right() > otherRect.left() && myRect.left() < otherRect.left()) {
                setX(otherRect.left() - halfW);
            }
            else if (dx < 0.0 && myRect.left() < otherRect.right() && myRect.right() > otherRect.right()) {
                setX(otherRect.right() + halfW);
            }
        }

        // Resolución de Colisión Vertical
        if (dy != 0.0) {
            if (dy > 0.0 && myRect.bottom() > otherRect.top() && myRect.top() < otherRect.top()) {
                setY(otherRect.top() - halfH);
                vy = 0.0;
                onGround = true;
            }
            else if (dy < 0.0 && myRect.top() < otherRect.bottom() && myRect.bottom() > otherRect.bottom()) {
                setY(otherRect.bottom() + halfH);
                vy = 0.0;
            }
        }
    }
}

// ============ MÉTODOS DE COLISIÓN (Información) ============

bool Persona::checkCollisionWith(Persona* other)
{
    if (!other || other == this) {
        return false;
    }

    return sceneBoundingRect().intersects(other->sceneBoundingRect());
}

bool Persona::checkCollisionBelow()
{
    for (QGraphicsItem* item : collidingItems()) {
        if (item != this && item->type() == QGraphicsRectItem::Type) {
            QRectF otherRect = item->sceneBoundingRect();
            QRectF myRect = sceneBoundingRect();
            if (myRect.bottom() <= otherRect.top() + 5 && myRect.bottom() >= otherRect.top()) {
                return true;
            }
        }
    }
    return false;
}

QList<Persona*> Persona::getCollidingPersonas()
{
    QList<Persona*> collidingPersonas;
    QList<QGraphicsItem*> collisions = collidingItems();

    for (QGraphicsItem* item : collisions) {
        Persona* persona = dynamic_cast<Persona*>(item);
        if (persona && persona != this) {
            collidingPersonas.append(persona);
        }
    }

    return collidingPersonas;
}
