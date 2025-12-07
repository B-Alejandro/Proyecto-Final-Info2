#include "enemigointeligente.h"
#include "jugador.h"
#include "proyectilinteligente.h"
#include <QGraphicsScene>
#include <QDebug>
#include <cmath>

/*
  Constructor
  Torreta estática que dispara desde el inicio
*/
EnemigoInteligente::EnemigoInteligente(qreal w,
                                       qreal h,
                                       qreal sceneWidth,
                                       qreal sceneHeight,
                                       TipoMovimiento tipo,
                                       int nivel,
                                       qreal radioDeteccion)
    : Enemigo(w, h, sceneWidth, sceneHeight, tipo, nivel)
    , radioDeteccion(radioDeteccion)
    , jugadorDetectado(false)
    , jugadorDetectadoAnterior(false)
    , contadorDetecciones(0)
    , intervaloDisparo(3000)
    , cantidadProyectiles(1)
    , areaDeteccionVisual(nullptr)
    , mostrarArea(false)
{
    // Configurar como 100% ESTÁTICO
    speed = 0;
    upPressed = false;
    downPressed = false;
    leftPressed = false;
    rightPressed = false;

    // Crear área visual
    areaDeteccionVisual = new QGraphicsEllipseItem(
        -radioDeteccion,
        -radioDeteccion,
        radioDeteccion * 2,
        radioDeteccion * 2,
        this
        );

    areaDeteccionVisual->setPen(QPen(QColor(255, 0, 0, 100), 2, Qt::DashLine));
    areaDeteccionVisual->setBrush(QBrush(QColor(255, 0, 0, 30)));
    areaDeteccionVisual->setZValue(-1);
    areaDeteccionVisual->setVisible(false);

    // Timer de disparo - INICIA INMEDIATAMENTE
    timerDisparo = new QTimer(this);
    connect(timerDisparo, &QTimer::timeout, this, &EnemigoInteligente::dispararProyectiles);
    timerDisparo->start(intervaloDisparo);

    qDebug() << "Torreta creada - Radio:" << radioDeteccion
             << "Disparo inicial:" << intervaloDisparo << "ms";
}

EnemigoInteligente::~EnemigoInteligente()
{
    if (timerDisparo) {
        timerDisparo->stop();
    }
}

/*
  *** NUEVO: Override de boundingRect para hitbox más pequeña ***
  Reduce la hitbox al 60% del tamaño original para hacer más justo el juego
*/
QRectF EnemigoInteligente::boundingRect() const
{
    // Obtener el rectángulo original de Enemigo
    QRectF rect = Enemigo::boundingRect();

    // Reducir al 60% del tamaño (40% más pequeño)
    qreal reduccion = 0.4; // 40% de reducción
    qreal offsetX = rect.width() * reduccion / 2.0;
    qreal offsetY = rect.height() * reduccion / 2.0;

    return QRectF(
        rect.x() + offsetX,
        rect.y() + offsetY,
        rect.width() * (1.0 - reduccion),
        rect.height() * (1.0 - reduccion)
        );
}

/*
  *** NUEVO: Override de shape para colisión más precisa ***
  Hace que la forma de colisión sea circular en lugar de cuadrada
*/
QPainterPath EnemigoInteligente::shape() const
{
    QPainterPath path;
    QRectF rect = boundingRect();

    // Crear una elipse que coincida con el boundingRect reducido
    path.addEllipse(rect);

    return path;
}

/*
  Cambia el radio de detección
*/
void EnemigoInteligente::setRadioDeteccion(qreal radio)
{
    if (radio <= 0) return;

    radioDeteccion = radio;

    // Actualizar el área visual
    if (areaDeteccionVisual) {
        areaDeteccionVisual->setRect(
            -radioDeteccion,
            -radioDeteccion,
            radioDeteccion * 2,
            radioDeteccion * 2
            );
    }
}

