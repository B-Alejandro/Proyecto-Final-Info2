// ============ nivel1.cpp (Modificado con PanelInfo) ============
#include "nivel1.h"
#include "juego.h"
#include "jugador.h"
#include "enemigo.h"
#include "tanque.h"
#include "obstaculo.h"
#include "fisica.h"
#include "persona.h"
#include "GameOverScreen.h"
// #include "hudnivel1.h" // ¡ELIMINADO!
#include "panelinfo.h" // ¡NUEVO!
#include "victoriascreen.h"
#include <QRandomGenerator>
#include <QDebug>
#include <QBrush>
#include <QPixmap>
#include <QTimer>
#include <QMap> // Necesario para PanelInfo::actualizar

Nivel1::Nivel1(Juego* juego, QObject* parent)
    : NivelBase(juego, 1, parent)
    , pantallaGameOver(nullptr)
    , pantallaVictoria(nullptr)
    // , hud(nullptr) // ¡ELIMINADO!
    , infoPanel(nullptr) // ¡NUEVO!
    , juegoEnPausa(false)
    , nivelGanado(false)
    , puntosActuales(80)
    , puntosObjetivo(100)
    , vidaJugadorActual(5) // ¡INICIALIZAR!
    , scrollOffset(0)
{
    vistaAncho = juego->getVistaAncho();
    vistaAlto = juego->getVistaAlto();

    int ancho = vistaAlto*3;
    int alto = vistaAncho;

    crearEscena(ancho, alto);
}

// ... (configurarEscena sin cambios) ...

void Nivel1::configurarEscena()
{
    if (!escena) return;

    // Llama a la base para la limpieza inicial y configuración básica
    NivelBase::configurarEscena();

    // Cargar la imagen del fondo
    QPixmap backgroundPixmap(":/Recursos/Backgrounds/Calle.png");

    if (backgroundPixmap.isNull()) {
        qDebug() << "Error: No se pudo cargar el fondo Calle.png";
        // Usa un color base si no se carga la imagen
        escena->setBackgroundBrush(QBrush(Qt::cyan));
        return;
    }

    // Usar la imagen como patrón para el pincel de fondo
    QBrush backgroundBrush(backgroundPixmap);
    escena->setBackgroundBrush(backgroundBrush);
}
// ------------------------------------------------------------------------
// En la función configurarNivel(), después de crear el infoPanel:
void Nivel1::configurarNivel()
{
    qreal posJugadorX = vistaAncho * 0.4;
    qreal alturaSuelo = sceneH * 0.4;

    qreal anchoJugador = vistaAncho * 0.06;
    qreal altoJugador = vistaAncho * 0.06;
    qreal posJugadorY = alturaSuelo - altoJugador;

    // Crear jugador con movimiento RECTILINEO
    jugador = new Jugador(anchoJugador, altoJugador, sceneW, sceneH, TipoMovimiento::RECTILINEO);
    jugador->setPos(posJugadorX, posJugadorY);
    jugador->setVida(5);
    vidaJugadorActual = 5;

    // Cargar sprites del jugador
    if (jugador) {
        jugador->cargarSpritesNivel1();
        jugador->setAnimacion(EstadoAnimacion::IDLE);
        jugador->reanudarAnimacion();
    }

    // Conectar señales del jugador
    connect(jugador, &Persona::murioPersona, this, &Nivel1::onJugadorMurio);
    connect(jugador, &Persona::vidaCambiada, this, &Nivel1::onJugadorDaniado);

    escena->addItem(jugador);

    // Configurar foco para el jugador
    jugador->setFlag(QGraphicsItem::ItemIsFocusable);
    jugador->setFocus();

    // Crear HUD con PanelInfo
    if (escena) {
        infoPanel = new PanelInfo(vistaAncho, nullptr);
        escena->addItem(infoPanel);

        // 🔥 CRÍTICO: Conectar las señales del PanelInfo
        connect(infoPanel, &PanelInfo::pausaPresionada,
                this, &Nivel1::manejarPausa);
        connect(infoPanel, &PanelInfo::accionPausaSeleccionada,
                this, &Nivel1::manejarAccionPausa);

        qDebug() << "✅ Señales de PanelInfo conectadas correctamente";

        actualizarInfoPanel(); // Inicializar el contenido

        pantallaGameOver = new GameOverScreen(escena, this);
        pantallaVictoria = new VictoriaScreen(escena, this);
    }

    qDebug() << "Jugador creado - Tamaño:" << anchoJugador << "x" << altoJugador;
}

