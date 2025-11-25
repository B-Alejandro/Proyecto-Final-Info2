#include "persona.h"
#include "fisica.h"
#include <QPainter>
#include <QDebug>
#include <QGraphicsScene>

/*
  Persona.cpp corregido
  - Restaura el timer interno de movimiento para compatibilidad con Nivel1
  - Mantiene el sistema de vida mejorado con invulnerabilidad
  - boundingRect declarado en header y definido aqui
  - paint dibuja sin escalado para evitar parpadeo
*/

/* Constructor: inicializacion en el mismo orden que los miembros en persona.h */
Persona::Persona(qreal w, qreal h, qreal sceneWidth, qreal sceneHeight, TipoMovimiento tipo)
    : QGraphicsRectItem(0, 0, w, h),
    tipoMovimiento(tipo),
    sceneW(sceneWidth),
    sceneH(sceneHeight),
    speed(5.0),
    timer(nullptr),
    spriteSheet(),
    anchoSprite(static_cast<int>(w)),
    altoSprite(static_cast<int>(h)),
    totalFrames(1),
    frameActual(0),
    usarSprites(false),
    estadoActual(EstadoAnimacion::IDLE),
    timerAnimacion(nullptr),
    animacionPausada(false),
    mirandoIzquierda(false),
    vidaActual(100),
    vidaMaxima(100),
    invulnerable(false),
    timerInvulnerabilidad(nullptr)
{
    setPen(QPen(Qt::NoPen));
    setBrush(QBrush(Qt::blue));

    // flags y valores iniciales de entrada/fisica
    upPressed = false;
    downPressed = false;
    leftPressed = false;
    rightPressed = false;
    vy = 0.0;
    g = 0.5;
    onGround = false;

    // *** RESTAURADO: timer interno de movimiento ***
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Persona::updateMovement);
    timer->start(16); // ~60 FPS

    // timer exclusivo para animacion
    timerAnimacion = new QTimer(this);
    connect(timerAnimacion, &QTimer::timeout, this, &Persona::actualizarAnimacion);
    timerAnimacion->start(60);

    // timer invulnerabilidad
    timerInvulnerabilidad = new QTimer(this);
    timerInvulnerabilidad->setSingleShot(true);
    connect(timerInvulnerabilidad, &QTimer::timeout, this, &Persona::finalizarInvulnerabilidad);

    setFlag(QGraphicsItem::ItemIsFocusable);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges);
    setCacheMode(QGraphicsItem::DeviceCoordinateCache);

    qDebug() << "Persona creada: anchoSprite=" << anchoSprite << " altoSprite=" << altoSprite;
}

/* boundingRect override para que Qt use el tamano del sprite sin recalculos extra */
QRectF Persona::boundingRect() const
{
    return QRectF(0, 0, anchoSprite, altoSprite);
}

/* recibir danio: aplica invulnerabilidad corta */
void Persona::recibirDanio(int cantidad)
{
    if (!estaVivo()) return;

    // si ya esta invulnerable, ignorar
    if (invulnerable) return;

    vidaActual -= cantidad;
    if (vidaActual < 0) vidaActual = 0;

    qDebug() << "Persona recibió daño:" << cantidad << "Vida restante:" << vidaActual;

    emit vidaCambiada(vidaActual, vidaMaxima);

    // invulnerabilidad corta para evitar daños frames contiguos
    invulnerable = true;
    timerInvulnerabilidad->start(200);

    if (vidaActual <= 0) {
        qDebug() << "Persona murió, eliminando de escena";
        setAnimacion(EstadoAnimacion::MUERTO);

        // *** DETENER MOVIMIENTO AL MORIR ***
        if (timer) timer->stop();
        if (timerAnimacion) timerAnimacion->stop();

        emit murioPersona();
        emit died(this);  // Para compatibilidad con nivel 1

        // *** ELIMINAR DE LA ESCENA INMEDIATAMENTE ***
        if (scene()) {
            scene()->removeItem(this);
        }
        deleteLater();
    }
}

void Persona::curar(int cantidad)
{
    if (!estaVivo()) return;

    vidaActual += cantidad;
    if (vidaActual > vidaMaxima) vidaActual = vidaMaxima;

    emit vidaCambiada(vidaActual, vidaMaxima);
}

void Persona::finalizarInvulnerabilidad()
{
    invulnerable = false;
}

