#include "enemigo.h"
#include "obstaculo.h"
#include "jugador.h"
#include "proyectil.h"
#include <QBrush>
#include <QRandomGenerator>
#include <QGraphicsScene>
#include <QTimer>
#include <QDebug>

/*
 Constructor unificado

 nivel == 1  -> IA simple (descendente, disparo), poca vida
 nivel != 1  -> IA avanzada (persecucion, salto inteligente), más vida
*/
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
    // CORRECCIÓN FLICKERING 1: Se eliminó setBrush(QBrush(Qt::red));
    setPen(QPen(Qt::NoPen)); // Asegura que no haya borde.

    changeDirectionTime = 2000;
    canJump = true;

    // *** NUEVO: Configurar sistema de disparo (desde la rama de merge) ***
    tiempoEntreDisparos = 2000; // Dispara cada 2 segundos
    timerDisparo = new QTimer(this);
    connect(timerDisparo, &QTimer::timeout, this, &Enemigo::intentarDisparar);

    // Iniciar disparos después de medio segundo, solo si es Nivel 1
    QTimer::singleShot(500, this, [this]() {
        if (estaVivo() && numeroNivel == 1) {
            timerDisparo->start(tiempoEntreDisparos);
        }
    });

    aiTimer = new QTimer(this);
    connect(aiTimer, &QTimer::timeout, this, &Enemigo::changeDirection);

    if (numeroNivel == 1)
    {
        // *** NIVEL 1: Configuración para movimiento descendente ***
        upPressed = false;
        leftPressed = false;
        rightPressed = false;
        downPressed = true;  // Siempre moviéndose hacia abajo

        speed = 5.0; // Velocidad ligeramente menor (de HEAD)

        // *** CRÍTICO: Configurar vida baja para nivel 1 ***
        setVida(3);

        qDebug() << "Enemigo Nivel 1 creado en pos:" << x() << y()
                 << "con speed:" << speed
                 << "vida:" << getVida()
                 << "downPressed:" << downPressed;
    }
    else
    {
        // *** NIVEL 2+: Modo inactivo hasta activar persecución ***
        speed = 0;
        setVida(500);
        aiTimer->start(changeDirectionTime);
        qDebug() << "Enemigo creado en modo inactivo con vida:" << getVida();
    }
}

/*
 Carga de sprites para niveles que los necesitan (nivel 2)
*/
void Enemigo::cargarSprites()
{
    qDebug() << "Cargando sprites enemigo";

    QString rutaIdle  = ":/Recursos/Sprites/Soldier_Idle.png";
    QString rutaRun   = ":/Recursos/Sprites/Run_soldier.png";
    QString rutaJump  = ":/Recursos/Sprites/Run_soldier.png";
    QString rutaDeath = ":/Recursos/Sprites/Attacck.png";

    spriteIdle     = QPixmap(rutaIdle);
    spriteCorrer = QPixmap(rutaRun);
    spriteSaltar = QPixmap(rutaJump);
    spriteMuerte = QPixmap(rutaDeath);

    if (spriteIdle.isNull()) spriteIdle = spriteCorrer;
    if (spriteSaltar.isNull()) spriteSaltar = spriteCorrer;
    if (spriteMuerte.isNull()) spriteMuerte = spriteCorrer;

    // *** Se mantiene la configuración de sprite de HEAD (más controlada para IDLE) ***
    setSprite(rutaIdle, 128, 128, 1);
    estadoActual = EstadoAnimacion::IDLE;
    frameActual = 0;

    // Forzar que spriteSheet apunte al correcto
    spriteSheet = spriteIdle;
    totalFrames = 1;

    // CORRECCIÓN FLICKERING 2: FORZAR BROCHA TRANSPARENTE
    setBrush(Qt::transparent);
}

/*
 Lógica para cambiar el sprite basado en el estado de animación
 Se utiliza la versión con correcciones de HEAD.
*/
void Enemigo::cambiarSpritePorEstado()
{
    EstadoAnimacion estado = getEstadoAnimacion();

    // Determinar el sprite y frames deseados
    QPixmap* nuevoSprite = &spriteIdle;
    int nuevosFrames = 1;

    switch (estado)
    {
    case EstadoAnimacion::MUERTO:
        nuevoSprite = &spriteMuerte;
        nuevosFrames = 8;
        break;
    case EstadoAnimacion::SALTANDO:
        nuevoSprite = &spriteSaltar;
        nuevosFrames = 8;
        break;
    case EstadoAnimacion::CORRIENDO:
        nuevoSprite = &spriteCorrer;
        nuevosFrames = 8;
        break;
    case EstadoAnimacion::IDLE:
    default:
        nuevoSprite = &spriteIdle;
        nuevosFrames = 1;
        break;
    }

    // *** CORRECCIÓN CRÍTICA: Evitar repintar si sprite y frames son iguales ***
    if (spriteSheet.cacheKey() == nuevoSprite->cacheKey() && totalFrames == nuevosFrames)
        return;

    // Actualizar sprite y frames
    spriteSheet = *nuevoSprite;
    totalFrames = nuevosFrames;
    frameActual = 0; // Reset frame solo cuando cambia el sprite

    // CORRECCIÓN FINAL FLICKERING: Solo llamar a update(), sin prepareGeometryChange().
    update(boundingRect());
}


