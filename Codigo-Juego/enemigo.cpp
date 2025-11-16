#include "enemigo.h"
#include <QBrush>
#include <QRandomGenerator>

Enemigo::Enemigo(qreal w, qreal h,
                 qreal sceneWidth, qreal sceneHeight,
                 TipoMovimiento tipo)
    : Persona(w, h, sceneWidth, sceneHeight, tipo)
{
    // Color rojo para el enemigo
    setBrush(QBrush(Qt::red));

    // Velocidad más lenta que el jugador
    speed = 3;

    // Configuración de IA
    changeDirectionTime = 2000;  // Cambiar dirección cada 2 segundos
    canJump = true;

    // Timer para cambiar de dirección aleatoriamente
    aiTimer = new QTimer(this);
    connect(aiTimer, &QTimer::timeout, this, &Enemigo::changeDirection);
    aiTimer->start(changeDirectionTime);

    // Inicializar con una dirección aleatoria
    randomizeDirection();
}

void Enemigo::handleInput()
{
    // Este método se llama en cada frame
    // Aquí puedes agregar lógica adicional de IA si es necesario

    // Intentar saltar aleatoriamente si tiene gravedad
    if (tipoMovimiento == TipoMovimiento::CON_GRAVEDAD && canJump) {
        tryJump();
    }
}

void Enemigo::changeDirection()
{
    randomizeDirection();
}

void Enemigo::randomizeDirection()
{
    // Generar números aleatorios para la dirección
    QRandomGenerator *rng = QRandomGenerator::global();

    if (tipoMovimiento == TipoMovimiento::RECTILINEO) {
        // Movimiento en 8 direcciones
        // 25% de probabilidad de estar quieto
        upPressed = rng->bounded(4) == 0;
        downPressed = rng->bounded(4) == 0;
        leftPressed = rng->bounded(4) == 0;
        rightPressed = rng->bounded(4) == 0;

        // Evitar direcciones opuestas simultáneas
        if (upPressed && downPressed) {
            if (rng->bounded(2)) upPressed = false;
            else downPressed = false;
        }
        if (leftPressed && rightPressed) {
            if (rng->bounded(2)) leftPressed = false;
            else rightPressed = false;
        }
    } else {
        // Movimiento lateral con gravedad
        int direction = rng->bounded(3); // 0: izquierda, 1: derecha, 2: quieto

        leftPressed = (direction == 0);
        rightPressed = (direction == 1);
    }
}

void Enemigo::tryJump()
{
    // Saltar con 2% de probabilidad cada frame si está en el suelo
    if (onGround && QRandomGenerator::global()->bounded(100) < 2) {
        vy = -10;
        onGround = false;
    }
}
