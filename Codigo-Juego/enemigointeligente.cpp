#include "enemigoInteligente.h"
#include "jugador.h"
#include "proyectilInteligente.h"  // *** CAMBIADO ***
#include <QGraphicsScene>
#include <QDebug>
#include <cmath>

/*
  Constructor
  Inicializa el enemigo inteligente con su sistema progresivo
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
    , modoPersecucion(false)
    , modoFrancotirador(false)
    , velocidadOriginal(0)
    , intervaloDisparo(2000)
    , areaDeteccionVisual(nullptr)
    , mostrarArea(false)
{
    // Guardar velocidad original (debe ser 0 para estático)
    velocidadOriginal = 0;
    speed = 0;

    // *** CRÍTICO: Configurar para que NO use la lógica de Enemigo base ***
    // Marcar como NO activo para que no use handleInput() de Enemigo
    // (Enemigo tiene una variable enemigoActivo que controla esto)

    // *** IMPORTANTE: Detener comportamiento aleatorio del padre ***
    upPressed = false;
    downPressed = false;
    leftPressed = false;
    rightPressed = false;

    // Crear el área visual de detección (invisible por defecto)
    areaDeteccionVisual = new QGraphicsEllipseItem(
        -radioDeteccion,
        -radioDeteccion,
        radioDeteccion * 2,
        radioDeteccion * 2,
        this
        );

    // Configurar estilo del área visual
    areaDeteccionVisual->setPen(QPen(QColor(255, 0, 0, 100), 2, Qt::DashLine));
    areaDeteccionVisual->setBrush(QBrush(QColor(255, 0, 0, 30)));
    areaDeteccionVisual->setZValue(-1);
    areaDeteccionVisual->setVisible(false);

    // Crear timer de disparo
    timerDisparo = new QTimer(this);
    connect(timerDisparo, &QTimer::timeout, this, &EnemigoInteligente::dispararAObjetivo);

    qDebug() << "EnemigoInteligente creado con radio:" << radioDeteccion;
    qDebug() << "  Velocidad inicial:" << speed << "(estático)";
}

EnemigoInteligente::~EnemigoInteligente()
{
    if (timerDisparo) {
        timerDisparo->stop();
    }
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
  Actualización por frame
  Verifica si el jugador está en el área y reacciona progresivamente
*/
void EnemigoInteligente::advance(int phase)
{
    if (!phase) return;

    // *** NO llamar a Enemigo::advance() para evitar su lógica ***
    // En su lugar, llamamos directamente a Persona::advance()
    // que maneja el movimiento básico

    // Verificar detección del jugador
    jugadorDetectado = detectarJugador();

    // Detectar cambios de estado (entrada/salida del área)
    if (jugadorDetectado && !jugadorDetectadoAnterior) {
        onJugadorDetectado();
    }
    else if (!jugadorDetectado && jugadorDetectadoAnterior) {
        onJugadorPerdido();
    }

    // Guardar estado para el siguiente frame
    jugadorDetectadoAnterior = jugadorDetectado;
}

/*
  Override de handleInput para persecución inteligente
*/
void EnemigoInteligente::handleInput()
{
    // Si no está en modo persecución, no hacer nada (comportamiento base)
    if (!modoPersecucion) {
        // Mantener quieto (modo estático)
        upPressed = false;
        downPressed = false;
        leftPressed = false;
        rightPressed = false;
        return;
    }

    // MODO PERSECUCIÓN ACTIVO
    Jugador* jugador = obtenerJugador();
    if (!jugador) return;

    // Calcular dirección hacia el jugador
    qreal dx = jugador->x() - x();
    qreal dy = jugador->y() - y();

    // Movimiento horizontal
    if (std::abs(dx) > 10) {  // Margen de error de 10 píxeles
        if (dx > 0) {
            rightPressed = true;
            leftPressed = false;
            mirandoIzquierda = false;
        } else {
            leftPressed = true;
            rightPressed = false;
            mirandoIzquierda = true;
        }
    } else {
        leftPressed = false;
        rightPressed = false;
    }

    // Movimiento vertical
    if (std::abs(dy) > 10) {
        if (dy > 0) {
            downPressed = true;
            upPressed = false;
        } else {
            upPressed = true;
            downPressed = false;
        }
    } else {
        upPressed = false;
        downPressed = false;
    }
}

