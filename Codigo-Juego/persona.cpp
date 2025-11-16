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
    : QGraphicsRectItem(0, 0, w, h),
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

    // Configurar el item para recibir eventos de teclado si es necesario
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

void Persona::handleInput()
{
    // Las subclases (Jugador, Enemigo) sobrescribirán esta función
}

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

    if (upPressed)     dy -= speed;
    if (downPressed)   dy += speed;
    if (leftPressed)   dx -= speed;
    if (rightPressed)  dx += speed;

    // Si no hay movimiento, no hacer nada
    if (dx == 0 && dy == 0) return;

    double w = rect().width();
    double h = rect().height();

    // Guardar posición actual ANTES de mover
    double prevX = x();
    double prevY = y();

    // Calcular nueva posición
    double newX = x() + dx;
    double newY = y() + dy;

    // Colisiones con los límites de la escena
    // Permitir que llegue exactamente al borde (0 a sceneW-width)
    if (newX < 0) newX = 0;
    if (newX + w > sceneW) newX = sceneW - w;
    if (newY < 0) newY = 0;
    if (newY + h > sceneH) newY = sceneH - h;

    // Aplicar movimiento
    setPos(newX, newY);

    // Verificar colisiones con otros objetos (no consigo mismo)
    QList<QGraphicsItem*> collisions = collidingItems();

    for (QGraphicsItem* item : collisions) {
        // Verificar si es un rectángulo diferente a este
        if (item->type() == QGraphicsRectItem::Type) {
            // Hay colisión con otro objeto, restaurar posición
            setPos(prevX, prevY);
            return;
        }
    }
}

void Persona::updateMovementConGravedad()
{
    double w = rect().width();
    double h = rect().height();

    // ========== FASE 1: MOVIMIENTO HORIZONTAL ==========
    double dx = 0.0;
    if (leftPressed) dx -= speed;
    if (rightPressed) dx += speed;

    if (dx != 0) {
        double prevX = x();
        setX(x() + dx);

        // Límites horizontales de la escena
        if (x() < 0) {
            setX(0);
        } else if (x() + w > sceneW) {
            setX(sceneW - w);
        } else {
            // Solo verificar colisiones si no estamos en los bordes
            QList<QGraphicsItem*> collisions = collidingItems();
            for (QGraphicsItem* item : collisions) {
                if (item->type() == QGraphicsRectItem::Type) {
                    // Restaurar posición X anterior
                    setX(prevX);
                    break;
                }
            }
        }
    }

    // ========== FASE 2: MOVIMIENTO VERTICAL (GRAVEDAD) ==========
    double prevY = y();

    // Aplicar gravedad
    vy += g;
    setY(y() + vy);

    // Asumir que no está en el suelo hasta que se demuestre lo contrario
    onGround = false;

    // Límite superior (techo)
    if (y() < 0) {
        setY(0);
        vy = 0.0;
    }
    // Límite inferior (suelo de la escena)
    else if (y() + h > sceneH) {
        setY(sceneH - h);
        vy = 0.0;
        onGround = true;
    }
    else {
        // Verificar colisiones verticales con otros objetos
        QList<QGraphicsItem*> collisions = collidingItems();

        for (QGraphicsItem* item : collisions) {
            if (item->type() != QGraphicsRectItem::Type) {
                continue;
            }

            QRectF otherRect = item->sceneBoundingRect();
            QRectF myRect = sceneBoundingRect();

            // Detectar si estamos cayendo sobre otro objeto
            if (vy > 0) {
                // Cayendo hacia abajo
                if (prevY + h <= otherRect.top() + 2) {
                    // Estábamos arriba del objeto, ahora aterrizamos
                    setY(otherRect.top() - h);
                    vy = 0.0;
                    onGround = true;
                    break;
                }
            }
            else if (vy < 0) {
                // Subiendo (golpeando techo)
                if (prevY >= otherRect.bottom() - 2) {
                    // Estábamos abajo del objeto, golpeamos su parte inferior
                    setY(otherRect.bottom());
                    vy = 0.0;
                    break;
                }
            }
        }
    }
}

bool Persona::resolveCollisions(double dx, double dy)
{
    // Este método ya no se usa en la nueva implementación
    // pero lo mantenemos por compatibilidad
    return false;
}

bool Persona::resolveCollisionsHorizontal(double dx)
{
    // Este método ya no se usa en la nueva implementación
    // pero lo mantenemos por compatibilidad
    return false;
}

bool Persona::resolveCollisionsVertical(double dy)
{
    // Este método ya no se usa en la nueva implementación
    // pero lo mantenemos por compatibilidad
    return false;
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