/* setSprite: configura parametros de sprite y actualiza rect */
void Persona::setSprite(const QString& rutaImagen, int anchoFrame, int altoFrame, int numFrames)
{
    spriteSheet = QPixmap(rutaImagen);
    if (spriteSheet.isNull()) {
        usarSprites = false;
        qDebug() << "Persona::setSprite - no se pudo cargar:" << rutaImagen;
        return;
    }

    anchoSprite = anchoFrame;
    altoSprite = altoFrame;
    totalFrames = numFrames;
    frameActual = 0;
    usarSprites = true;

    prepareGeometryChange();
    setRect(0, 0, anchoFrame, altoFrame);

    qDebug() << "Sprite cargado. Frames:" << numFrames
             << "Ancho:" << anchoFrame
             << "Alto:" << altoFrame;
}

void Persona::onEstadoAnimacionCambiado()
{
    // implementacion por defecto vacia
}

void Persona::pausarAnimacion()
{
    animacionPausada = true;
    if (timerAnimacion) timerAnimacion->stop();
    if (timer) timer->stop();  // También pausar movimiento
}

void Persona::reanudarAnimacion()
{
    animacionPausada = false;
    if (timerAnimacion) timerAnimacion->start(60);
    if (timer) timer->start(16);  // También reanudar movimiento
}

void Persona::handleInput()
{
    // metodo virtual, sobrescrito por clases derivadas
}

void Persona::updateMovement()
{
    // *** NO actualizar si está pausado o muerto ***
    if (animacionPausada || estadoActual == EstadoAnimacion::MUERTO) return;

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

    if (upPressed) dy -= speed;
    if (downPressed) dy += speed;
    if (leftPressed) {
        dx -= speed;
        mirandoIzquierda = true;
    }
    if (rightPressed) {
        dx += speed;
        mirandoIzquierda = false;
    }

    if (dx != 0 || dy != 0) setAnimacion(EstadoAnimacion::CORRIENDO);
    else setAnimacion(EstadoAnimacion::IDLE);

    Fisica::aplicarMovimientoRectilineo(this, dx, dy, sceneW, sceneH);
}

void Persona::updateMovementConGravedad()
{
    double dx = 0;

    if (leftPressed) {
        dx -= speed;
        mirandoIzquierda = true;
    }
    if (rightPressed) {
        dx += speed;
        mirandoIzquierda = false;
    }

    if (estadoActual == EstadoAnimacion::MUERTO) return;

    EstadoAnimacion nuevoEstado;
    if (!onGround) nuevoEstado = EstadoAnimacion::SALTANDO;
    else if (dx != 0) nuevoEstado = EstadoAnimacion::CORRIENDO;
    else nuevoEstado = EstadoAnimacion::IDLE;

    if (estadoActual != nuevoEstado) setAnimacion(nuevoEstado);

    Fisica::aplicarMovimientoConGravedad(this, dx, vy, g, onGround, sceneW, sceneH);
}

void Persona::setAnimacion(EstadoAnimacion estado)
{
    if (estadoActual != estado) {
        estadoActual = estado;
        frameActual = 0;
        onEstadoAnimacionCambiado();
    }
}

void Persona::actualizarAnimacion()
{
    if (!usarSprites || animacionPausada) return;

    frameActual++;
    if (frameActual >= totalFrames) frameActual = 0;

    update(); // solicita repaint
}

void Persona::setTipoMovimiento(TipoMovimiento tipo)
{
    tipoMovimiento = tipo;
}

/* paint: dibuja el frame sin escalado y maneja flip horizontal */
void Persona::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    if (usarSprites && !spriteSheet.isNull()) {

        int srcX = frameActual * anchoSprite;
        int srcY = 0;

        if (mirandoIzquierda) {
            painter->save();
            painter->translate(anchoSprite / 2.0, altoSprite / 2.0);
            painter->scale(-1, 1);
            painter->translate(-anchoSprite / 2.0, -altoSprite / 2.0);
        }

        painter->drawPixmap(0, 0, spriteSheet,
                            srcX, srcY,
                            anchoSprite, altoSprite);

        if (mirandoIzquierda) painter->restore();

        // Efecto visual de invulnerabilidad
        if (invulnerable) {
            painter->fillRect(0, 0, anchoSprite, altoSprite, QColor(255, 255, 255, 120));
        }

    } else {
        painter->setBrush(brush());
        painter->setPen(pen());
        painter->drawRect(rect());
    }
}