/*
  Detecta si el jugador está dentro del área circular
*/
bool EnemigoInteligente::detectarJugador()
{
    Jugador* jugador = obtenerJugador();
    if (!jugador) return false;

    // Calcular centros de ambos objetos
    QPointF centroEnemigo = scenePos() +
                            QPointF(boundingRect().width() / 2,
                                    boundingRect().height() / 2);

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
  ════════════════════════════════════════════════════════════
  🎯 EVENTO: JUGADOR DETECTADO
  ════════════════════════════════════════════════════════════
  Sistema progresivo que escala con el número de detecciones:
  - Detecciones 1-3: Modo persecución (más lento que el jugador)
  - Detecciones 4+: Modo francotirador (dispara proyectiles)
*/
void EnemigoInteligente::onJugadorDetectado()
{
    // Incrementar contador
    contadorDetecciones++;

    qDebug() << "\n╔══════════════════════════════════════════════════════╗";
    qDebug() << "║  🔴 JUGADOR DETECTADO - Detección #" << contadorDetecciones << "               ║";
    qDebug() << "╚══════════════════════════════════════════════════════╝";

    // ════════════════════════════════════════════════════════════
    // FASE 1: PERSECUCIÓN (Detecciones 1-3)
    // ════════════════════════════════════════════════════════════
    if (contadorDetecciones <= 3) {
        qDebug() << "   📊 Modo: PERSECUCIÓN";
        qDebug() << "   🏃 El enemigo te seguirá (velocidad reducida)";
        qDebug() << "   💡 Consejo: ¡Aléjate del círculo rojo!";

        activarPersecucion();

        // Cambiar color a naranja (persecución)
        setBrush(QBrush(QColor(255, 165, 0)));

        // Cambiar color del área a naranja
        if (areaDeteccionVisual) {
            areaDeteccionVisual->setPen(QPen(QColor(255, 165, 0, 120), 2, Qt::DashLine));
            areaDeteccionVisual->setBrush(QBrush(QColor(255, 165, 0, 40)));
        }
    }

    // ════════════════════════════════════════════════════════════
    // FASE 2: FRANCOTIRADOR (Detecciones 4+)
    // ════════════════════════════════════════════════════════════
    else {
        qDebug() << "   📊 Modo: FRANCOTIRADOR";
        qDebug() << "   🎯 El enemigo dispará proyectiles a tu posición!";
        qDebug() << "   ⚠️  PELIGRO: ¡Mantente en movimiento!";

        activarModoFrancotirador();

        // Cambiar color a rojo intenso (peligro)
        setBrush(QBrush(QColor(200, 0, 0)));

        // Cambiar color del área a rojo intenso
        if (areaDeteccionVisual) {
            areaDeteccionVisual->setPen(QPen(QColor(200, 0, 0, 150), 3, Qt::SolidLine));
            areaDeteccionVisual->setBrush(QBrush(QColor(200, 0, 0, 60)));
        }
    }

    qDebug() << "   📍 Posición detectada:" << ultimaPosicionJugador;
    qDebug() << "";
}

/*
  Evento: Jugador perdido (sale del área)
*/
void EnemigoInteligente::onJugadorPerdido()
{
    qDebug() << "\n╔══════════════════════════════════════════════════════╗";
    qDebug() << "║  🟢 JUGADOR PERDIDO - Volviendo a vigilancia        ║";
    qDebug() << "╚══════════════════════════════════════════════════════╝";
    qDebug() << "   📊 Total de detecciones:" << contadorDetecciones;
    qDebug() << "";

    // Desactivar comportamientos activos
    if (modoPersecucion) {
        desactivarPersecucion();
    }

    if (modoFrancotirador) {
        timerDisparo->stop();
        modoFrancotirador = false;
    }

    // Volver a color original (azul)
    setBrush(QBrush(QColor(100, 100, 255)));

    // Restaurar color del área
    if (areaDeteccionVisual) {
        areaDeteccionVisual->setPen(QPen(QColor(255, 0, 0, 100), 2, Qt::DashLine));
        areaDeteccionVisual->setBrush(QBrush(QColor(255, 0, 0, 30)));
    }
}

/*
  ════════════════════════════════════════════════════════════
  🏃 MODO PERSECUCIÓN
  ════════════════════════════════════════════════════════════
*/
void EnemigoInteligente::activarPersecucion()
{
    if (modoPersecucion) return;

    modoPersecucion = true;

    // *** CRÍTICO: Establecer velocidad de persecución ***
    // Como el enemigo es estático (velocidad original = 0),
    // necesitamos darle una velocidad base para perseguir
    // Debe ser menor que la del jugador (jugador tiene 7.0)
    speed = 4.0;  // 60% aproximado de 7.0

    qDebug() << "   ✓ Persecución activada - Velocidad:" << speed;
}

void EnemigoInteligente::desactivarPersecucion()
{
    if (!modoPersecucion) return;

    modoPersecucion = false;

    // Volver a velocidad 0 (estático)
    speed = 0;

    // Detener movimiento
    upPressed = false;
    downPressed = false;
    leftPressed = false;
    rightPressed = false;

    qDebug() << "   ✓ Persecución desactivada - Volviendo a modo estático";
}

/*
  ════════════════════════════════════════════════════════════
  🎯 MODO FRANCOTIRADOR
  ════════════════════════════════════════════════════════════
*/
void EnemigoInteligente::activarModoFrancotirador()
{
    if (modoFrancotirador) return;

    modoFrancotirador = true;

    // Desactivar persecución si estaba activa
    if (modoPersecucion) {
        desactivarPersecucion();
    }

    // Iniciar disparos automáticos cada 2 segundos
    timerDisparo->start(intervaloDisparo);

    qDebug() << "   ✓ Modo francotirador activado - Disparo cada" << intervaloDisparo << "ms";
}

/*
  Dispara un proyectil hacia la última posición conocida del jugador
*/
void EnemigoInteligente::dispararAObjetivo()
{
    if (!scene() || !estaVivo()) {
        timerDisparo->stop();
        return;
    }

    Jugador* jugador = obtenerJugador();
    if (!jugador) return;

    // Calcular punto de inicio (centro del enemigo)
    QPointF centroEnemigo = scenePos() +
                            QPointF(boundingRect().width() / 2,
                                    boundingRect().height() / 2);

    qDebug() << "   🔫 DISPARANDO proyectil inteligente";
    qDebug() << "      Desde:" << centroEnemigo;
    qDebug() << "      Hacia:" << ultimaPosicionJugador;

    // Crear proyectil inteligente direccional
    ProyectilInteligente* proyectil = new ProyectilInteligente(
        centroEnemigo,           // Punto de inicio
        ultimaPosicionJugador,   // Punto objetivo
        8.0,                     // Velocidad
        1                        // Daño (1 vida)
        );

    proyectil->setOwner(this);
    scene()->addItem(proyectil);

    qDebug() << "      ✓ Proyectil creado";
}
