// ============ jugador.cpp ============
#include "jugador.h"
#include <QBrush>
#include <QDebug>
#include <QTimer>
#include "proyectil.h"
#include <QGraphicsScene>
#include <QKeyEvent> // Asegurar inclusión para eventos de teclado

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
}

void Jugador::cargarSpritesnivel2()
{
    qDebug() << "=== CARGANDO SPRITES DEL JUGADOR ===";

    QString rutaIdle = ":/Recursos/Sprites/Idle_homeless.png";
    QString rutaRun = ":/Recursos/Sprites/Run_homeless.png";
    QString rutaJump = ":/Recursos/Sprites/Jump_homeless.png";
    QString rutaDeath = ":/Recursos/Sprites/Dead.png";

    spriteIdle = QPixmap(rutaIdle);
    spriteCorrer = QPixmap(rutaRun);
    spriteSaltar = QPixmap(rutaJump);
    spriteMuerte = QPixmap(rutaDeath);

    if (spriteIdle.isNull()) {
        qDebug() << " ERROR: No se pudo cargar" << rutaIdle;
        spriteIdle = spriteCorrer;  // Fallback
    } else {
        qDebug() << " Sprite IDLE cargado:" << spriteIdle.width() << "x" << spriteIdle.height();
    }

    if (spriteCorrer.isNull()) {
        qDebug() << " ERROR: No se pudo cargar" << rutaRun;
        qDebug() << "     Verifica que el archivo existe en el .qrc";
    } else {
        qDebug() << " Sprite correr cargado:" << spriteCorrer.width() << "x" << spriteCorrer.height();
    }

    if (spriteSaltar.isNull()) {
        qDebug() << " No se pudo cargar" << rutaJump << "- usando fallback";
        spriteSaltar = spriteCorrer;
    } else {
        qDebug() << " Sprite saltar cargado:" << spriteSaltar.width() << "x" << spriteSaltar.height();
    }

    if (spriteMuerte.isNull()) {
        qDebug() << " No se pudo cargar" << rutaDeath << "- usando fallback";
        spriteMuerte = spriteCorrer;
    } else {
        qDebug() << " Sprite muerte cargado:" << spriteMuerte.width() << "x" << spriteMuerte.height();
    }

    // Iniciar con sprite IDLE
    if (!spriteIdle.isNull()) {
        // CORRECCIÓN 1: Establecer a 1 frame para detener el repintado en reposo
        setSprite(rutaIdle, 128, 128, 1);
        setAnimacion(EstadoAnimacion::IDLE);
    } else if (!spriteCorrer.isNull()) {
        setSprite(rutaRun, 128, 128, 8);
        setAnimacion(EstadoAnimacion::IDLE);
    }

    qDebug() << "=== FIN CARGA DE SPRITES ===\n";
}

void Jugador::onEstadoAnimacionCambiado()
{
    cambiarSpritePorEstado();
}

void Jugador::cambiarSpritePorEstado()
{
    EstadoAnimacion estado = getEstadoAnimacion();

    // Las animaciones de MUERTO, SALTAR y CORRER mantienen 8 frames.
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
            // CORRECCIÓN 2: Establecer a 1 frame aquí también.
            totalFrames = 1;
            qDebug() << "Sprite IDLE activado";
        }
    }
}

void Jugador::activarAnimacionMuerte()
{
    qDebug() << "Activando animacion de muerte";

    setAnimacion(EstadoAnimacion::MUERTO);

    QTimer::singleShot(800, this, [this]() {
        pausarAnimacion();
        qDebug() << "Animacion de muerte completada - todo pausado";
    });
}

void Jugador::keyPressEvent(QKeyEvent* event)
{
    if (estadoActual == EstadoAnimacion::MUERTO) {
        return;
    }

    if (event->key() == Qt::Key_Space) {
        if (!scene()) return;

        qreal projW = 10;
        qreal projH = 16;
        qreal projSpeed = 10.0;
        int dirY = -1; // hacia arriba

        Proyectil* bala = new Proyectil(projW, projH, projSpeed, dirY);
        bala->setOwner(this);

        // spawn en la “parte superior” del jugador
        QRectF br = boundingRect();             // local
        QPointF spawn = scenePos()
                        + QPointF(0, br.top() - projH/2.0);

        bala->setPos(spawn);
        scene()->addItem(bala);
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
        // CON_GRAVEDAD: Solo cambiar el estado de las teclas
        // NO forzar animaciones aqui - persona.cpp lo maneja
        switch (event->key()) {
        case Qt::Key_Up:
        case Qt::Key_W:
        case Qt::Key_Space:
            if (onGround) {
                vy = -10;
                onGround = false;
                qDebug() << "SALTO!";
            }
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
    }
}

void Jugador::keyReleaseEvent(QKeyEvent* event)
{
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
        // CON_GRAVEDAD: Solo cambiar el estado de las teclas
        // NO forzar animaciones aqui - persona.cpp lo maneja
        switch (event->key()) {
        case Qt::Key_Left:
        case Qt::Key_A:
            leftPressed = false;
            break;
        case Qt::Key_Right:
        case Qt::Key_D:
            rightPressed = false;
            break;
        }
    }
}
