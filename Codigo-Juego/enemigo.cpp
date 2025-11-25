// enemigo.cpp - CORREGIDO
#include "enemigo.h"
#include "obstaculo.h"
#include <QBrush>
#include <QRandomGenerator>
#include <QGraphicsScene>
#include <QDebug>
#include <QTimer>
#include "jugador.h"
Enemigo::Enemigo(qreal w,
                 qreal h,
                 qreal sceneWidth,
                 qreal sceneHeight,
                 TipoMovimiento tipo,
                 int nivel)
    : Persona(w, h, sceneWidth, sceneHeight, tipo)
    , numeroNivel(nivel)
    , enemigoActivo(false)
{
    setBrush(QBrush(Qt::red));

    // Sin velocidad inicial
    speed = 0;

    changeDirectionTime = 2000;
    canJump = true;

    // Timer de IA (por si se necesita en el futuro)
    aiTimer = new QTimer(this);
    connect(aiTimer, &QTimer::timeout, this, &Enemigo::changeDirection);
    // NO iniciar el timer hasta que se active

    qDebug() << "Enemigo creado en modo inactivo";
}

void Enemigo::cargarSprites()
{
    qDebug() << "=== CARGANDO SPRITES DEL ENEMIGO ===";

    QString rutaIdle  = ":/Recursos/Sprites/Soldier_Idle.png";
    QString rutaRun   = ":/Recursos/Sprites/Run_soldier.png";
    QString rutaJump  = ":/Recursos/Sprites/Run_soldier.png";  // Usar run como jump
    QString rutaDeath = ":/Recursos/Sprites/Attacck.png";

    spriteIdle   = QPixmap(rutaIdle);
    spriteCorrer = QPixmap(rutaRun);
    spriteSaltar = QPixmap(rutaJump);
    spriteMuerte = QPixmap(rutaDeath);

    // Validar carga de sprites
    if (spriteIdle.isNull()) {
        qDebug() << " ERROR: Sprite Idle no cargado";
        spriteIdle = spriteCorrer;
    } else {
        qDebug() << " Sprite IDLE cargado:" << spriteIdle.width() << "x" << spriteIdle.height();
    }

    if (spriteCorrer.isNull()) {
        qDebug() << " ERROR: Sprite Correr no cargado";
        return;
    } else {
        qDebug() << " Sprite CORRER cargado:" << spriteCorrer.width() << "x" << spriteCorrer.height();
    }

    if (spriteSaltar.isNull()) {
        spriteSaltar = spriteCorrer;
        qDebug() << " Sprite SALTAR usando fallback";
    } else {
        qDebug() << " Sprite SALTAR cargado";
    }

    if (spriteMuerte.isNull()) {
        spriteMuerte = spriteCorrer;
        qDebug() << " Sprite MUERTE usando fallback";
    } else {
        qDebug() << " Sprite MUERTE cargado";
    }

    // Configurar sprite inicial (IDLE)
    setSprite(rutaIdle, 128, 128, 8);
    estadoActual = EstadoAnimacion::IDLE;
    frameActual = 0;

    qDebug() << "=== SPRITES DEL ENEMIGO CARGADOS ===\n";
}

void Enemigo::cambiarSpritePorEstado()
{
    EstadoAnimacion estado = getEstadoAnimacion();

    if (estado == EstadoAnimacion::MUERTO && !spriteMuerte.isNull()) {
        if (spriteSheet.cacheKey() != spriteMuerte.cacheKey()) {
            spriteSheet = spriteMuerte;
            frameActual = 0;
            totalFrames = 8;
            qDebug() << "Sprite MUERTE activado";
        }
    }
    else if (estado == EstadoAnimacion::SALTANDO && !spriteSaltar.isNull()) {
        if (spriteSheet.cacheKey() != spriteSaltar.cacheKey()) {
            spriteSheet = spriteSaltar;
            frameActual = 0;
            totalFrames = 8;
            qDebug() << "Sprite SALTAR activado";
        }
    }
    else if (estado == EstadoAnimacion::CORRIENDO && !spriteCorrer.isNull()) {
        if (spriteSheet.cacheKey() != spriteCorrer.cacheKey()) {
            spriteSheet = spriteCorrer;
            frameActual = 0;
            totalFrames = 8;
            qDebug() << "Sprite CORRER activado";
        }
    }
    else if (estado == EstadoAnimacion::IDLE && !spriteIdle.isNull()) {
        if (spriteSheet.cacheKey() != spriteIdle.cacheKey()) {
            spriteSheet = spriteIdle;
            frameActual = 0;
            totalFrames = 8;
            qDebug() << "Sprite IDLE activado";
        }
    }
}