// ... (crearEnemigos, spawnearOleada, gameTick, cleanupOffscreen sin cambios sustanciales) ...
void Nivel1::crearEnemigos()
{
    QTimer* gameTimer = new QTimer(this);
    connect(gameTimer, &QTimer::timeout, this, &Nivel1::gameTick);
    gameTimer->start(16);

    spawnearOleada();
}


    // ============ nivel1.cpp - spawnearOleada() COMPLETA CORREGIDA ============

    void Nivel1::spawnearOleada()
{
    int size = vistaAlto * 0.1;
    if (!escena) return;

    int tipoOleada = QRandomGenerator::global()->bounded(3);

    // 🔥 CONSTANTE CRÍTICA: Posición Y inicial para TODOS los elementos
    // Debe estar DEBAJO del PanelInfo (altura 50px)
    const qreal POS_Y_SPAWN = 70; // 50 (panel) + 20 (margen seguridad)

    /*
     * ========================================================================
     * PRIMER TIPO DE OLEADAS - ENEMIGOS BÁSICOS
     * menos de 30 puntos
     * ========================================================================
     */
    if(puntosActuales < 30){
        if(tipoOleada == 0) {
            // Oleada simple de enemigos en línea horizontal
            int posX = 50;
            for (int i = 0; i < 8; ++i) {
                posX += 150;
                Enemigo* e1 = new Enemigo(size, size, sceneW, sceneH, TipoMovimiento::RECTILINEO, 1);
                e1->setPos(posX, POS_Y_SPAWN); // ✅ CORREGIDO

                e1->cargarSpritesNivel1();
                e1->setAnimacion(EstadoAnimacion::IDLE);
                e1->reanudarAnimacion();

                listaEnemigos.append(e1);
                escena->addItem(e1);
                connect(e1, &Persona::died, this, &Nivel1::onEnemyDied);
            }
            qDebug() << "🎯 Oleada 1-0: 8 enemigos en línea spawneados en Y:" << POS_Y_SPAWN;
        }
        else if(tipoOleada == 1) {
            // Oleada en formación V
            int posX = 300;
            double posY[8] = {
                POS_Y_SPAWN,
                POS_Y_SPAWN + vistaAlto*0.05,
                POS_Y_SPAWN + vistaAlto*0.1,
                POS_Y_SPAWN + vistaAlto*0.15,
                POS_Y_SPAWN + vistaAlto*0.15,
                POS_Y_SPAWN + vistaAlto*0.1,
                POS_Y_SPAWN + vistaAlto*0.05,
                POS_Y_SPAWN
            };

            for (int i = 0; i < 8; ++i) {
                posX += 100;
                Enemigo* e1 = new Enemigo(size, size, sceneW, sceneH, TipoMovimiento::RECTILINEO, 1);
                e1->setPos(posX, posY[i]); // ✅ CORREGIDO

                e1->cargarSpritesNivel1();
                e1->setAnimacion(EstadoAnimacion::IDLE);
                e1->reanudarAnimacion();

                listaEnemigos.append(e1);
                escena->addItem(e1);
                connect(e1, &Persona::died, this, &Nivel1::onEnemyDied);
            }
            qDebug() << "🎯 Oleada 1-1: 8 enemigos en V spawneados";
        }
        else if(tipoOleada == 2) {
            // Oleada diagonal escalonada
            int posX = 100;
            double posY[8] = {
                POS_Y_SPAWN,
                POS_Y_SPAWN + vistaAlto*0.05,
                POS_Y_SPAWN + vistaAlto*0.1,
                POS_Y_SPAWN + vistaAlto*0.15,
                POS_Y_SPAWN + vistaAlto*0.15,
                POS_Y_SPAWN + vistaAlto*0.1,
                POS_Y_SPAWN + vistaAlto*0.05,
                POS_Y_SPAWN
            };

            for (int i = 0; i < 8; ++i) {
                posX += 100;
                Enemigo* e1 = new Enemigo(size, size, sceneW, sceneH, TipoMovimiento::RECTILINEO, 1);
                e1->setPos(posX, posY[i]); // ✅ CORREGIDO

                e1->cargarSpritesNivel1();
                e1->setAnimacion(EstadoAnimacion::IDLE);
                e1->reanudarAnimacion();

                listaEnemigos.append(e1);
                escena->addItem(e1);
                connect(e1, &Persona::died, this, &Nivel1::onEnemyDied);
            }
            qDebug() << "🎯 Oleada 1-2: 8 enemigos diagonales spawneados";
        }
    }

    /*
     * ========================================================================
     * SEGUNDO TIPO DE OLEADAS - ENEMIGOS + OBSTÁCULOS
     * más de 30 y menos de 60 puntos
     * ========================================================================
     */
    else if(puntosActuales < 60 && puntosActuales >= 30){
        if(tipoOleada == 0) {
            // Oleada con obstáculos móviles y enemigos detrás
            int sizeObs = vistaAlto * 0.12;
            int posX = 300;

            for (int i = 0; i < 4; ++i) {
                posX += 150;

                // Crear obstáculo móvil
                Obstaculo* obs = new Obstaculo(posX, POS_Y_SPAWN, sizeObs, sizeObs, // ✅ CORREGIDO
                                               QColor(150, 75, 0), true);
                obs->setMovil(true);
                obs->setVida(2);
                obs->setVelocidad(4.0);
                obs->setDanioColision(20);
                obs->setBorderColor(Qt::black, 3);

                listaObstaculosMoviles.append(obs);
                escena->addItem(obs);
                connect(obs, &Obstaculo::obstaculoMuerto, this, &Nivel1::onObstaculoDestruido);

                // Crear enemigo detrás del obstáculo
                Enemigo* e1 = new Enemigo(size, size, sceneW, sceneH, TipoMovimiento::RECTILINEO, 1);
                e1->setPos(posX, POS_Y_SPAWN); // ✅ CORREGIDO

                e1->cargarSpritesNivel1();
                e1->setAnimacion(EstadoAnimacion::IDLE);
                e1->reanudarAnimacion();

                listaEnemigos.append(e1);
                escena->addItem(e1);
                connect(e1, &Persona::died, this, &Nivel1::onEnemyDied);
            }
            qDebug() << "🎯 Oleada 2-0: 4 obstáculos + 4 enemigos spawneados en Y:" << POS_Y_SPAWN;
        }
        else if(tipoOleada == 1) {
            // Línea de obstáculos
            int posX = 50;
            int sizeObs = vistaAlto * 0.12;

            for (int i = 0; i < 12; ++i) {
                posX += 100;
                Obstaculo* obs = new Obstaculo(posX, POS_Y_SPAWN, sizeObs, sizeObs, // ✅ CORREGIDO
                                               QColor(150, 75, 0), true);
                obs->setMovil(true);
                obs->setVida(2);
                obs->setVelocidad(4.0);
                obs->setDanioColision(20);
                obs->setTextura(":/Recursos/Objects/Obstaculos2.png", false);

                listaObstaculosMoviles.append(obs);
                escena->addItem(obs);
                connect(obs, &Obstaculo::obstaculoMuerto, this, &Nivel1::onObstaculoDestruido);
            }
            qDebug() << "🎯 Oleada 2-1: 12 obstáculos en línea spawneados";
        }
        else if(tipoOleada == 2) {
            // Obstáculos con hueco en el centro + enemigos en el hueco
            int posX = 50;
            int sizeObs = vistaAlto * 0.12;

            for (int i = 0; i < 12; ++i) {
                posX += 100;

                if(i < 4 || i > 8){
                    // Crear obstáculos en los extremos
                    Obstaculo* obs = new Obstaculo(posX, POS_Y_SPAWN, sizeObs, sizeObs, // ✅ CORREGIDO
                                                   QColor(150, 75, 0), true);
                    obs->setMovil(true);
                    obs->setVida(2);
                    obs->setVelocidad(4.0);
                    obs->setDanioColision(20);
                    obs->setBorderColor(Qt::black, 3);

                    listaObstaculosMoviles.append(obs);
                    escena->addItem(obs);
                    connect(obs, &Obstaculo::obstaculoMuerto, this, &Nivel1::onObstaculoDestruido);
                }
                else {
                    // Crear enemigos en el centro
                    Enemigo* e1 = new Enemigo(size, size, sceneW, sceneH, TipoMovimiento::RECTILINEO, 1);
                    e1->setPos(posX, POS_Y_SPAWN); // ✅ CORREGIDO

                    e1->cargarSpritesNivel1();
                    e1->setAnimacion(EstadoAnimacion::IDLE);
                    e1->reanudarAnimacion();

                    listaEnemigos.append(e1);
                    escena->addItem(e1);
                    connect(e1, &Persona::died, this, &Nivel1::onEnemyDied);
                }
            }
            qDebug() << "🎯 Oleada 2-2: Obstáculos + enemigos centrales spawneados";
        }
    }

    /*
     * ========================================================================
     * TERCER TIPO DE OLEADAS - TANQUES + OBSTÁCULOS AVANZADOS
     * más de 60 puntos
     * ========================================================================
     */
    if(puntosActuales < 100 && puntosActuales >= 60){
        if(tipoOleada == 0) {
            // Dos tanques (izquierdo y derecho)
            int sizeTanque = vistaAlto * 0.15;

            qreal posXIzq = 50;
            Tanque* tanqueIzq = new Tanque(sizeTanque, sizeTanque, sceneW, sceneH, -1);
            tanqueIzq->setPos(posXIzq, POS_Y_SPAWN); // ✅ CORREGIDO
            tanqueIzq->setTiempoDisparo(3000);
            tanqueIzq->setZValue(100); // ✅ Debajo del panel visualmente

            // ✅ Cargar sprite del tanque (elige el método que prefieras)
            tanqueIzq->cargarSpriteTanque();
            // tanqueIzq->cargarSpriteComoPixmap(); // Alternativa

            listaTanques.append(tanqueIzq);
            escena->addItem(tanqueIzq);
            connect(tanqueIzq, &Persona::died, this, &Nivel1::onTankDied);

            qreal posXDer = 1400;
            Tanque* tanqueDer = new Tanque(sizeTanque, sizeTanque, sceneW, sceneH, 1);
            tanqueDer->setPos(posXDer, POS_Y_SPAWN); // ✅ CORREGIDO
            tanqueDer->setTiempoDisparo(3000);
            tanqueDer->setZValue(100);

            tanqueDer->cargarSpriteTanque();

            listaTanques.append(tanqueDer);
            escena->addItem(tanqueDer);
            connect(tanqueDer, &Persona::died, this, &Nivel1::onTankDied);

            qDebug() << "🎯 Oleada 3-0: 2 tanques spawneados en Y:" << POS_Y_SPAWN;
        }
        else if(tipoOleada == 1) {
            // Tanque izquierdo + línea de obstáculos
            int sizeTanque = vistaAlto * 0.15;

            qreal posXIzq = 50;
            Tanque* tanqueIzq = new Tanque(sizeTanque, sizeTanque, sceneW, sceneH, -1);
            tanqueIzq->setPos(posXIzq, POS_Y_SPAWN); // ✅ CORREGIDO
            tanqueIzq->setTiempoDisparo(3000);
            tanqueIzq->setZValue(100);

            tanqueIzq->cargarSpriteTanque();

            listaTanques.append(tanqueIzq);
            escena->addItem(tanqueIzq);
            connect(tanqueIzq, &Persona::died, this, &Nivel1::onTankDied);

            int posX = 400;
            int sizeObs = vistaAlto * 0.12;

            for (int i = 0; i < 8; ++i) {
                posX += 100;
                Obstaculo* obs = new Obstaculo(posX, POS_Y_SPAWN, sizeObs, sizeObs, // ✅ CORREGIDO
                                               QColor(150, 75, 0), true);
                obs->setMovil(true);
                obs->setVida(2);
                obs->setVelocidad(4.0);
                obs->setDanioColision(20);
                obs->setBorderColor(Qt::black, 3);

                listaObstaculosMoviles.append(obs);
                escena->addItem(obs);
                connect(obs, &Obstaculo::obstaculoMuerto, this, &Nivel1::onObstaculoDestruido);
            }
            qDebug() << "🎯 Oleada 3-1: 1 tanque + 8 obstáculos spawneados";
        }
        else if(tipoOleada == 2) {
            // Tanque derecho + línea de obstáculos
            int sizeTanque = vistaAlto * 0.15;

            qreal posXDer = 1300;
            Tanque* tanqueDer = new Tanque(sizeTanque, sizeTanque, sceneW, sceneH, 1);
            tanqueDer->setPos(posXDer, POS_Y_SPAWN); // ✅ CORREGIDO
            tanqueDer->setTiempoDisparo(3000);
            tanqueDer->setZValue(100);

            tanqueDer->cargarSpriteTanque();

            listaTanques.append(tanqueDer);
            escena->addItem(tanqueDer);
            connect(tanqueDer, &Persona::died, this, &Nivel1::onTankDied);

            int posX = 50;
            int sizeObs = vistaAlto * 0.12;

            for (int i = 0; i < 8; ++i) {
                posX += 100;
                Obstaculo* obs = new Obstaculo(posX, POS_Y_SPAWN, sizeObs, sizeObs, // ✅ CORREGIDO
                                               QColor(150, 75, 0), true);
                obs->setMovil(true);
                obs->setVida(2);
                obs->setVelocidad(4.0);
                obs->setDanioColision(20);
                obs->setBorderColor(Qt::black, 3);

                listaObstaculosMoviles.append(obs);
                escena->addItem(obs);
                connect(obs, &Obstaculo::obstaculoMuerto, this, &Nivel1::onObstaculoDestruido);
            }
            qDebug() << "🎯 Oleada 3-2: 1 tanque + 8 obstáculos spawneados";
        }
    }

    qDebug() << "========================================";
    qDebug() << "📊 RESUMEN DE OLEADA:";
    qDebug() << "  - Enemigos activos:" << listaEnemigos.size();
    qDebug() << "  - Tanques activos:" << listaTanques.size();
    qDebug() << "  - Obstáculos activos:" << listaObstaculosMoviles.size();
    qDebug() << "  - Puntos actuales:" << puntosActuales << "/" << puntosObjetivo;
    qDebug() << "========================================";
}
void Nivel1::gameTick()
{
    // 🔥 CRÍTICO: No actualizar nada si el juego está pausado o ganado
    if (juegoEnPausa || nivelGanado) {
        return;
    }

    if (escena) {
        escena->advance();
    }

    // Actualizar obstáculos móviles
    for (Obstaculo* obs : listaObstaculosMoviles) {
        if (obs) {
            obs->actualizar(sceneH);
        }
    }

    revisarColision();
    cleanupOffscreen();
    verificarEstadoJugador();
    verificarVictoria();
}

