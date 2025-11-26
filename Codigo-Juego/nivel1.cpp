// ============ nivel1.cpp - MODIFICADO CON TANQUES ============
#include "nivel1.h"
#include "juego.h"
#include "jugador.h"
#include "enemigo.h"
#include "tanque.h"
#include "obstaculo.h"
#include "fisica.h"
#include "persona.h"
#include <QRandomGenerator>

Nivel1::Nivel1(Juego* juego, QObject* parent)
    : NivelBase(juego, 1, parent)
{
    // Crear escena del mismo tamaño que la vista
    vistaAncho = juego->getVistaAncho();
    vistaAlto = juego->getVistaAlto();

    int ancho = vistaAlto*3;
    int alto = vistaAncho;

    crearEscena(ancho, alto);
}

void Nivel1::configurarNivel()
{
    // Crear jugador con movimiento libre
    qreal posJugadorX = vistaAncho * 0.4;
    qreal alturaSuelo = sceneH *0.4;

    // Tamaño del jugador
    qreal anchoJugador = vistaAncho * 0.1;
    qreal altoJugador = vistaAncho * 0.1;

    qreal posJugadorY = alturaSuelo - altoJugador;

    // Crear jugador con nuevo tamaño
    jugador = new Jugador(anchoJugador, altoJugador, sceneW, sceneH, TipoMovimiento::RECTILINEO);
    jugador->setPos(posJugadorX, posJugadorY);
    escena->addItem(jugador);
}

void Nivel1::crearEnemigos()
{
    // Timer principal del nivel
    QTimer* gameTimer = new QTimer(this);
    connect(gameTimer, &QTimer::timeout, this, &Nivel1::gameTick);
    gameTimer->start(16); // ~60 FPS

    spawnearOleada();
}

