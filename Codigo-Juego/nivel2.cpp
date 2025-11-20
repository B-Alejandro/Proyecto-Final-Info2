// ============ nivel2.cpp ============
#include "nivel2.h"
#include "juego.h"
#include "jugador.h"
#include "enemigo.h"
#include "obstaculo.h"
#include "persona.h"
#include "fisica.h"
#include "GameOverScreen.h"
#include <QGraphicsView>
#include <QBrush>
#include <QRandomGenerator>
#include <QDebug>
#include <QTimer>

Nivel2::Nivel2(Juego* juego, QObject* parent)
    : NivelBase(juego, 2, parent), enemigoAtras(0), suelo(0),
    pantallaGameOver(nullptr), juegoEnPausa(false)
{
    // Guardar dimensiones de la vista
    vistaAncho = juego->getVistaAncho();
    vistaAlto = juego->getVistaAlto();

    // Crear escena 3 veces más grande que la vista
    int anchoEscena = vistaAncho * 3;
    int altoEscena = vistaAlto;

    crearEscena(anchoEscena, altoEscena);

    // Conectar señal de juego terminado
    connect(this, &Nivel2::juegoTerminado, this, &Nivel2::onJuegoTerminado);
}

Nivel2::~Nivel2()
{
    if (pantallaGameOver) {
        delete pantallaGameOver;
        pantallaGameOver = nullptr;
    }
}

void Nivel2::configurarNivel()
{
    // Crear jugador a mitad de pantalla
    qreal posJugadorX = vistaAncho * 0.5;
    qreal alturaSuelo = sceneH * 0.85;

    // Tamaño del jugador
    qreal anchoJugador = sceneH * 0.15;
    qreal altoJugador = sceneH * 0.15;

    qreal posJugadorY = alturaSuelo - altoJugador;

    // Crear jugador con nuevo tamaño
    jugador = new Jugador(anchoJugador, altoJugador, sceneW, sceneH, TipoMovimiento::CON_GRAVEDAD);
    jugador->setPos(posJugadorX, posJugadorY);
    escena->addItem(jugador);

    // Verificar que los sprites se cargaron
    if (jugador) {
        Jugador* j = dynamic_cast<Jugador*>(jugador);
        if (j) {
            j->cargarSprites();
            qDebug() << "Sprites del jugador cargados en Nivel2";
        }
    }

    // Crear pantalla de Game Over
    if (escena) {
        pantallaGameOver = new GameOverScreen(escena, this);
    }

    // Centrar la vista inicialmente en el jugador
    if (escena && !escena->views().isEmpty()) {
        QGraphicsView* vista = escena->views().first();
        vista->centerOn(jugador);
    }
}

void Nivel2::crearEnemigos()
{
    if (!jugador) return;

    // Crear UN SOLO enemigo atrás del jugador
    qreal posEnemigoX = vistaAncho * 0.2;
    qreal alturaSuelo = sceneH * 0.85;
    qreal posEnemigoY = alturaSuelo - sceneH * 0.08;

    int sizeEnemigo = sceneH * 0.08;
    enemigoAtras =  new Enemigo(sizeEnemigo,
                                                         sizeEnemigo,
                                                         sceneW,
                                                         sceneH,
                                                         TipoMovimiento::CON_GRAVEDAD,
                                                         2);

    enemigoAtras->setPos(posEnemigoX, posEnemigoY);
    enemigoAtras->setBrush(QBrush(Qt::red));
    enemigoAtras->setSpeed(4);

    escena->addItem(enemigoAtras);
    enemigos.append(enemigoAtras);
}