void Nivel1::cleanupOffscreen()
{
    if (!escena) return;

    qreal cleanupMargin = 100;
    qreal cleanupMarginT = 200;
    bool removidos = false;

    // Limpiar enemigos
    auto copiaEnemigos = listaEnemigos;
    for (Enemigo* e : copiaEnemigos) {
        if (!e) continue;
        qreal ey = e->scenePos().y();

        if (ey > sceneH - cleanupMargin) {
            qDebug() << "Enemigo fuera de pantalla en y:" << ey << "sceneH:" << sceneH;

            listaEnemigos.removeOne(e);
            // enemigos.removeOne(e); // 'enemigos' no está en la lista de miembros, se asume que fue un error tipográfico
            escena->removeItem(e);
            e->deleteLater();
            removidos = true;
        }
    }

    // Limpiar tanques
    auto copiaTanques = listaTanques;
    for (Tanque* t : copiaTanques) {
        if (!t) continue;
        qreal ty = t->scenePos().y();
        qreal alturaT = t->boundingRect().height();

        if (ty > sceneH - cleanupMarginT) {
            listaTanques.removeOne(t);
            escena->removeItem(t);
            t->deleteLater();
            removidos = true;
        }
    }

    // NUEVO: Limpiar obstáculos móviles
    auto copiaObstaculos = listaObstaculosMoviles;
    for (Obstaculo* obs : copiaObstaculos) {
        if (!obs) continue;
        qreal oy = obs->scenePos().y();

        if (oy > sceneH - cleanupMargin) {
            listaObstaculosMoviles.removeOne(obs);
            escena->removeItem(obs);
            obs->deleteLater();
            removidos = true;
        }
    }

    if (removidos && listaEnemigos.isEmpty() && listaTanques.isEmpty() && listaObstaculosMoviles.isEmpty()) {
        QTimer::singleShot(spawnDelayMs, this, [this]() {
            this->spawnearOleada();
        });
    }
}