void Nivel1::spawnearOleada()
{
    int size = vistaAlto * 0.1;
    if (!escena) return;

    // Determinar tipo de oleada aleatoriamente
    //int tipoOleada = QRandomGenerator::global()->bounded(4);  // 0, 1, 2, o 3 (nueva con tanques)
    int tipoOleada = 3;

    if(tipoOleada == 0) {
        // Oleada horizontal de enemigos
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
    else if(tipoOleada == 3) {
        // *** NUEVA: Oleada con tanques en los extremos ***
        qDebug() << "=== SPAWNEANDO OLEADA CON TANQUES ===";

        // Tamaño de tanques (más grandes que enemigos)
        int sizeTanque = vistaAlto * 0.15;

        // Spawn 2 tanques en los extremos
        qreal margenLateral = 50;

        // Tanque izquierdo
        qreal posXIzq = margenLateral;
        Tanque* tanqueIzq = new Tanque(sizeTanque, sizeTanque, sceneW, sceneH, -1);
        tanqueIzq->setPos(posXIzq, sceneH * 0);
        tanqueIzq->setTiempoDisparo(3000);  // Dispara cada 3 segundos

        listaTanques.append(tanqueIzq);
        escena->addItem(tanqueIzq);
        connect(tanqueIzq, &Persona::died, this, &Nivel1::onTankDied);

        qDebug() << "Tanque izquierdo spawneado en X:" << posXIzq;

        // Tanque derecho
        qreal posXDer = margenLateral + 1000;
        Tanque* tanqueDer = new Tanque(sizeTanque, sizeTanque, sceneW, sceneH, 1);
        tanqueDer->setPos(posXDer, sceneH * 0);
        tanqueDer->setTiempoDisparo(3500);  // Dispara con ritmo ligeramente diferente

        listaTanques.append(tanqueDer);
        escena->addItem(tanqueDer);
        connect(tanqueDer, &Persona::died, this, &Nivel1::onTankDied);

        qDebug() << "Tanque derecho spawneado en X:" << posXDer;

        // Agregar algunos enemigos normales también
        /*int posX = sceneW * 0.3;
        for (int i = 0; i < 5; ++i) {
            posX += 120;

            Enemigo* e1 = new Enemigo(size, size, sceneW, sceneH, TipoMovimiento::RECTILINEO, 1);
            e1->setPos(posX, sceneH * 0);

            listaEnemigos.append(e1);
            escena->addItem(e1);
            connect(e1, &Persona::died, this, &Nivel1::onEnemyDied);
        }

        qDebug() << "Oleada de tanques completa: 2 tanques + 5 enemigos";
        */
    }

    qDebug() << "Oleada spawneada. Enemigos:" << listaEnemigos.size()
             << "Tanques:" << listaTanques.size();
}

void Nivel1::gameTick()
{
    // Advance mueve todos los items (proyectiles, explosiones, etc.)
    if (escena) {
        escena->advance();
    }

    // Revisar colisiones
    revisarColision();

    // Eliminar enemigos y tanques fuera de pantalla
    cleanupOffscreen();
}

void Nivel1::cleanupOffscreen()
{
    if (!escena) return;

    qreal cleanupMargin = 100;
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

    // *** CORREGIDO: Limpiar tanques con su altura incluida ***
    auto copiaTanques = listaTanques;
    for (Tanque* t : copiaTanques) {
        if (!t) continue;

        // Usar el BORDE INFERIOR del tanque (posición Y + altura)
        qreal ty = t->scenePos().y();
        qreal cleanupMarginT = 200;


        // Eliminar cuando el CENTRO o borde superior pase el límite
        // (más generoso para que se note que salió de pantalla)
        if (ty > sceneH - cleanupMarginT) {
            listaTanques.removeOne(t);
            escena->removeItem(t);
            t->deleteLater();
            removidos = true;
        }
    }

    // Si todos fueron eliminados, spawnear nueva oleada
    if (removidos && listaEnemigos.isEmpty() && listaTanques.isEmpty()) {
        qDebug() << "Todos los enemigos y tanques eliminados. Nueva oleada en" << spawnDelayMs << "ms";

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

    // *** NUEVO: Colisión con tanques ***
    auto copiaTanques = listaTanques;
    for (Tanque* t : copiaTanques) {
        if (!t) continue;

        if (jugador->collidesWithItem(t, Qt::IntersectsItemShape)) {
            colisionTanqueDetectada(t);
        }
    }
}

void Nivel1::colisionDetectada(Enemigo* e)
{
    if (!e || !jugador) return;

    qDebug() << "¡Colisión con enemigo!";

    // Aplicar daño al jugador
    jugador->recibirDanio(10);

    // Eliminar enemigo
    listaEnemigos.removeOne(e);
    escena->removeItem(e);
    e->deleteLater();
}

void Nivel1::colisionTanqueDetectada(Tanque* t)
{
    if (!t || !jugador) return;

    qDebug() << "¡Colisión con tanque!";

    // Aplicar más daño que un enemigo normal
    jugador->recibirDanio(20);

    // Eliminar tanque
    listaTanques.removeOne(t);
    escena->removeItem(t);
    t->deleteLater();
}

void Nivel1::crearObstaculos()
{
    // Sin suelo visible en este nivel
    int sueloAltura = 0;

    Obstaculo* suelo = new Obstaculo(0, sceneH - sueloAltura, sceneW, sueloAltura, QColor(139, 69, 19));
    suelo->setBorderColor(Qt::black, 2);
    obstaculos.append(suelo);
    escena->addItem(suelo);
}

void Nivel1::onEnemyDied(Persona* p)
{
    Enemigo* e = dynamic_cast<Enemigo*>(p);
    if (!e) return;

    qDebug() << "Enemigo murió";

    listaEnemigos.removeOne(e);
}

void Nivel1::onTankDied(Persona* p)
{
    Tanque* t = dynamic_cast<Tanque*>(p);
    if (!t) return;

    qDebug() << "Tanque murió. Vida era:" << t->getVida();

    listaTanques.removeOne(t);
}