void Enemigo::onEstadoAnimacionCambiado()
{
    cambiarSpritePorEstado();
}

void Enemigo::activarAnimacionMuerte()
{
    qDebug() << "Activando animacion de muerte del enemigo";
    setAnimacion(EstadoAnimacion::MUERTO);

    QTimer::singleShot(800, this, [this]() {
        pausarAnimacion();
        qDebug() << "Animacion de muerte del enemigo completada";
    });
}

void Enemigo::activarPersecucion()
{
    if (!enemigoActivo) {
        qDebug() << "Activando persecución del enemigo";
        enemigoActivo = true;
        speed = 6;  // Velocidad aumentada de 4 a 6

        // Iniciar timer de IA si se necesita
        if (aiTimer && !aiTimer->isActive()) {
            aiTimer->start(changeDirectionTime);
        }

        // Asegurar que está en el suelo al activarse
        if (onGround) {
            qDebug() << "Enemigo en el suelo, gravedad = 0";
            vy = 0;
        }

        qDebug() << "Enemigo activado - Speed:" << speed << "onGround:" << onGround;
    }
}
void Enemigo::handleInput()
{
    // gravedad siempre
    if (!enemigoActivo) return;

    // obtener jugador
    if (!scene()) return;

    QList<QGraphicsItem*> lista = scene()->items();
    QGraphicsItem* objetivo = nullptr;

    for (QGraphicsItem* item : lista) {
        Jugador* j = dynamic_cast<Jugador*>(item);
        if (j) {
            objetivo = j;
            break;
        }
    }

    if (!objetivo) return;

    // perseguir al jugador SIEMPRE
    qreal dx = objetivo->x() - x();

    if (dx > 0) {
        rightPressed = true;
        leftPressed  = false;
        mirandoIzquierda = false;
    } else {
        rightPressed = false;
        leftPressed  = true;
        mirandoIzquierda = true;
    }

    // saltar si hay obstaculo delante
    if (enemigoActivo &&
        tipoMovimiento == TipoMovimiento::CON_GRAVEDAD &&
        canJump &&
        onGround)
    {
        tryJumpIfObstacleAhead();
    }
}

void Enemigo::changeDirection()
{
    if (enemigoActivo) {
        randomizeDirection();
    }
}

void Enemigo::randomizeDirection()
{
    // Para nivel 2, la dirección la maneja nivel2.cpp
    // Esta función puede quedar vacía o hacer ajustes menores

    if (numeroNivel == 1) {
        upPressed = false;
        leftPressed = false;
        rightPressed = false;
        downPressed = true;
        speed = 0.0001;
        return;
    }
}

void Enemigo::tryJumpIfObstacleAhead()
{
    if (!onGround) return;

    if (detectarObstaculoAdelante()) {
        vy = -10;
        onGround = false;
        qDebug() << "Enemigo saltando sobre obstáculo";
    }
}

bool Enemigo::detectarObstaculoAdelante()
{
    if (!scene()) return false;

    qreal distancia = rect().width() * 1.5;
    qreal direccion = mirandoIzquierda ? -1 : 1;

    QRectF zona;
    if (direccion > 0) {
        zona = QRectF(
            x() + rect().width(),
            y(),
            distancia,
            rect().height()
            );
    } else {
        zona = QRectF(
            x() - distancia,
            y(),
            distancia,
            rect().height()
            );
    }

    QList<QGraphicsItem*> objs = scene()->items(zona, Qt::IntersectsItemShape);

    for (QGraphicsItem* item : objs) {
        if (item == this) continue;
        Obstaculo* obs = dynamic_cast<Obstaculo*>(item);
        if (obs) {
            return true;
        }
    }
    return false;
}

void Enemigo::tryJump()
{
    if (onGround && QRandomGenerator::global()->bounded(100) < 2) {
        vy = -10;
        onGround = false;
    }
}