void Nivel1::revisarColision()
{
    if (!jugador) return;

    // Colisión con enemigos
    auto copiaEnemigos = listaEnemigos;
    for (Enemigo* e : copiaEnemigos) {
        if (!e) continue;
        if (jugador->collidesWithItem(e, Qt::IntersectsItemShape)) {
            colisionDetectada(e);
        }
    }

    // Colisión con tanques
    auto copiaTanques = listaTanques;
    for (Tanque* t : copiaTanques) {
        if (!t) continue;
        if (jugador->collidesWithItem(t, Qt::IntersectsItemShape)) {
            colisionTanqueDetectada(t);
        }
    }

    // NUEVO: Colisión con obstáculos móviles
    auto copiaObstaculos = listaObstaculosMoviles;
    for (Obstaculo* obs : copiaObstaculos) {
        if (!obs) continue;
        if (jugador->collidesWithItem(obs, Qt::IntersectsItemShape)) {
            colisionObstaculoDetectada(obs);
        }
    }
}

void Nivel1::colisionDetectada(Enemigo* e)
{
    if (!e || !jugador) return;

    jugador->recibirDanio(1);

    listaEnemigos.removeOne(e);
    // enemigos.removeOne(e); // Asumido error tipográfico
    escena->removeItem(e);
    e->deleteLater();
}

