// ============ jugador.cpp ============
#include "jugador.h"
#include <QBrush>
#include <QDebug>
#include <QTimer>

Jugador::Jugador(qreal w,
                 qreal h,
                 qreal sceneWidth,
                 qreal sceneHeight,
                 TipoMovimiento tipo)
    : Persona(w, h, sceneWidth, sceneHeight, tipo)
{
    setBrush(QBrush(Qt::green));
    setFlag(QGraphicsItem::ItemIsFocusable);
    setFocus();

    // Cargar sprites por defecto
    cargarSprites();
}

void Jugador::cargarSprites()
{
    qDebug() << "=== CARGANDO SPRITES DEL JUGADOR ===";

    // Rutas de los sprites
    QString rutaRun = ":/Recursos/Sprites/Run_homeless.png";
    QString rutaJump = ":/Recursos/Sprites/Jump_homeless.png";
    QString rutaDeath = ":/Recursos/Sprites/Dead.png";  // *** NUEVO ***

    spriteCorrer = QPixmap(rutaRun);
    spriteSaltar = QPixmap(rutaJump);
    spriteMuerte = QPixmap(rutaDeath);  // *** NUEVO ***

    // Verificación detallada
    if (spriteCorrer.isNull()) {
        qDebug() << "❌ ERROR: No se pudo cargar" << rutaRun;
        qDebug() << "   Verifica que el archivo existe en el .qrc";
    } else {
        qDebug() << "✓ Sprite correr cargado:" << spriteCorrer.width() << "x" << spriteCorrer.height();
    }

    if (spriteSaltar.isNull()) {
        qDebug() << "⚠ No se pudo cargar" << rutaJump << "- usando fallback";
        spriteSaltar = spriteCorrer;
    } else {
        qDebug() << "✓ Sprite saltar cargado:" << spriteSaltar.width() << "x" << spriteSaltar.height();
    }

    // *** NUEVO: Verificar sprite de muerte ***
    if (spriteMuerte.isNull()) {
        qDebug() << "⚠ No se pudo cargar" << rutaDeath << "- usando fallback";
        spriteMuerte = spriteCorrer;
    } else {
        qDebug() << "✓ Sprite muerte cargado:" << spriteMuerte.width() << "x" << spriteMuerte.height();
    }

    // Establecer sprite inicial (correr)
    if (!spriteCorrer.isNull()) {
        setSprite(rutaRun, 128, 128, 8);
        setAnimacion(EstadoAnimacion::CORRIENDO);
    }

    qDebug() << "=== FIN CARGA DE SPRITES ===\n";
}

// Este método se llama automáticamente cuando cambia el estado
void Jugador::onEstadoAnimacionCambiado()
{
    cambiarSpritePorEstado();
}

void Jugador::cambiarSpritePorEstado()
{
    EstadoAnimacion estado = getEstadoAnimacion();

    // *** MODIFICADO: Prioridad al estado MUERTO ***
    if (estado == EstadoAnimacion::MUERTO && !spriteMuerte.isNull()) {
        // Cambiar a sprite de muerte
        if (spriteSheet.cacheKey() != spriteMuerte.cacheKey()) {
            spriteSheet = spriteMuerte;
            frameActual = 0;
            totalFrames = 8;  // Ajusta según tu sprite de muerte
            qDebug() << "→ Sprite MUERTE activado";
        }
    }
    else if (estado == EstadoAnimacion::SALTANDO && !spriteSaltar.isNull()) {
        // Cambiar a sprite de salto
        if (spriteSheet.cacheKey() != spriteSaltar.cacheKey()) {
            spriteSheet = spriteSaltar;
            frameActual = 0;
            qDebug() << "→ Sprite SALTAR activado";
        }
    }
    else if (estado == EstadoAnimacion::CORRIENDO && !spriteCorrer.isNull()) {
        // Cambiar a sprite de correr
        if (spriteSheet.cacheKey() != spriteCorrer.cacheKey()) {
            spriteSheet = spriteCorrer;
            frameActual = 0;
            qDebug() << "→ Sprite CORRER activado";
        }
    }
}

// *** NUEVO: Método para activar animación de muerte ***
void Jugador::activarAnimacionMuerte()
{
    qDebug() << "🎬 Activando animación de muerte";

    // Cambiar al estado de muerte
    setAnimacion(EstadoAnimacion::MUERTO);

    // Crear timer para reproducir la animación una sola vez
    // Duración: 800ms = aproximadamente 8 frames a 10fps
    QTimer::singleShot(800, this, [this]() {
        pausarAnimacion();
        qDebug() << "⏸️ Animación de muerte completada - todo pausado";
    });
}

void Jugador::keyPressEvent(QKeyEvent* event)
{
    // *** NUEVO: No procesar teclas si está muerto ***
    if (estadoActual == EstadoAnimacion::MUERTO) {
        return;
    }

    if (tipoMovimiento == TipoMovimiento::RECTILINEO) {
        switch (event->key()) {
        case Qt::Key_Up:
        case Qt::Key_W:
            upPressed = true;
            break;
        case Qt::Key_Down:
        case Qt::Key_S:
            downPressed = true;
            break;
        case Qt::Key_Left:
        case Qt::Key_A:
            leftPressed = true;
            break;
        case Qt::Key_Right:
        case Qt::Key_D:
            rightPressed = true;
            break;
        }
    } else {
        // CON_GRAVEDAD: SOLO movimiento vertical (salto)
        switch (event->key()) {
        case Qt::Key_Up:
        case Qt::Key_W:
        case Qt::Key_Space:
            if (onGround) {
                vy = -10;
                onGround = false;
                qDebug() << "¡SALTO! Estado: SALTANDO";
                // El cambio de animación se maneja automáticamente
                // en updateMovementConGravedad() -> setAnimacion()
            }
            break;
            // *** NO HAY MOVIMIENTO HORIZONTAL ***
            // No procesamos teclas Left/Right/A/D
        }
    }
}

void Jugador::keyReleaseEvent(QKeyEvent* event)
{
    // *** NUEVO: No procesar teclas si está muerto ***
    if (estadoActual == EstadoAnimacion::MUERTO) {
        return;
    }

    if (tipoMovimiento == TipoMovimiento::RECTILINEO) {
        switch (event->key()) {
        case Qt::Key_Up:
        case Qt::Key_W:
            upPressed = false;
            break;
        case Qt::Key_Down:
        case Qt::Key_S:
            downPressed = false;
            break;
        case Qt::Key_Left:
        case Qt::Key_A:
            leftPressed = false;
            break;
        case Qt::Key_Right:
        case Qt::Key_D:
            rightPressed = false;
            break;
        }
    } else {
        // CON_GRAVEDAD: No hay teclas de movimiento horizontal que liberar
        // Los cambios de animación se manejan automáticamente
    }
}