void Enemigo::onEstadoAnimacionCambiado()
{
    cambiarSpritePorEstado();
}

void Enemigo::activarAnimacionMuerte()
{
    setAnimacion(EstadoAnimacion::MUERTO);

    QTimer::singleShot(800, this, [this]()
                       {
                           pausarAnimacion();
                       });
}

/*
 Activar IA de persecucion (solo nivel 2+)
*/
void Enemigo::activarPersecucion()
{
    if (numeroNivel == 1) return;

    if (!enemigoActivo)
    {
        enemigoActivo = true;
        // Se mantiene la velocidad ajustada de HEAD, que es más baja
        speed = 4.46;

        if (onGround)
            vy = 0;

        if (!aiTimer->isActive())
            aiTimer->start(changeDirectionTime);
    }
}

/*
 IA principal de movimiento - SOBRESCRIBE handleInput de Persona
*/
void Enemigo::handleInput()
{
    if (numeroNivel == 1)
    {
        // *** NIVEL 1: Asegurar que siempre está bajando ***
        downPressed = true;
        upPressed = false;
        leftPressed = false;
        rightPressed = false;

        // Debug cada cierto tiempo para verificar
        static int debugCounter = 0;
        if (debugCounter++ % 60 == 0) {
            qDebug() << "Enemigo en:" << x() << y() << "speed:" << speed;
        }
        return;
    }

    // *** NIVEL 2+: IA de persecución ***
    if (!enemigoActivo) return;
    if (!scene()) return;

    QGraphicsItem* objetivo = nullptr;
    for (QGraphicsItem* item : scene()->items())
    {
        Jugador* j = dynamic_cast<Jugador*>(item);
        if (j)
        {
            objetivo = j;
            break;
        }
    }

    if (!objetivo) return;

    qreal dx = objetivo->x() - x();

    if (dx > 0)
    {
        rightPressed = true;
        leftPressed = false;
        mirandoIzquierda = false;
    }
    else
    {
        rightPressed = false;
        leftPressed = true;
        mirandoIzquierda = true;
    }

    if (canJump && onGround)
        tryJumpIfObstacleAhead();
}

void Enemigo::changeDirection()
{
    if (numeroNivel == 1)
    {
        // En nivel 1, los enemigos no cambian de dirección
        return;
    }

    randomizeDirection();
}

/*
 IA de movimiento aleatorio (solo para nivel 2+)
*/
void Enemigo::randomizeDirection()
{
    if (numeroNivel == 1)
        return;

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
        leftPressed = false;
        rightPressed = false;
    }
}

/*
 Saltar si hay obstaculo adelante (solo nivel 2+)
*/
void Enemigo::tryJumpIfObstacleAhead()
{
    if (numeroNivel == 1) return;
    if (!onGround) return;

    if (detectarObstaculoAdelante())
    {
        vy = -10;
        onGround = false;
    }
}

/*
 Deteccion de obstaculo justo adelante
*/
bool Enemigo::detectarObstaculoAdelante()
{
    if (!scene()) return false;

    qreal distancia = rect().width() * 1.5;
    qreal dir = mirandoIzquierda ? -1 : 1;

    QRectF zona = QRectF(
                      x() + dir * rect().width(),
                      y(),
                      distancia * dir,
                      rect().height()
                      ).normalized();

    QList<QGraphicsItem*> items = scene()->items(zona, Qt::IntersectsItemShape);

    for (QGraphicsItem* item : items)
    {
        if (item == this) continue;
        if (dynamic_cast<Obstaculo*>(item))
            return true;
    }
    return false;
}

void Enemigo::tryJump()
{
    if (numeroNivel == 1) return;

    if (onGround && QRandomGenerator::global()->bounded(100) < 2)
    {
        vy = -10;
        onGround = false;
    }
}

// *** Lógica de disparo (desde la rama de merge) ***
void Enemigo::intentarDisparar()
{
    if (!scene() || !estaVivo()) {
        if (timerDisparo) {
            timerDisparo->stop();
        }
        return;
    }

    // Solo disparar en nivel 1
    if (numeroNivel == 1) {
        dispararProyectil();
    }
}

void Enemigo::dispararProyectil()
{
    if (!scene()) return;

    qreal projW = 10;
    qreal projH = 16;
    qreal projSpeed = 8.0;
    int dirY = 1; // *** HACIA ABAJO ***

    Proyectil* bala = new Proyectil(projW, projH, projSpeed, dirY);
    bala->setOwner(this);

    // Spawn en la parte inferior del enemigo
    QRectF br = boundingRect();
    QPointF spawn = scenePos() + QPointF(br.width() / 2 - projW / 2, br.bottom());

    bala->setPos(spawn);
    scene()->addItem(bala);

    // Debug cada cierto tiempo
    static int disparoCounter = 0;
    if (disparoCounter++ % 5 == 0) {
        qDebug() << "Enemigo disparó en:" << spawn;
    }
}
