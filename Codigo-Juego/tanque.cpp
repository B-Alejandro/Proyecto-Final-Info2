#include "tanque.h"
#include "proyectiltanque.h"
#include <QBrush>
#include <QGraphicsScene>
#include <QDebug>

Tanque::Tanque(qreal w,
               qreal h,
               qreal sceneWidth,
               qreal sceneHeight,
               qreal posicionX)
    : Persona(w, h, sceneWidth, sceneHeight, TipoMovimiento::RECTILINEO)
    , tiempoEntreDisparos(3000)  // Dispara cada 3 segundos
    , velocidadDescenso(3.0)      // Velocidad de caída más lenta que enemigos
{
    // Configurar vida especial para tanques
    setVida(5);

    // Configurar apariencia (color distintivo)
    setBrush(QBrush(QColor(139, 69, 19))); // Marrón oscuro para tanque

    // Determinar en qué lado está el tanque
    ladoIzquierdo = (posicionX < 0);

    // Configurar movimiento descendente
    downPressed = true;
    upPressed = false;
    leftPressed = false;
    rightPressed = false;
    speed = velocidadDescenso;

    // Configurar timer de disparo
    timerDisparo = new QTimer(this);
    connect(timerDisparo, &QTimer::timeout, this, &Tanque::disparar);

    // Iniciar disparos después de 1 segundo (para que aparezca primero)
    QTimer::singleShot(1000, this, [this]() {
        if (estaVivo()) {
            timerDisparo->start(tiempoEntreDisparos);
        }
    });

    qDebug() << "Tanque creado en X:" << posicionX
             << "Lado izquierdo:" << ladoIzquierdo
             << "Vida:" << getVida();
}

Tanque::~Tanque()
{
    if (timerDisparo) {
        timerDisparo->stop();
    }
}

void Tanque::handleInput()
{
    // Asegurar que el tanque siempre se mueve hacia abajo
    downPressed = true;
    upPressed = false;
    leftPressed = false;
    rightPressed = false;

    // Debug periódico
    static int debugCounter = 0;
    if (debugCounter++ % 120 == 0) {
        qDebug() << "Tanque en Y:" << y() << "Vida:" << getVida();
    }
}

void Tanque::disparar()
{
    if (!scene() || !estaVivo()) {
        if (timerDisparo) {
            timerDisparo->stop();
        }
        return;
    }

    qDebug() << "Tanque disparando desde posición:" << x() << y();

    // Crear proyectil parabólico
    qreal anchoProyectil = 15;
    qreal altoProyectil = 15;

    // Calcular posición inicial del proyectil (frente del tanque)
    QRectF rectTanque = boundingRect();
    QPointF posicionInicial = scenePos() + QPointF(
                                  rectTanque.width() / 2 - anchoProyectil / 2,
                                  rectTanque.height() / 2
                                  );

    // Crear el proyectil
    ProyectilTanque* proyectil = new ProyectilTanque(
        anchoProyectil,
        altoProyectil,
        sceneW,
        sceneH,
        ladoIzquierdo
        );

    proyectil->setOwner(this);
    proyectil->setPos(posicionInicial);

    scene()->addItem(proyectil);

    qDebug() << "Proyectil de tanque creado en:" << posicionInicial;
}
