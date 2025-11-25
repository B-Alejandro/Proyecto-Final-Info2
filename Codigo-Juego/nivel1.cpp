
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
    // Implementar creación de enemigos específicos del nivel 1
    //int size = sceneH * 0.12;
    QTimer* gameTimer = nullptr;            // tick principal
    gameTimer = new QTimer(this);
    connect(gameTimer, &QTimer::timeout, this, &Nivel1::gameTick);
    gameTimer->start(16); // ~60 FPS



    spawnearOleada();

}

void Nivel1::spawnearOleada()
{
    int size = vistaAlto * 0.1;
    if (!escena) return;
    int tipoOleada = 0;
    //int tipoOleada = QRandomGenerator::global()->bounded(2);  // 0 a 5

    if(tipoOleada == 0){
        int posX = 50;

        for (int i = 0; i < 8; ++i) {
            posX += 150;

            Enemigo* e1 = new Enemigo(size, size, sceneW, sceneH, TipoMovimiento::RECTILINEO,1);
            e1->setPos(posX, sceneH * 0);
            e1->setSpeed(0);
            enemigos.append(e1);
            escena->addItem(e1);

            // guardamos referencias para control de la oleada
            listaEnemigos.append(e1);
            connect(e1, &Persona::died, this, &Nivel1::onEnemyDied);
        }
    }
    else if(tipoOleada == 1){
        int posX = 300;
        double posY[8] = {vistaAlto*0.1,vistaAlto*0.15,vistaAlto*0.2,vistaAlto*0.25,vistaAlto*0.25,vistaAlto*0.2,vistaAlto*0.15,vistaAlto*0.1};

        for (int i = 0; i < 8; ++i) {
            posX += 100;

            Enemigo* e1 = new Enemigo(size, size, sceneW, sceneH, TipoMovimiento::RECTILINEO,1);
            e1->setPos(posX, posY[i]);
            e1->setSpeed(0);
            enemigos.append(e1);
            escena->addItem(e1);

            // guardamos referencias para control de la oleada
            listaEnemigos.append(e1);
            // Conectar señal de muerte del enemigo hacia Nivel1
            connect(e1, &Persona::died, this, &Nivel1::onEnemyDied);
        }
    }
    else if(tipoOleada == 2){
        int posX = 100;
        double posY[8] = {sceneH*0.1,sceneH*0.15,sceneH*0.2,sceneH*0.25,sceneH*0.25,sceneH*0.2,sceneH*0.15,sceneH*0.1};
        for (int i = 0; i < 8; ++i) {
            posX += 100;

            Enemigo* e1 = new Enemigo(size, size, sceneW, sceneH, TipoMovimiento::RECTILINEO,1);
            e1->setPos(posX, posY[i]);
            e1->setSpeed(0);
            enemigos.append(e1);
            escena->addItem(e1);

            // guardamos referencias para control de la oleada
            listaEnemigos.append(e1);
            // Conectar señal de muerte del enemigo hacia Nivel1
            connect(e1, &Persona::died, this, &Nivel1::onEnemyDied);
        }
    }
}

void Nivel1::gameTick()
{
    // avanza items (llama a Enemy::advance())
    escena->advance();

    // cleanup: elimina enemigos que llegaron al fondo visible
    cleanupOffscreen();
    revisarColision();
}

void Nivel1::cleanupOffscreen()
{
    if (!escena) return;

    //QRectF visible = vista->mapToScene(vista->viewport()->rect()).boundingRect();
    //qreal bottom = visible.bottom();
    qreal cleanupMargin = 100; // margen extra

    // iteramos sobre copia porque vamos a borrar elementos
    auto copia = listaEnemigos;
    bool Removidos = false;

    for (Enemigo* e : copia) {
        if (!e) continue;
        qreal ey = e->scenePos().y();

        // si el enemigo pasó el fondo visible + margen
        if (ey > sceneH - cleanupMargin) {
            listaEnemigos.removeOne(e);
            escena->removeItem(e);
            e->deleteLater(); // seguro para Qt
            Removidos = true;
        }
    }

    if (Removidos && listaEnemigos.isEmpty()) {
        // Lanza nextWave tras spawnDelayMs
        QTimer::singleShot(spawnDelayMs, this, [this]() {
            this->spawnearOleada();
        });
    }
}

/*
 * Detectar colisiones entre enmigos y jugador para aplicar las penalizaciones necesarias
 *
 */
void Nivel1::revisarColision()
{

    if (!jugador) return;

    QRectF playerRect = jugador->sceneBoundingRect();

    // iteramos sobre copia porque onEnemyRemoved() puede modificar la lista
    auto copia = listaEnemigos;
    for (Enemigo* e : copia) {
        if (!e) continue;

        // 1) filtro rápido: bounding rect overlap?
        if (playerRect.intersects(e->sceneBoundingRect())) {
            colisionDetectada(e);
        }

        // 2) comprobación precisa: usa collidesWithItem con la forma (shape)
        // Usa IntersectsItemShape para basarse en shape() en lugar de boundingRect
        if (jugador->collidesWithItem(e, Qt::IntersectsItemShape)) {
            // ¡Colisión detectada!
            colisionDetectada(e);        // función que maneja daño/efectos
            // onPlayerHit puede llamar a onEnemyRemoved(e) si quieres borrar enemigo
        }
    }
}
void Nivel1::colisionDetectada(Enemigo* e)
{
    if (!e || !jugador) return;

    // ejemplo: bajar vida del jugador (suponiendo que jugador tiene metodo damage())
    // jugador->damage(1);

    // eliminar enemigo y mantener listas consistentes
    if (!e) return;

    // Eliminarlo de la lista de enemigos del juego
    listaEnemigos.removeOne(e);
    escena->removeItem(e);
    e->deleteLater(); // seguro para Qt
    // opcional: reproducir sonido, animación, efectos, etc.
}

/*
  Crea obstáculos para el nivel 1
*/
void Nivel1::crearObstaculos()
{
    int sueloAltura = 0;

    Obstaculo* suelo = new Obstaculo(0, sceneH - sueloAltura, sceneW, sueloAltura, QColor(139, 69, 19));
    suelo->setBorderColor(Qt::black, 2);
    obstaculos.append(suelo);
    escena->addItem(suelo);
}

void Nivel1::onEnemyDied(Persona* p)
{
    Enemigo* e = dynamic_cast<Enemigo*>(p);
    if (!e) return;  // No es enemigo, ignorar.

    // 1. Eliminar de la lista
    listaEnemigos.removeOne(e);


    qDebug() << "Enemigo eliminado correctamente. Enemigos restantes:" << listaEnemigos.size();

    // 2. Aquí NO llamas delete, ya se hace en takeDamage() con deleteLater()
    //    Así que NO toques memoria, ya está hecho.
}