void Nivel1::colisionTanqueDetectada(Tanque* t)
{
    if (!t || !jugador) return;

    jugador->recibirDanio(2);

    listaTanques.removeOne(t);
    escena->removeItem(t);
    t->deleteLater();
}

void Nivel1::colisionObstaculoDetectada(Obstaculo* obs)
{
    if (!obs || !jugador) return;

    jugador->recibirDanio(2);

    listaObstaculosMoviles.removeOne(obs);
    escena->removeItem(obs);
    obs->deleteLater();
}

void Nivel1::crearObstaculos()
{
    // Sin suelo visible en este nivel
    int sueloAltura = 0;
    Obstaculo* suelo = new Obstaculo(0, sceneH - sueloAltura, sceneW, sueloAltura,
                                     QColor(139, 69, 19), false);
    suelo->setBorderColor(Qt::black, 2);
    // obstaculos.append(suelo); // 'obstaculos' no está en la lista de miembros, se omite
    escena->addItem(suelo);
}

void Nivel1::verificarEstadoJugador()
{
    if (!jugador) return;

    if (!jugador->estaVivo() && !juegoEnPausa) {
        juegoEnPausa = true;

        QTimer::singleShot(500, this, [this]() {
            if (pantallaGameOver) {
                pantallaGameOver->mostrar();
            }
        });
    }
}

