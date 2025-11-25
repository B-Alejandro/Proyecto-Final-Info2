// ============ nivel1.cpp ============
#include "nivel1.h"
#include "juego.h"
#include "jugador.h"
#include "enemigo.h"
#include "obstaculo.h"
#include "fisica.h"
#include "persona.h"

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

    int tipoOleada = 0;
    //int tipoOleada = QRandomGenerator::global()->bounded(3);  // 0, 1 o 2

    if(tipoOleada == 0){
        int posX = 50;

        for (int i = 0; i < 8; ++i) {
            posX += 150;

            Enemigo* e1 = new Enemigo(size, size, sceneW, sceneH, TipoMovimiento::RECTILINEO, 1);
            e1->setPos(posX, sceneH * 0);  // Spawn en la parte superior
            // *** NO LLAMAR setSpeed(0) - dejamos que use la velocidad del constructor ***

            enemigos.append(e1);
            escena->addItem(e1);

            // guardamos referencias para control de la oleada
            listaEnemigos.append(e1);
            connect(e1, &Persona::died, this, &Nivel1::onEnemyDied);

            qDebug() << "Enemigo spawneado en:" << e1->x() << e1->y() << "speed:" << e1->getSpeed();
        }
    }
    else if(tipoOleada == 1){
        int posX = 300;
        double posY[8] = {vistaAlto*0.1, vistaAlto*0.15, vistaAlto*0.2, vistaAlto*0.25,
                          vistaAlto*0.25, vistaAlto*0.2, vistaAlto*0.15, vistaAlto*0.1};

        for (int i = 0; i < 8; ++i) {
            posX += 100;

            Enemigo* e1 = new Enemigo(size, size, sceneW, sceneH, TipoMovimiento::RECTILINEO, 1);
            e1->setPos(posX, posY[i]);
            // *** NO llamar setSpeed(0) ***

            enemigos.append(e1);
            escena->addItem(e1);

            listaEnemigos.append(e1);
            connect(e1, &Persona::died, this, &Nivel1::onEnemyDied);
        }
    }
    else if(tipoOleada == 2){
        int posX = 100;
        double posY[8] = {sceneH*0.1, sceneH*0.15, sceneH*0.2, sceneH*0.25,
                          sceneH*0.25, sceneH*0.2, sceneH*0.15, sceneH*0.1};

        for (int i = 0; i < 8; ++i) {
            posX += 100;

            Enemigo* e1 = new Enemigo(size, size, sceneW, sceneH, TipoMovimiento::RECTILINEO, 1);
            e1->setPos(posX, posY[i]);
            // *** NO llamar setSpeed(0) ***

            enemigos.append(e1);
            escena->addItem(e1);

            listaEnemigos.append(e1);
            connect(e1, &Persona::died, this, &Nivel1::onEnemyDied);
        }
    }

    qDebug() << "Oleada spawneada. Total enemigos:" << listaEnemigos.size();
}

void Nivel1::gameTick()
{
    // *** CRÍTICO: advance() mueve todos los items que implementan advance() ***
    // Esto incluye los Proyectiles
    if (escena) {
        escena->advance();
    }

    // Revisar colisiones
    revisarColision();

    // Eliminar enemigos que salieron de la pantalla
    cleanupOffscreen();
}

void Nivel1::cleanupOffscreen()
{
    if (!escena) return;

    qreal cleanupMargin = 100; // margen extra

    // iteramos sobre copia porque vamos a borrar elementos
    auto copia = listaEnemigos;
    bool removidos = false;

    for (Enemigo* e : copia) {
        if (!e) continue;
        qreal ey = e->scenePos().y();

        // si el enemigo pasó el fondo visible + margen
        if (ey > sceneH - cleanupMargin) {
            qDebug() << "Enemigo fuera de pantalla en y:" << ey << "sceneH:" << sceneH;

            listaEnemigos.removeOne(e);
            enemigos.removeOne(e);
            escena->removeItem(e);
            e->deleteLater();
            removidos = true;
        }
    }

    if (removidos && listaEnemigos.isEmpty()) {
        qDebug() << "Todos los enemigos eliminados. Spawneando nueva oleada en" << spawnDelayMs << "ms";

        // Lanza nextWave tras spawnDelayMs
        QTimer::singleShot(spawnDelayMs, this, [this]() {
            this->spawnearOleada();
        });
    }
}

/*
 * Detectar colisiones entre enemigos y jugador
 */
void Nivel1::revisarColision()
{
    if (!jugador) return;

    QRectF playerRect = jugador->sceneBoundingRect();

    // iteramos sobre copia porque colisionDetectada() puede modificar la lista
    auto copia = listaEnemigos;
    for (Enemigo* e : copia) {
        if (!e) continue;

        // Comprobación precisa con shape
        if (jugador->collidesWithItem(e, Qt::IntersectsItemShape)) {
            colisionDetectada(e);
        }
    }
}

void Nivel1::colisionDetectada(Enemigo* e)
{
    if (!e || !jugador) return;

    qDebug() << "¡Colisión detectada con enemigo!";

    // Aplicar daño al jugador
    jugador->recibirDanio(10);

    // Eliminar enemigo
    listaEnemigos.removeOne(e);
    enemigos.removeOne(e);
    escena->removeItem(e);
    e->deleteLater();
}

/*
  Crea obstáculos para el nivel 1
*/
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

    qDebug() << "Señal died recibida de enemigo";

    // Eliminar de la lista
    listaEnemigos.removeOne(e);
    enemigos.removeOne(e);

    qDebug() << "Enemigo eliminado. Enemigos restantes:" << listaEnemigos.size();
}