/*
  Muestra u oculta el área de detección (útil para debug)
*/
void EnemigoInteligente::mostrarAreaDeteccion(bool mostrar)
{
    mostrarArea = mostrar;
    if (areaDeteccionVisual) {
        areaDeteccionVisual->setVisible(mostrar);
    }
}

/*
  Obtiene el puntero al jugador en la escena
*/
Jugador* EnemigoInteligente::obtenerJugador()
{
    if (!scene()) return nullptr;

    for (QGraphicsItem* item : scene()->items()) {
        Jugador* j = dynamic_cast<Jugador*>(item);
        if (j && j->estaVivo()) {
            return j;
        }
    }
    return nullptr;
}

/*
  Override de handleInput - Forzar que sea 100% estático
*/
void EnemigoInteligente::handleInput()
{
    // SIEMPRE estático - sin movimiento bajo ninguna circunstancia
    upPressed = false;
    downPressed = false;
    leftPressed = false;
    rightPressed = false;
    speed = 0;
}

/*
  Actualización por frame - Solo detecta, NO se mueve
*/
void EnemigoInteligente::advance(int phase)
{
    if (!phase) return;

    // NO llamar a Enemigo::advance() para evitar cualquier movimiento

    // Solo detectar jugador
    jugadorDetectado = detectarJugador();

    if (jugadorDetectado && !jugadorDetectadoAnterior) {
        onJugadorDetectado();
    }
    else if (!jugadorDetectado && jugadorDetectadoAnterior) {
        onJugadorPerdido();
    }

    jugadorDetectadoAnterior = jugadorDetectado;
}

/*
  Detecta si el jugador está dentro del área circular
*/
bool EnemigoInteligente::detectarJugador()
{
    Jugador* jugador = obtenerJugador();
    if (!jugador) return false;

    // Calcular centros de ambos objetos
    // Usar el boundingRect original (no el reducido) para el centro visual
    QRectF rectOriginal = Enemigo::boundingRect();
    QPointF centroEnemigo = scenePos() +
                            QPointF(rectOriginal.width() / 2,
                                    rectOriginal.height() / 2);

    QPointF centroJugador = jugador->scenePos() +
                            QPointF(jugador->boundingRect().width() / 2,
                                    jugador->boundingRect().height() / 2);

    // Guardar posición del jugador para disparos
    ultimaPosicionJugador = centroJugador;

    // Calcular distancia euclidiana
    qreal dx = centroJugador.x() - centroEnemigo.x();
    qreal dy = centroJugador.y() - centroEnemigo.y();
    qreal distancia = std::sqrt(dx * dx + dy * dy);

    return (distancia <= radioDeteccion);
}

/*
  ══════════════════════════════════════════════════════════════
  🎯 EVENTO: JUGADOR DETECTADO - AUMENTAR AGRESIVIDAD
  ══════════════════════════════════════════════════════════════
*/
void EnemigoInteligente::onJugadorDetectado()
{
    contadorDetecciones++;

    qDebug() << "\n╔════════════════════════════════════════════════════╗";
    qDebug() << "║  🔴 JUGADOR DETECTADO - Detección #" << contadorDetecciones << "               ║";
    qDebug() << "╚════════════════════════════════════════════════════╝";

    actualizarAgresividad();

    qDebug() << "   📍 Posición detectada:" << ultimaPosicionJugador;
    qDebug() << "";
}

void EnemigoInteligente::onJugadorPerdido()
{
    qDebug() << "\n╔════════════════════════════════════════════════════╗";
    qDebug() << "║  🟢 JUGADOR PERDIDO - Total detecciones:" << contadorDetecciones << "         ║";
    qDebug() << "╚════════════════════════════════════════════════════╝\n";
}

