#include "enemigo.h"
#include "obstaculo.h"
#include "jugador.h"
#include "proyectil.h"
#include <QBrush>
#include <QRandomGenerator>
#include <QGraphicsScene>
#include <QTimer>
#include <QDebug>
#include <QPen> // Aseguramos que QPen esté incluido

/*
 * Constructor unificado
 *
 * nivel == 1  -> IA simple (descendente, disparo), poca vida, sin sprites (solo color o EnemigoN1.png)
 * nivel != 1  -> IA avanzada (persecucion, salto inteligente), más vida, usa sprites Soldier_...
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

    // *** Configurar sistema de disparo ***
    tiempoEntreDisparos = 3500; // Dispara cada 3 segundos
    timerDisparo = new QTimer(this);
    connect(timerDisparo, &QTimer::timeout, this, &Enemigo::intentarDisparar);

    // Iniciar disparos después de medio segundo, solo si es Nivel 1
    if (numeroNivel == 1) {
        QTimer::singleShot(300, this, [this]() {
            if (estaVivo()) {
                dispararProyectil(); // ¡Disparo inmediato!
                timerDisparo->start(tiempoEntreDisparos);
            }
        });
    }

    aiTimer = new QTimer(this);
    connect(aiTimer, &QTimer::timeout, this, &Enemigo::changeDirection);

    if (numeroNivel == 1)
    {
        // *** NIVEL 1: Configuración para movimiento descendente y disparo ***
        upPressed = false;
        leftPressed = false;
        rightPressed = false;
        downPressed = true;  // Siempre moviéndose hacia abajo

        speed = 1.8; // Velocidad de IA simple
        setVida(3); // Vida baja

        // CORRECCIÓN FLICKERING 2: FORZAR BROCHA TRANSPARENTE
        setBrush(Qt::transparent);

        // Cargar sprite específico de nivel 1 (si existe)
        cargarSpritesNivel1();

        qDebug() << "Enemigo Nivel 1 creado en pos:" << x() << y()
                 << "con speed:" << speed
                 << "vida:" << getVida()
                 << "downPressed:" << downPressed;
    }
    else
    {
        // *** NIVEL 2+: Modo inactivo hasta activar persecución, IA avanzada ***
        speed = 0; // Inactivo
        setVida(500); // Vida alta
        aiTimer->start(changeDirectionTime);
        cargarSprites(); // Cargar sprites avanzados
        qDebug() << "Enemigo creado en modo inactivo con vida:" << getVida();
    }
}

/*
 * Carga de sprites para niveles que los necesitan (nivel 2+)
 */
void Enemigo::cargarSprites()
{
    qDebug() << "Cargando sprites enemigo Nivel 2+";

    QString rutaIdle  = ":/Recursos/Sprites/Soldier_Idle.png";
    QString rutaRun   = ":/Recursos/Sprites/Run_soldier.png";
    QString rutaJump  = ":/Recursos/Sprites/Run_soldier.png";
    QString rutaDeath = ":/Recursos/Sprites/Attacck.png";

    spriteIdle     = QPixmap(rutaIdle);
    spriteCorrer   = QPixmap(rutaRun);
    spriteSaltar   = QPixmap(rutaJump);
    spriteMuerte   = QPixmap(rutaDeath);

    // Fallback por si alguna ruta falla
    if (spriteIdle.isNull()) spriteIdle = spriteCorrer;
    if (spriteSaltar.isNull()) spriteSaltar = spriteCorrer;
    if (spriteMuerte.isNull()) spriteMuerte = spriteCorrer;

    // Se mantiene la configuración de sprite de HEAD (más controlada para IDLE)
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
 * Carga de sprites para Nivel 1 (EnemigoN1.png)
 */
void Enemigo::cargarSpritesNivel1()
{
    // Ruta y configuracion para EnemigoN1.png
    QString rutaBase = ":/Recursos/Sprites/EnemigoN1.png";

    if (spriteSheet.load(rutaBase)) {
        usarSprites = true;

        // ** Configuración para EnemigoN1.png **
        totalFrames = 1;
        frameActual = 0;

        if (timerAnimacion) {
            timerAnimacion->setInterval(100);
            reanudarAnimacion();
        }

        qDebug() << "Sprites del Enemigo Nivel 1 cargados:" << rutaBase;
    } else {
        qDebug() << "ERROR: No se pudo cargar el sprite del Enemigo Nivel 1:" << rutaBase;
        usarSprites = false;
    }
}

/*
 * Lógica para cambiar el sprite basado en el estado de animación (Solo Nivel 2+)
 * Se utiliza la versión con correcciones de HEAD.
 */
void Enemigo::cambiarSpritePorEstado()
{
    if (numeroNivel == 1) return; // Nivel 1 tiene sprites estáticos o usa cargarSpritesNivel1

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
 * Activar IA de persecucion (solo nivel 2+)
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
 * IA principal de movimiento - SOBRESCRIBE handleInput de Persona
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

    // Lógica de salto inteligente: si estoy en el suelo, intento saltar si hay obstáculo adelante
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

    // Esta función se conecta al aiTimer, pero la IA de persecución (handleInput)
    // sobrescribe la dirección, por lo que esta función solo tiene efecto si la IA
    // se detiene o si se implementara movimiento aleatorio puro (randomizeDirection)
    // en lugar de persecución.

    // La lógica de persecución en handleInput es dominante.
    // Mantenemos randomizeDirection por si se necesita para una IA no-persecución.
    // randomizeDirection();
}

/*
 * IA de movimiento aleatorio (solo para nivel 2+ y si no está en persecución)
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
 * Saltar si hay obstaculo adelante (solo nivel 2+)
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
 * Deteccion de obstaculo justo adelante
 */
bool Enemigo::detectarObstaculoAdelante()
{
    if (!scene()) return false;

    // Distancia de detección (1.5 veces el ancho del enemigo)
    qreal distancia = rect().width() * 1.5;
    qreal dir = mirandoIzquierda ? -1 : 1;

    // Zona de colisión a revisar
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

// Sobrecarga de tryJump, usada solo si se quisiera un salto aleatorio
void Enemigo::tryJump()
{
    if (numeroNivel == 1) return;

    if (onGround && QRandomGenerator::global()->bounded(100) < 2)
    {
        vy = -10;
        onGround = false;
    }
}

// *** Lógica de disparo ***
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
    qreal projSpeed = 6.0;
    int dirY = 1; // *** HACIA ABAJO (Nivel 1 es descendente) ***

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
