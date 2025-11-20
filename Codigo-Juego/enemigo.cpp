#include "enemigo.h"
#include "obstaculo.h"
#include <QBrush>
#include <QRandomGenerator>
#include <QGraphicsScene>

Enemigo::Enemigo(qreal w,
                 qreal h,
                 qreal sceneWidth,
                 qreal sceneHeight,
                 TipoMovimiento tipo)
    : Persona(w, h, sceneWidth, sceneHeight, tipo)
{
    setBrush(QBrush(Qt::red));
    speed = 3;
    changeDirectionTime = 2000;
    canJump = true;
    aiTimer = new QTimer(this);
    connect(aiTimer, &QTimer::timeout, this, &Enemigo::changeDirection);
    aiTimer->start(changeDirectionTime);
    randomizeDirection();
}

void Enemigo::handleInput()
{
    if (tipoMovimiento == TipoMovimiento::CON_GRAVEDAD && canJump) {
        tryJumpIfObstacleAhead();
    }
}

void Enemigo::changeDirection()
{
    randomizeDirection();
}

void Enemigo::randomizeDirection()
{
    QRandomGenerator* rng = QRandomGenerator::global();
    if (tipoMovimiento == TipoMovimiento::RECTILINEO) {
        upPressed    = rng->bounded(4) == 0;
        downPressed  = rng->bounded(4) == 0;
        leftPressed  = rng->bounded(4) == 0;
        rightPressed = rng->bounded(4) == 0;
        if (upPressed && downPressed) {
            if (rng->bounded(2)) upPressed = false;
            else downPressed = false;
        }
        if (leftPressed && rightPressed) {
            if (rng->bounded(2)) leftPressed = false;
            else rightPressed = false;
        }
    } else {
        // CON_GRAVEDAD: no necesita dirección aleatoria
        // El salto se controla por detección de obstáculos
        leftPressed  = false;
        rightPressed = false;
    }
}

void Enemigo::tryJumpIfObstacleAhead()
{
    if (!onGround) return; // Solo saltar si está en el suelo

    // Detectar obstáculos adelante
    bool obstaculoDetectado = detectarObstaculoAdelante();

    if (obstaculoDetectado) {
        // Saltar al detectar obstáculo
        vy = -10;
        onGround = false;
    }
}

bool Enemigo::detectarObstaculoAdelante()
{
    if (!scene()) return false;

    // Distancia de detección MÁS CORTA para saltar más cerca del obstáculo
    qreal distanciaDeteccion = rect().width() * 1.5; // 1.5 veces el ancho del enemigo (antes era 2.5)

    // Crear un rectángulo de detección adelante del enemigo
    QRectF zonaDeteccion(
        x() + rect().width(),           // Desde el borde derecho del enemigo
        y(),                             // Misma altura Y
        distanciaDeteccion,              // Ancho de la zona de detección (más corto)
        rect().height()                  // Misma altura que el enemigo
        );

    // Obtener todos los items en la zona de detección
    QList<QGraphicsItem*> itemsAdelante = scene()->items(zonaDeteccion, Qt::IntersectsItemShape);

    // Verificar si hay obstáculos en la zona
    for (QGraphicsItem* item : itemsAdelante) {
        if (item == this) continue; // Ignorarse a sí mismo

        // Verificar si es un obstáculo
        Obstaculo* obs = dynamic_cast<Obstaculo*>(item);
        if (obs) {
            // Verificar que el obstáculo está realmente adelante (no atrás)
            if (obs->x() > x()) {
                return true; // Obstáculo detectado adelante
            }
        }
    }

    return false; // No hay obstáculos adelante
}

void Enemigo::tryJump()
{
    // Método anterior (ya no usado con CON_GRAVEDAD)
    if (onGround && QRandomGenerator::global()->bounded(100) < 2) {
        vy = -10;
        onGround = false;
    }
}
