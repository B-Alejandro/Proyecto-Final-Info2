#include "nivel1.h"
#include "juego.h"
#include "jugador.h"
#include "enemigo.h"
#include "tanque.h"
#include "obstaculo.h"
#include "fisica.h"
#include "persona.h"
#include "GameOverScreen.h"
#include "hudnivel1.h"
#include "victoriascreen.h"
#include <QRandomGenerator>

Nivel1::Nivel1(Juego* juego, QObject* parent)
    : NivelBase(juego, 1, parent)
    , pantallaGameOver(nullptr)
    , pantallaVictoria(nullptr)
    , hud(nullptr)
    , juegoEnPausa(false)
    , nivelGanado(false)
    , puntosActuales(30)
    , puntosObjetivo(100)  // 50 puntos para ganar
{
    vistaAncho = juego->getVistaAncho();
    vistaAlto = juego->getVistaAlto();

    int ancho = vistaAlto*3;
    int alto = vistaAncho;

    crearEscena(ancho, alto);
}

void Nivel1::configurarNivel()
{
    qreal posJugadorX = vistaAncho * 0.4;
    qreal alturaSuelo = sceneH * 0.4;

    // *** REDUCIDO: Jugador más pequeño ***
    qreal anchoJugador = vistaAncho * 0.06;  // Era 0.1, ahora 0.06 (40% más pequeño)
    qreal altoJugador = vistaAncho * 0.06;   // Era 0.1, ahora 0.06

    qreal posJugadorY = alturaSuelo - altoJugador;

    // Crear jugador con nuevo tamaño más pequeño
    jugador = new Jugador(anchoJugador, altoJugador, sceneW, sceneH, TipoMovimiento::RECTILINEO);
    jugador->setPos(posJugadorX, posJugadorY);
    jugador->setVida(5);

    // Conectar señales
    connect(jugador, &Persona::murioPersona, this, &Nivel1::onJugadorMurio);
    connect(jugador, &Persona::vidaCambiada, this, &Nivel1::onJugadorDaniado);

    escena->addItem(jugador);

    // Crear HUD
    if (escena) {
        hud = new HUDNivel1(escena, this);
        hud->actualizarVidas(5);
        hud->actualizarPuntuacion(0, puntosObjetivo);

        pantallaGameOver = new GameOverScreen(escena, this);
        pantallaVictoria = new VictoriaScreen(escena, this);
    }

    qDebug() << "Jugador creado - Tamaño:" << anchoJugador << "x" << altoJugador;
}

void Nivel1::crearEnemigos()
{
    QTimer* gameTimer = new QTimer(this);
    connect(gameTimer, &QTimer::timeout, this, &Nivel1::gameTick);
    gameTimer->start(16);

    spawnearOleada();
}