void Nivel1::agregarPuntos(int cantidad)
{
    puntosActuales += cantidad;
    actualizarInfoPanel(); // Actualizar panel después de sumar puntos

    qDebug() << "Puntos:" << puntosActuales << "/" << puntosObjetivo;
}

void Nivel1::verificarVictoria()
{
    if (nivelGanado) return;

    if (puntosActuales >= puntosObjetivo) {
        nivelGanado = true;
        juegoEnPausa = true;

        qDebug() << "¡NIVEL GANADO!";

        QTimer::singleShot(500, this, [this]() {
            if (pantallaVictoria) {
                pantallaVictoria->mostrar(puntosActuales);
            }
        });
    }
}

/**
 * @brief Construye el QMap con la información del nivel y la pasa al PanelInfo.
 */
void Nivel1::actualizarInfoPanel()
{
    if (!infoPanel) return;

    QMap<QString, QString> datos;
    datos["PUNTOS"] = QString::number(puntosActuales) + "/" + QString::number(puntosObjetivo);
    datos["VIDA"] = QString::number(vidaJugadorActual);
    datos["NIVEL"] = "1";

    infoPanel->actualizar(datos);
}



void Nivel1::actualizar()
{
    // 🔥 CRÍTICO: No actualizar el scroll si está en pausa
    if (juegoEnPausa || nivelGanado) {
        return;
    }

    // Scroll Infinito Vertical hacia abajo
    if (escena) {
        qreal scrollSpeed = 5.0;

        scrollOffset += scrollSpeed;

        QPixmap backgroundPixmap(":/Recursos/Backgrounds/Calle.png");
        if (backgroundPixmap.isNull()) {
            return;
        }

        qreal textureHeight = backgroundPixmap.height();

        if (scrollOffset >= textureHeight) {
            scrollOffset -= textureHeight;
        }

        QBrush backgroundBrush(backgroundPixmap);
        QTransform transform;
        transform.translate(0, -scrollOffset);
        backgroundBrush.setTransform(transform);

        escena->setBackgroundBrush(backgroundBrush);
    }
}

void Nivel1::onEnemyDied(Persona* p)
{
    Enemigo* e = dynamic_cast<Enemigo*>(p);
    if (!e) return;

    listaEnemigos.removeOne(e);
    agregarPuntos(5);
}

void Nivel1::onTankDied(Persona* p)
{
    Tanque* t = dynamic_cast<Tanque*>(p);
    if (!t) return;

    listaTanques.removeOne(t);
    agregarPuntos(10);
}

void Nivel1::onObstaculoDestruido(Obstaculo* obs)
{
    if (!obs) return;

    listaObstaculosMoviles.removeOne(obs);
    agregarPuntos(3);

    escena->removeItem(obs);
    obs->deleteLater();
}