/*
  Actualiza agresividad según detecciones
*/
void EnemigoInteligente::actualizarAgresividad()
{
    // Detener timer actual
    timerDisparo->stop();

    if (contadorDetecciones == 1) {
        // NIVEL 1: Lento - 3s, 1 proyectil
        intervaloDisparo = 3000;
        cantidadProyectiles = 1;
        setBrush(QBrush(QColor(100, 100, 255)));  // Azul

        if (areaDeteccionVisual) {
            areaDeteccionVisual->setPen(QPen(QColor(100, 100, 255, 120), 2, Qt::DashLine));
            areaDeteccionVisual->setBrush(QBrush(QColor(100, 100, 255, 40)));
        }

        qDebug() << "   📊 NIVEL 1: Disparo lento (3s) - 1 proyectil";
    }
    else if (contadorDetecciones == 2) {
        // NIVEL 2: Normal - 2s, 1 proyectil
        intervaloDisparo = 2000;
        cantidadProyectiles = 1;
        setBrush(QBrush(QColor(255, 165, 0)));  // Naranja

        if (areaDeteccionVisual) {
            areaDeteccionVisual->setPen(QPen(QColor(255, 165, 0, 120), 2, Qt::DashLine));
            areaDeteccionVisual->setBrush(QBrush(QColor(255, 165, 0, 40)));
        }

        qDebug() << "   📊 NIVEL 2: Disparo normal (2s) - 1 proyectil";
    }
    else if (contadorDetecciones == 3) {
        // NIVEL 3: Rápido - 1.5s, 2 proyectiles
        intervaloDisparo = 1500;
        cantidadProyectiles = 2;
        setBrush(QBrush(QColor(255, 100, 0)));  // Naranja oscuro

        if (areaDeteccionVisual) {
            areaDeteccionVisual->setPen(QPen(QColor(255, 100, 0, 140), 3, Qt::SolidLine));
            areaDeteccionVisual->setBrush(QBrush(QColor(255, 100, 0, 50)));
        }

        qDebug() << "   📊 NIVEL 3: Disparo rápido (1.5s) - 2 proyectiles";
    }
    else {
        // NIVEL 4+: Muy rápido - 1s, 3 proyectiles
        intervaloDisparo = 1000;
        cantidadProyectiles = 3;
        setBrush(QBrush(QColor(200, 0, 0)));  // Rojo intenso

        if (areaDeteccionVisual) {
            areaDeteccionVisual->setPen(QPen(QColor(200, 0, 0, 200), 4, Qt::SolidLine));
            areaDeteccionVisual->setBrush(QBrush(QColor(200, 0, 0, 70)));
        }

        qDebug() << "   📊 NIVEL 4+: Disparo MUY rápido (1s) - 3 proyectiles";
        qDebug() << "   ⚠️  ¡MÁXIMO PELIGRO!";
    }

    // Reiniciar timer con nuevo intervalo
    timerDisparo->start(intervaloDisparo);
}

/*
  Dispara múltiples proyectiles según agresividad
*/
void EnemigoInteligente::dispararProyectiles()
{
    if (!scene() || !estaVivo()) {
        timerDisparo->stop();
        return;
    }

    Jugador* jugador = obtenerJugador();
    if (!jugador) return;

    // Usar el rectángulo original para el centro de disparo
    QRectF rectOriginal = Enemigo::boundingRect();
    QPointF centroEnemigo = scenePos() +
                            QPointF(rectOriginal.width() / 2,
                                    rectOriginal.height() / 2);

    qDebug() << "   🔫 DISPARANDO" << cantidadProyectiles << "proyectil(es)";

    // Disparar múltiples proyectiles con ligera variación angular
    for (int i = 0; i < cantidadProyectiles; ++i) {
        QPointF objetivo = ultimaPosicionJugador;

        // Si hay múltiples proyectiles, dispersarlos ligeramente
        if (cantidadProyectiles > 1) {
            qreal desviacion = (i - cantidadProyectiles / 2.0) * 30.0;  // 30px de separación
            objetivo.setX(objetivo.x() + desviacion);
        }

        ProyectilInteligente* proyectil = new ProyectilInteligente(
            centroEnemigo,
            objetivo,
            8.0,  // Velocidad
            1     // Daño
            );

        proyectil->setOwner(this);
        scene()->addItem(proyectil);
    }
}