void Nivel2::crearObstaculos()
{
    // Crear el SUELO
    qreal posYSuelo = sceneH * 0.85;
    qreal alturaSuelo = sceneH * 0.15;

    suelo = new Obstaculo(0, posYSuelo, sceneW, alturaSuelo, Qt::darkGreen);
    escena->addItem(suelo);
    obstaculos.append(suelo);

    // Crear obstáculos iniciales
    for (int i = 0; i < 8; i++) {
        qreal factorSeparacion = 1.8; // Mayor separación inicial
        qreal posX = vistaAncho * (1.0 + i * factorSeparacion); // Distribuidos hacia adelante

        int ancho = 50 + QRandomGenerator::global()->bounded(30);
        int alto = 50 + QRandomGenerator::global()->bounded(30);

        qreal posY = posYSuelo - alto;

        Obstaculo* obs = new Obstaculo(posX, posY, ancho, alto, Qt::darkGray);
        obs->setBorderColor(Qt::black, 2);

        escena->addItem(obs);
        obstaculos.append(obs);
    }
}

void Nivel2::actualizar()
{
    // Si el juego está en pausa (Game Over), no actualizar
    if (juegoEnPausa) {
        return;
    }

    // Llamar actualización base
    NivelBase::actualizar();

    if (jugador && escena && !escena->views().isEmpty()) {
        QGraphicsView* vista = escena->views().first();

        // Velocidad de movimiento de los obstáculos (hacia la izquierda)
        qreal velocidadObstaculos = 9.0;

        // === DETECTAR COLISIONES ANTES DE MOVER OBSTÁCULOS ===

        // Verificar colisiones del jugador con obstáculos ANTES de mover
        QList<QGraphicsItem*> colisionesJugador = jugador->collidingItems();
        bool jugadorEmpujado = false;

        for (QGraphicsItem* item : colisionesJugador) {
            Obstaculo* obs = dynamic_cast<Obstaculo*>(item);

            if (obs && obs != suelo) {
                QRectF rectJugador = jugador->sceneBoundingRect();
                QRectF rectObs = obs->sceneBoundingRect();

                // Verificar si el obstáculo está a la derecha del jugador (empujándolo)
                qreal overlapIzq = rectJugador.right() - rectObs.left();
                qreal overlapDer = rectObs.right() - rectJugador.left();

                // Si hay mayor overlap desde la derecha, el obstáculo empuja
                if (overlapDer > overlapIzq && overlapIzq > 0) {
                    // El obstáculo está empujando al jugador desde la derecha
                    jugadorEmpujado = true;
                    break;
                }
            }
        }

        // Verificar colisiones del enemigo con obstáculos ANTES de mover
        bool enemigoEmpujado = false;
        if (enemigoAtras) {
            QList<QGraphicsItem*> colisionesEnemigo = enemigoAtras->collidingItems();

            for (QGraphicsItem* item : colisionesEnemigo) {
                Obstaculo* obs = dynamic_cast<Obstaculo*>(item);

                if (obs && obs != suelo) {
                    QRectF rectEnemigo = enemigoAtras->sceneBoundingRect();
                    QRectF rectObs = obs->sceneBoundingRect();

                    // Verificar si el obstáculo está a la derecha del enemigo
                    qreal overlapIzq = rectEnemigo.right() - rectObs.left();
                    qreal overlapDer = rectObs.right() - rectEnemigo.left();

                    if (overlapDer > overlapIzq && overlapIzq > 0) {
                        enemigoEmpujado = true;
                        break;
                    }
                }
            }
        }

        // === MOVER OBSTÁCULOS Y EMPUJAR PERSONAJES ===
        for (Obstaculo* obs : obstaculos) {
            if (obs != suelo) {
                // Mover el obstáculo hacia la izquierda
                obs->setPos(obs->x() - velocidadObstaculos, obs->y());
            }
        }

        // Si el jugador está siendo empujado, moverlo con el obstáculo
        if (jugadorEmpujado) {
            jugador->setX(jugador->x() - velocidadObstaculos);
        }

        // Si el enemigo está siendo empujado, moverlo con el obstáculo
        if (enemigoEmpujado && enemigoAtras) {
            enemigoAtras->setX(enemigoAtras->x() - velocidadObstaculos);
        }

        // === RESOLVER PENETRACIONES DESPUÉS DEL MOVIMIENTO ===

        // Resolver penetración del jugador
        QList<QGraphicsItem*> colisionesPostJugador = jugador->collidingItems();
        for (QGraphicsItem* item : colisionesPostJugador) {
            Obstaculo* obs = dynamic_cast<Obstaculo*>(item);

            if (obs && obs != suelo) {
                QRectF rectJugador = jugador->sceneBoundingRect();
                QRectF rectObs = obs->sceneBoundingRect();

                // Si hay penetración lateral, ajustar posición
                qreal overlapIzq = rectJugador.right() - rectObs.left();
                qreal overlapDer = rectObs.right() - rectJugador.left();

                if (overlapIzq > 0 && overlapDer > 0) {
                    // Empujar al jugador fuera del obstáculo
                    if (overlapIzq < overlapDer) {
                        // Empujar hacia la izquierda
                        jugador->setX(rectObs.left() - rectJugador.width());
                    } else {
                        // Empujar hacia la derecha
                        jugador->setX(rectObs.right());
                    }
                }
            }
        }

        // Resolver penetración del enemigo
        if (enemigoAtras) {
            QList<QGraphicsItem*> colisionesPostEnemigo = enemigoAtras->collidingItems();
            for (QGraphicsItem* item : colisionesPostEnemigo) {
                Obstaculo* obs = dynamic_cast<Obstaculo*>(item);

                if (obs && obs != suelo) {
                    QRectF rectEnemigo = enemigoAtras->sceneBoundingRect();
                    QRectF rectObs = obs->sceneBoundingRect();

                    qreal overlapIzq = rectEnemigo.right() - rectObs.left();
                    qreal overlapDer = rectObs.right() - rectEnemigo.left();

                    if (overlapIzq > 0 && overlapDer > 0) {
                        if (overlapIzq < overlapDer) {
                            enemigoAtras->setX(rectObs.left() - rectEnemigo.width());
                        } else {
                            enemigoAtras->setX(rectObs.right());
                        }
                    }
                }
            }
        }

        // === DETECTAR COLISIÓN JUGADOR-ENEMIGO (GAME OVER) ===
        if (enemigoAtras && jugador) {
            QRectF rectJugador = jugador->sceneBoundingRect();
            QRectF rectEnemigo = enemigoAtras->sceneBoundingRect();

            // Verificar si se tocan
            if (rectJugador.intersects(rectEnemigo)) {
                qDebug() << "💥 COLISION DETECTADA - GAME OVER!";

                // *** NUEVO: Activar animación de muerte ANTES de mostrar Game Over ***
                Jugador* j = dynamic_cast<Jugador*>(jugador);
                if (j) {
                    j->activarAnimacionMuerte();
                }

                // Pausar enemigo también
                if (enemigoAtras) {
                    enemigoAtras->pausarAnimacion();
                }

                // Pausar el juego inmediatamente
                juegoEnPausa = true;

                // Mostrar pantalla de Game Over después de que termine la animación
                QTimer::singleShot(900, this, [this]() {
                    emit juegoTerminado();
                });

                return; // Detener actualización
            }
        }

        // === LÓGICA DE PERSECUCIÓN DEL ENEMIGO ===
        if (enemigoAtras && !enemigoEmpujado) {
            qreal distanciaIdeal = vistaAncho * 0.3;
            qreal posIdealX = jugador->x() - distanciaIdeal;

            // Simple persecución/reposicionamiento
            if (enemigoAtras->x() < posIdealX - 50) {
                enemigoAtras->setX(posIdealX - 50);
            }
        }

        // Centrar la vista en el jugador
        vista->centerOn(jugador->x(), sceneH / 2);

        // Regenerar obstáculos que salen por la izquierda
        qreal limiteIzquierdo = jugador->x() - vistaAncho;
        qreal posYSuelo = sceneH * 0.85;

        // === PASO 1: ENCONTRAR EL BORDE DERECHO MÁS LEJANO (maxPosX) ===
        qreal maxPosX = jugador->x() + vistaAncho;

        for (Obstaculo* obs : obstaculos) {
            if (obs != suelo) {
                qreal bordeDerecho = obs->x() + obs->rect().width();
                if (bordeDerecho > maxPosX) {
                    maxPosX = bordeDerecho;
                }
            }
        }

        // === PASO 2: REPOSICIONAR OBSTÁCULOS RECICLADOS ===
        for (Obstaculo* obs : obstaculos) {
            if (obs != suelo && obs->x() < limiteIzquierdo) {
                qreal separacionMinima = 300.0;
                qreal variacionAdicional = QRandomGenerator::global()->bounded(400);

                qreal nuevoX = maxPosX + separacionMinima + variacionAdicional;

                int ancho = 50 + QRandomGenerator::global()->bounded(30);
                int alto = 50 + QRandomGenerator::global()->bounded(50);

                qreal nuevoY = posYSuelo - alto;

                obs->setPos(nuevoX, nuevoY);
                obs->setRect(0, 0, ancho, alto);

                maxPosX = nuevoX + ancho;
            }
        }
    }
}