void Nivel1::onJugadorDaniado(int vidaActual, int vidaMax)
{
    vidaJugadorActual = vidaActual; // Actualizar variable de tracking
    actualizarInfoPanel(); // Actualizar panel después de daño

    // Código de depuración
    // if (infoPanel) {
    //     QMap<QString, QString> datos;
    //     datos["VIDA"] = QString::number(vidaActual);
    //     infoPanel->actualizar(datos);
    // }
}

void Nivel1::onJugadorMurio()
{
    juegoEnPausa = true;
}

// ============ nivel1.cpp (manejarTecla Corregido) ============

void Nivel1::manejarTecla(Qt::Key key)
{
    // 1. MANEJO DE PAUSA / GAME OVER / VICTORIA
    // Si el juego está en pausa, solo se permiten acciones de sistema.
    if (juegoEnPausa) {

        // CRÍTICO: Corrección del error de compilación.
        // Se usa 'estaVisible()' en lugar de 'isVisible()' para GameOverScreen/VictoriaScreen.
        if (pantallaGameOver->estaVisible() || pantallaVictoria->estaVisible()) {
            if (key == Qt::Key_R) {
                reiniciarNivel();
                return;
            }
            if (key == Qt::Key_Escape) {
                // Emite la señal que la clase principal (Juego) debe conectar
                emit volverAMenuPrincipal();
                return;
            }
        }

        // Si solo está en pausa con el menú visible, el manejo se hace por botones de clic,
        // no por teclas.
        return;
    }

    // 2. MANEJO DE MOVIMIENTO DEL JUGADOR (JUEGO CORRIENDO)
    if (!jugador) {
        return;
    }

    switch (key) {
    case Qt::Key_A:
    case Qt::Key_Left:
        // ⚠️ Temporalmente COMENTADO para evitar ERROR DE COMPILACIÓN.
        // Debe descomentarse una vez que Jugador::moverIzquierda() se declare en jugador.h.
        // jugador->moverIzquierda();
        qDebug() << "⬅️ Solicitud de movimiento a la izquierda (Pendiente de implementación en Jugador).";
        break;

    case Qt::Key_D:
    case Qt::Key_Right:
        // ⚠️ Temporalmente COMENTADO para evitar ERROR DE COMPILACIÓN.
        // jugador->moverDerecha();
        qDebug() << "➡️ Solicitud de movimiento a la derecha (Pendiente de implementación en Jugador).";
        break;

    case Qt::Key_Space:
        // ⚠️ Temporalmente COMENTADO para evitar ERROR DE COMPILACIÓN.
        // jugador->atacar();
        qDebug() << "💥 Solicitud de ataque (Pendiente de implementación en Jugador).";
        break;

    case Qt::Key_Escape:
        // Si el juego está corriendo, la tecla ESC pausa el juego.
        manejarPausa();
        qDebug() << "⏸️ Tecla ESC presionada, pausando el juego.";
        break;

    default:
        break;
    }
}
// ============ nivel1.cpp (NUEVAS DEFINICIONES DE SLOTS) ============

void Nivel1::manejarPausa()
{
    // Alternar el estado de pausa
    if (!nivelGanado && jugador && jugador->estaVivo()) {
        juegoEnPausa = !juegoEnPausa;

        if (juegoEnPausa) {
            qDebug() << " Juego PAUSADO";
            // Pausar animaciones del jugador
            if (jugador) {
                jugador->pausarAnimacion();
            }

            // Pausar animaciones de enemigos
            for (Enemigo* e : listaEnemigos) {
                if (e) e->pausarAnimacion();
            }

            // Mostrar menú de pausa
            if (infoPanel) {
                infoPanel->setMenuPausaVisible(true);
                infoPanel->setBotonPausaVisible(false); // Ocultar botón mientras está el menú
            }
        } else {
            qDebug() << "🟢 Juego REANUDADO";
            // Reanudar animaciones del jugador
            if (jugador) {
                jugador->reanudarAnimacion();
            }

            // Reanudar animaciones de enemigos
            for (Enemigo* e : listaEnemigos) {
                if (e) e->reanudarAnimacion();
            }

            // Ocultar menú de pausa
            if (infoPanel) {
                infoPanel->setMenuPausaVisible(false);
                infoPanel->setBotonPausaVisible(true); // Mostrar botón nuevamente
            }
        }
    }
}

