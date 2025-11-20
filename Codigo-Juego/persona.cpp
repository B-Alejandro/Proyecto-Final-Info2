// ============ persona.cpp ============
#include "persona.h"
#include "fisica.h"
#include <QPainter>
#include <QDebug>

Persona::Persona(qreal w, qreal h, qreal sceneWidth, qreal sceneHeight, TipoMovimiento tipo)
    : QGraphicsRectItem(0, 0, w, h),
    tipoMovimiento(tipo),
    sceneW(sceneWidth),
    sceneH(sceneHeight),
    usarSprites(false),
    estadoActual(EstadoAnimacion::IDLE),
    frameActual(0),
    anchoSprite(0),
    altoSprite(0),
    totalFrames(0),
    animacionPausada(false)  // *** NUEVO ***
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

    timerAnimacion = new QTimer(this);
    connect(timerAnimacion, &QTimer::timeout, this, &Persona::actualizarAnimacion);
    timerAnimacion->start(60); // 10 fps para animación

    setFlag(QGraphicsItem::ItemIsFocusable);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges);
    setCacheMode(QGraphicsItem::DeviceCoordinateCache);
}

void Persona::setSprite(const QString& rutaImagen, int anchoFrame, int altoFrame, int numFrames)
{
    spriteSheet = QPixmap(rutaImagen);

    if (spriteSheet.isNull()) {
        qDebug() << "Error: No se pudo cargar el sprite:" << rutaImagen;
        usarSprites = false;
        return;
    }

    anchoSprite = anchoFrame;
    altoSprite = altoFrame;
    totalFrames = numFrames;
    frameActual = 0;
    usarSprites = true;

    qDebug() << "Sprite cargado:" << rutaImagen << "Frames:" << numFrames;
}

void Persona::setAnimacion(EstadoAnimacion estado)
{
    if (estadoActual != estado) {
        estadoActual = estado;
        frameActual = 0; // Reiniciar animación

        // Notificar a las subclases del cambio
        onEstadoAnimacionCambiado();
    }
}

// Método virtual para que las subclases reaccionen
void Persona::onEstadoAnimacionCambiado()
{
    // Implementación por defecto vacía
    // Las subclases pueden sobrescribir este método
}

// *** NUEVO: Pausar animación ***
void Persona::pausarAnimacion()
{
    animacionPausada = true;
    if (timerAnimacion) {
        timerAnimacion->stop();
    }
    if (timer) {
        timer->stop();  // Detener también el movimiento
    }
    qDebug() << "Animación pausada";
}

// *** NUEVO: Reanudar animación ***
void Persona::reanudarAnimacion()
{
    animacionPausada = false;
    if (timerAnimacion) {
        timerAnimacion->start(60);
    }
    if (timer) {
        timer->start(16);
    }
    qDebug() << "Animación reanudada";
}

void Persona::actualizarAnimacion()
{
    if (!usarSprites || animacionPausada) return;  // *** MODIFICADO ***

    frameActual++;
    if (frameActual >= totalFrames) {
        frameActual = 0;
    }

    update(); // Redibuja el item
}

void Persona::handleInput()
{
    // Método virtual para que las subclases puedan sobreescribir
}

void Persona::updateMovement()
{
    if (animacionPausada) return;  // *** NUEVO ***

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

    // Actualizar animación según movimiento
    if (dx != 0 || dy != 0) {
        setAnimacion(EstadoAnimacion::CORRIENDO);
    } else {
        setAnimacion(EstadoAnimacion::IDLE);
    }

    // Delegar todo el manejo de colisiones a Fisica
    Fisica::aplicarMovimientoRectilineo(this, dx, dy, sceneW, sceneH);
}

void Persona::updateMovementConGravedad()
{
    double dx = 0;

    // *** SIN MOVIMIENTO HORIZONTAL ***
    // El jugador solo salta verticalmente
    // if (leftPressed) dx -= speed + 6;
    // if (rightPressed) dx += speed + 6;

    // Actualizar animación según estado
    EstadoAnimacion nuevoEstado;

    // *** MODIFICADO: No cambiar estado si está muerto ***
    if (estadoActual == EstadoAnimacion::MUERTO) {
        nuevoEstado = EstadoAnimacion::MUERTO;
    } else if (!onGround) {
        nuevoEstado = EstadoAnimacion::SALTANDO;
    } else {
        // Siempre corriendo cuando está en el suelo
        nuevoEstado = EstadoAnimacion::CORRIENDO;
    }

    setAnimacion(nuevoEstado);

    // Delegar todo el manejo de colisiones y gravedad a Fisica
    Fisica::aplicarMovimientoConGravedad(this, dx, vy, g, onGround, sceneW, sceneH);
}

void Persona::setTipoMovimiento(TipoMovimiento tipo)
{
    tipoMovimiento = tipo;
}

void Persona::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    if (usarSprites && !spriteSheet.isNull()) {

        /*
          Se obtiene el cuadro del spritesheet.
          frameActual indica cual cuadro se usa.
        */
        int srcX = frameActual * anchoSprite;
        int srcY = 0;
        int srcW = anchoSprite;
        int srcH = altoSprite;

        /*
          El destino se ajusta al tamano del rect del item.
          Esto garantiza que el sprite se escale y siempre se
          dibuje dentro de los limites del objeto grafico.
        */
        QRectF dstRect = rect();

        /*
          Se dibuja el pixmap escalado.
          Qt hace la transformacion de manera interna.
        */
        painter->drawPixmap(
            dstRect,            // rect destino escalado
            spriteSheet,        // imagen completa
            QRect(srcX, srcY, srcW, srcH) // rect fuente en el spritesheet
            );
    }
    else {
        painter->setBrush(brush());
        painter->setPen(pen());
        painter->drawRect(rect());
    }
}