void Nivel2::onJuegoTerminado()
{
    qDebug() << "🎮 onJuegoTerminado llamado";

    // El juego ya está pausado desde actualizar()

    // Mostrar pantalla de Game Over
    if (pantallaGameOver) {
        qDebug() << "📺 Mostrando pantalla de Game Over";
        pantallaGameOver->mostrar();
    } else {
        qDebug() << "❌ ERROR: pantallaGameOver es NULL";
    }
}

void Nivel2::manejarTecla(Qt::Key key)
{
    if (!juegoEnPausa) return;

    qDebug() << "⌨️ Tecla presionada en Game Over:" << key;

    // Tecla R para reiniciar
    if (key == Qt::Key_R) {
        qDebug() << " Reiniciando nivel...";
        juego->iniciar();

        // Ocultar pantalla de Game Over
        if (pantallaGameOver) {
            pantallaGameOver->ocultar();
        }

        // Reiniciar el juego
        juegoEnPausa = false;

        // *** NUEVO: Reanudar animaciones del jugador ***
        if (jugador) {
            Jugador* j = dynamic_cast<Jugador*>(jugador);
            if (j) {
                j->reanudarAnimacion();
                j->setAnimacion(EstadoAnimacion::CORRIENDO);
            }

            // Reposicionar jugador
            qreal posJugadorX = vistaAncho * 0.5;
            qreal alturaSuelo = sceneH * 0.85;
            qreal posJugadorY = alturaSuelo - sceneH * 0.15;
            jugador->setPos(posJugadorX, posJugadorY);
        }

        // *** NUEVO: Reanudar animaciones del enemigo ***
        if (enemigoAtras) {
            enemigoAtras->reanudarAnimacion();

            // Reposicionar enemigo
            qreal posEnemigoX = vistaAncho * 0.2;
            qreal alturaSuelo = sceneH * 0.85;
            qreal posEnemigoY = alturaSuelo - sceneH * 0.08;
            enemigoAtras->setPos(posEnemigoX, posEnemigoY);
        }

        // Centrar vista
        if (escena && !escena->views().isEmpty() && jugador) {
            QGraphicsView* vista = escena->views().first();
            vista->centerOn(jugador);
        }

        qDebug() << "✅ Nivel reiniciado correctamente";
    }
    // Tecla ESC para volver al menú
    else if (key == Qt::Key_Escape) {
        qDebug() << "🚪 Volver al menú...";
        // Aquí puedes emitir una señal para volver al menú principal
        // emit volverAlMenu();
    }
}