void Nivel1::manejarAccionPausa(PanelInfo::AccionPausa accion)
{
    qDebug() << "🎮 Acción de pausa recibida:" << static_cast<int>(accion);

    switch (accion) {
    case PanelInfo::AccionPausa::ReiniciarNivel:
        qDebug() << "♻️ Reiniciando nivel...";
        juegoEnPausa = false;
        if (infoPanel) {
            infoPanel->setMenuPausaVisible(false);
            infoPanel->setBotonPausaVisible(true);
        }
        reiniciarNivel();
        break;

    case PanelInfo::AccionPausa::Reanudar:
        qDebug() << "▶️ Reanudando juego...";
        juegoEnPausa = false;

        // Reanudar animaciones
        if (jugador) {
            jugador->reanudarAnimacion();
        }
        for (Enemigo* e : listaEnemigos) {
            if (e) e->reanudarAnimacion();
        }

        if (infoPanel) {
            infoPanel->setMenuPausaVisible(false);
            infoPanel->setBotonPausaVisible(true);
        }
        break;

    case PanelInfo::AccionPausa::VolverMenu:
        qDebug() << "🏠 Volviendo al menú principal...";
        juegoEnPausa = false;
        if (infoPanel) {
            infoPanel->setMenuPausaVisible(false);
            infoPanel->setBotonPausaVisible(true);
        }
        // Emitir señal para que Juego maneje el cambio de escena
        emit volverAMenuPrincipal();
        break;
    }
}

// ============ nivel1.cpp (Definición de reiniciarNivel) ============
void Nivel1::reiniciarNivel()
{
    qDebug() << "♻️ Reiniciando Nivel 1...";

    // 1. Resetear estados
    juegoEnPausa = false;
    nivelGanado = false;
    puntosActuales = 0;
    vidaJugadorActual = 5;
    scrollOffset = 0;

    // 2. Limpiar enemigos
    for (Enemigo* e : listaEnemigos) {
        if (e && escena) {
            escena->removeItem(e);
            e->deleteLater();
        }
    }
    listaEnemigos.clear();

    // 3. Limpiar tanques
    for (Tanque* t : listaTanques) {
        if (t && escena) {
            escena->removeItem(t);
            t->deleteLater();
        }
    }
    listaTanques.clear();

    // 4. Limpiar obstáculos móviles
    for (Obstaculo* obs : listaObstaculosMoviles) {
        if (obs && escena) {
            escena->removeItem(obs);
            obs->deleteLater();
        }
    }
    listaObstaculosMoviles.clear();

    // 5. Recrear jugador
    if (jugador && escena) {
        escena->removeItem(jugador);
        jugador->deleteLater();
        jugador = nullptr;
    }

    qreal posJugadorX = vistaAncho * 0.4;
    qreal alturaSuelo = sceneH * 0.4;
    qreal anchoJugador = vistaAncho * 0.06;
    qreal altoJugador = vistaAncho * 0.06;
    qreal posJugadorY = alturaSuelo - altoJugador;

    jugador = new Jugador(anchoJugador, altoJugador, sceneW, sceneH, TipoMovimiento::RECTILINEO);
    jugador->setPos(posJugadorX, posJugadorY);
    jugador->setVida(5);

    // Cargar sprites del jugador
    if (jugador) {
        jugador->cargarSpritesNivel1();
        jugador->setAnimacion(EstadoAnimacion::IDLE);
        jugador->reanudarAnimacion();
    }

    // Reconectar señales del jugador
    connect(jugador, &Persona::murioPersona, this, &Nivel1::onJugadorMurio);
    connect(jugador, &Persona::vidaCambiada, this, &Nivel1::onJugadorDaniado);

    escena->addItem(jugador);

    // Asegurar foco después del reinicio
    jugador->setFlag(QGraphicsItem::ItemIsFocusable);
    jugador->setFocus();

    // 6. Reiniciar fondo
    configurarEscena();

    // 7. Actualizar HUD
    if (infoPanel) {
        actualizarInfoPanel();
        infoPanel->setMenuPausaVisible(false);
        infoPanel->setBotonPausaVisible(true);
    }

    // 8. Ocultar pantallas
    if (pantallaGameOver) {
        pantallaGameOver->ocultar();
    }
    if (pantallaVictoria) {
        pantallaVictoria->ocultar();
    }

    // 9. Spawnear primera oleada
    spawnearOleada();

    qDebug() << "✅ Nivel reiniciado correctamente";
}