void Nivel1::spawnearOleada()
{
    int size = vistaAlto * 0.1;
    if (!escena) return;

    //int tipoOleada = QRandomGenerator::global()->bounded(3);  // 0-2
    int tipoOleada = 1;
    /*
     * PRIMER TIPO DE OLEADAS
     * menos de 30 puntos
     *
     */
    if(puntosActuales < 30){
        if(tipoOleada == 0) {
            // Oleada simple de enemigos
            int posX = 50;
            for (int i = 0; i < 8; ++i) {
                posX += 150;
                Enemigo* e1 = new Enemigo(size, size, sceneW, sceneH, TipoMovimiento::RECTILINEO, 1);
                e1->setPos(posX, sceneH * 0);
                listaEnemigos.append(e1);
                escena->addItem(e1);
                connect(e1, &Persona::died, this, &Nivel1::onEnemyDied);
            }
        }
        else if(tipoOleada == 1) {
            // Oleada en V
            int posX = 300;
            double posY[8] = {vistaAlto*0.1, vistaAlto*0.15, vistaAlto*0.2, vistaAlto*0.25,
                              vistaAlto*0.25, vistaAlto*0.2, vistaAlto*0.15, vistaAlto*0.1};
            for (int i = 0; i < 8; ++i) {
                posX += 100;
                Enemigo* e1 = new Enemigo(size, size, sceneW, sceneH, TipoMovimiento::RECTILINEO, 1);
                e1->setPos(posX, posY[i]);
                listaEnemigos.append(e1);
                escena->addItem(e1);
                connect(e1, &Persona::died, this, &Nivel1::onEnemyDied);
            }
        }
        else if(tipoOleada == 2) {
            // Oleada diagonal
            int posX = 100;
            double posY[8] = {sceneH*0.1, sceneH*0.15, sceneH*0.2, sceneH*0.25,
                              sceneH*0.25, sceneH*0.2, sceneH*0.15, sceneH*0.1};
            for (int i = 0; i < 8; ++i) {
                posX += 100;
                Enemigo* e1 = new Enemigo(size, size, sceneW, sceneH, TipoMovimiento::RECTILINEO, 1);
                e1->setPos(posX, posY[i]);
                listaEnemigos.append(e1);
                escena->addItem(e1);
                connect(e1, &Persona::died, this, &Nivel1::onEnemyDied);

            }
        }
    }

    /*
     * SEGUNDO TIPO DE OLEADAS
     * mas de 30 y menos de 60 puntos
     *
     */
    else if(puntosActuales < 60 && puntosActuales >= 30){
        if(tipoOleada == 0) {
            // Oleada con obstáculos móviles ***
            int sizeObs = vistaAlto * 0.12;
            int posX = 300;

            // Spawn 4 obstáculos móviles y 4 enemigos detras de dichos enemigos
            for (int i = 0; i < 4; ++i) {
                posX += 150;

                Obstaculo* obs = new Obstaculo(posX, sceneH * 0.15, sizeObs, sizeObs,
                                               QColor(150, 75, 0), true);  // Marrón, móvil
                obs->setMovil(true);
                obs->setVida(2);
                obs->setVelocidad(4.0);
                obs->setDanioColision(20);
                obs->setBorderColor(Qt::black, 3);

                listaObstaculosMoviles.append(obs);
                escena->addItem(obs);
                connect(obs, &Obstaculo::obstaculoMuerto, this, &Nivel1::onObstaculoDestruido);

                Enemigo* e1 = new Enemigo(size, size, sceneW, sceneH, TipoMovimiento::RECTILINEO, 1);
                e1->setPos(posX, sceneH * 0);
                listaEnemigos.append(e1);
                escena->addItem(e1);
                connect(e1, &Persona::died, this, &Nivel1::onEnemyDied);
            }
        }

        //Linea de obstaculos
        else if(tipoOleada == 1) {
            int posX = 50;
            int sizeObs = vistaAlto * 0.12;

            for (int i = 0; i < 12; ++i) {
                posX += 100;
                Obstaculo* obs = new Obstaculo(posX, sceneH * 0, sizeObs, sizeObs,
                                               QColor(150, 75, 0), true);  // Marrón, móvil
                obs->setMovil(true);
                obs->setVida(2);
                obs->setVelocidad(4.0);
                obs->setDanioColision(20);
                obs->setTextura(":/Recursos/Objects/Obstaculos2.png", false);

                listaObstaculosMoviles.append(obs);
                escena->addItem(obs);
                connect(obs, &Obstaculo::obstaculoMuerto, this, &Nivel1::onObstaculoDestruido);
            }
        }
        else if(tipoOleada == 2) {
            int posX = 50;
            int sizeObs = vistaAlto * 0.12;

            for (int i = 0; i < 12; ++i)
            {
                if(i < 4 || i > 8){
                    posX += 100;
                    Obstaculo* obs = new Obstaculo(posX, sceneH * 0, sizeObs, sizeObs,
                                                   QColor(150, 75, 0), true);  // Marrón, móvil
                    obs->setMovil(true);
                    obs->setVida(2);
                    obs->setVelocidad(4.0);
                    obs->setDanioColision(20);
                    obs->setBorderColor(Qt::black, 3);

                    listaObstaculosMoviles.append(obs);
                    escena->addItem(obs);
                    connect(obs, &Obstaculo::obstaculoMuerto, this, &Nivel1::onObstaculoDestruido);
                }
                else{
                    posX += 100;
                    Enemigo* e1 = new Enemigo(size, size, sceneW, sceneH, TipoMovimiento::RECTILINEO, 1);
                    e1->setPos(posX, sceneH * 0);
                    listaEnemigos.append(e1);
                    escena->addItem(e1);
                    connect(e1, &Persona::died, this, &Nivel1::onEnemyDied);
                }
            }
        }
    }

    /*
     * TERCER TIPO DE OLEADAS
     * mas de 60 puntos
     *
     */
    if(puntosActuales < 100 && puntosActuales >= 60){
        if(tipoOleada == 0) {
            // Oleada con tanques
            int sizeTanque = vistaAlto * 0.15;
            qreal margenLateral = 50;

            qreal posXIzq = 50;
            Tanque* tanqueIzq = new Tanque(sizeTanque, sizeTanque, sceneW, sceneH, -1); // -1 signifia lado izquierdo
            tanqueIzq->setPos(posXIzq, sceneH * 0);
            tanqueIzq->setTiempoDisparo(3000);
            listaTanques.append(tanqueIzq);
            escena->addItem(tanqueIzq);
            connect(tanqueIzq, &Persona::died, this, &Nivel1::onTankDied);

            qreal posXDer = 1400;
            Tanque* tanqueDer = new Tanque(sizeTanque, sizeTanque, sceneW, sceneH, 1); // 1 significa lado derecho
            tanqueDer->setPos(posXDer, sceneH * 0);
            tanqueDer->setTiempoDisparo(3000);
            listaTanques.append(tanqueDer);
            escena->addItem(tanqueDer);
            connect(tanqueDer, &Persona::died, this, &Nivel1::onTankDied);
        }
        else if(tipoOleada == 1) {

            int sizeTanque = vistaAlto * 0.15;

            qreal posXIzq = 50;
            Tanque* tanqueIzq = new Tanque(sizeTanque, sizeTanque, sceneW, sceneH, -1); // -1 signifia lado izquierdo
            tanqueIzq->setPos(posXIzq, sceneH * 0);
            tanqueIzq->setTiempoDisparo(3000);
            listaTanques.append(tanqueIzq);
            escena->addItem(tanqueIzq);
            connect(tanqueIzq, &Persona::died, this, &Nivel1::onTankDied);

            int posX = 400;
            int sizeObs = vistaAlto * 0.12;

            for (int i = 0; i < 8; ++i) {
                posX += 100;
                Obstaculo* obs = new Obstaculo(posX, sceneH * 0, sizeObs, sizeObs,
                                               QColor(150, 75, 0), true);  // Marrón, móvil
                obs->setMovil(true);
                obs->setVida(2);
                obs->setVelocidad(4.0);
                obs->setDanioColision(20);
                obs->setBorderColor(Qt::black, 3);

                listaObstaculosMoviles.append(obs);
                escena->addItem(obs);
                connect(obs, &Obstaculo::obstaculoMuerto, this, &Nivel1::onObstaculoDestruido);
            }
        }
        else if(tipoOleada == 2) {

            int sizeTanque = vistaAlto * 0.15;

            qreal posXDer = 1300;
            Tanque* tanqueDer = new Tanque(sizeTanque, sizeTanque, sceneW, sceneH, 1); // 1 signifia lado derecho
            tanqueDer->setPos(posXDer, sceneH * 0);
            tanqueDer->setTiempoDisparo(3000);
            listaTanques.append(tanqueDer);
            escena->addItem(tanqueDer);
            connect(tanqueDer, &Persona::died, this, &Nivel1::onTankDied);

            int posX = 50;
            int sizeObs = vistaAlto * 0.12;

            for (int i = 0; i < 8; ++i) {
                posX += 100;
                Obstaculo* obs = new Obstaculo(posX, sceneH * 0, sizeObs, sizeObs,
                                               QColor(150, 75, 0), true);  // Marrón, móvil
                obs->setMovil(true);
                obs->setVida(2);
                obs->setVelocidad(4.0);
                obs->setDanioColision(20);
                obs->setBorderColor(Qt::black, 3);

                listaObstaculosMoviles.append(obs);
                escena->addItem(obs);
                connect(obs, &Obstaculo::obstaculoMuerto, this, &Nivel1::onObstaculoDestruido);
            }
        }
    }

    qDebug() << "Oleada spawneada. Enemigos:" << listaEnemigos.size()
             << "Tanques:" << listaTanques.size()
             << "Obstáculos:" << listaObstaculosMoviles.size();
}

void Nivel1::gameTick()
{
    if (juegoEnPausa || nivelGanado) return;

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
            listaEnemigos.removeOne(e);
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

    // *** NUEVO: Limpiar obstáculos móviles ***
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

    // *** NUEVO: Colisión con obstáculos móviles ***
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

    jugador->recibirDanio(1);  // 1 vida

    listaEnemigos.removeOne(e);
    escena->removeItem(e);
    e->deleteLater();
}

void Nivel1::colisionTanqueDetectada(Tanque* t)
{
    if (!t || !jugador) return;

    jugador->recibirDanio(2);  // 2 vidas

    listaTanques.removeOne(t);
    escena->removeItem(t);
    t->deleteLater();
}

void Nivel1::colisionObstaculoDetectada(Obstaculo* obs)
{
    if (!obs || !jugador) return;

    jugador->recibirDanio(2);  // 2 vidas

    listaObstaculosMoviles.removeOne(obs);
    escena->removeItem(obs);
    obs->deleteLater();
}

void Nivel1::crearObstaculos()
{
    int sueloAltura = 0;
    Obstaculo* suelo = new Obstaculo(0, sceneH - sueloAltura, sceneW, sueloAltura,
                                     QColor(139, 69, 19), false);
    suelo->setBorderColor(Qt::black, 2);
    obstaculos.append(suelo);
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

    if (hud) {
        hud->actualizarPuntuacion(puntosActuales, puntosObjetivo);
    }

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

void Nivel1::actualizar()
{
    NivelBase::actualizar();

    // *** ACTUALIZADO: Posicionar HUD debajo del jugador ***
    if (hud && jugador) {
        // Pasar la posición X e Y del jugador
        // El HUD se centrará automáticamente debajo
        hud->actualizarPosicion(jugador->x(), jugador->y());
    }
}

void Nivel1::onEnemyDied(Persona* p)
{
    Enemigo* e = dynamic_cast<Enemigo*>(p);
    if (!e) return;

    listaEnemigos.removeOne(e);
    agregarPuntos(5);  // *** 5 puntos por enemigo ***
}

void Nivel1::onTankDied(Persona* p)
{
    Tanque* t = dynamic_cast<Tanque*>(p);
    if (!t) return;

    listaTanques.removeOne(t);
    agregarPuntos(10);  // *** 10 puntos por tanque ***
}

void Nivel1::onObstaculoDestruido(Obstaculo* obs)
{
    if (!obs) return;

    listaObstaculosMoviles.removeOne(obs);
    agregarPuntos(3);  // *** 3 puntos por obstáculo ***

    escena->removeItem(obs);
    obs->deleteLater();
}

void Nivel1::onJugadorDaniado(int vidaActual, int vidaMax)
{
    if (hud) {
        hud->actualizarVidas(vidaActual);
    }
}

void Nivel1::onJugadorMurio()
{
    juegoEnPausa = true;
}

void Nivel1::manejarTecla(Qt::Key key)
{
    if (!juegoEnPausa) return;

    if (key == Qt::Key_R) {
        // Ocultar pantallas
        if (pantallaGameOver) pantallaGameOver->ocultar();
        if (pantallaVictoria) pantallaVictoria->ocultar();

        juegoEnPausa = false;
        nivelGanado = false;
        puntosActuales = 0;

        // Limpiar todo
        for (Enemigo* e : listaEnemigos) {
            if (e && escena) { escena->removeItem(e); e->deleteLater(); }
        }
        listaEnemigos.clear();

        for (Tanque* t : listaTanques) {
            if (t && escena) { escena->removeItem(t); t->deleteLater(); }
        }
        listaTanques.clear();

        for (Obstaculo* obs : listaObstaculosMoviles) {
            if (obs && escena) { escena->removeItem(obs); obs->deleteLater(); }
        }
        listaObstaculosMoviles.clear();

        // Recrear jugador
        if (jugador && escena) {
            escena->removeItem(jugador);
            jugador->deleteLater();
        }

        qreal posJugadorX = vistaAncho * 0.4;
        qreal alturaSuelo = sceneH * 0.4;
        qreal anchoJugador = vistaAncho * 0.1;
        qreal altoJugador = vistaAncho * 0.1;
        qreal posJugadorY = alturaSuelo - altoJugador;

        jugador = new Jugador(anchoJugador, altoJugador, sceneW, sceneH, TipoMovimiento::RECTILINEO);
        jugador->setPos(posJugadorX, posJugadorY);
        jugador->setVida(5);

        connect(jugador, &Persona::murioPersona, this, &Nivel1::onJugadorMurio);
        connect(jugador, &Persona::vidaCambiada, this, &Nivel1::onJugadorDaniado);

        escena->addItem(jugador);
        jugador->setFocus();

        // Actualizar HUD
        if (hud) {
            hud->actualizarVidas(5);
            hud->actualizarPuntuacion(0, puntosObjetivo);
        }

        spawnearOleada();

        qDebug() << "Nivel reiniciado";
    }
    else if (key == Qt::Key_Escape) {
        qDebug() << "Volver al menú...";
    }
}
