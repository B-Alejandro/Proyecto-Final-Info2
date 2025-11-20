
// ============ nivel1.cpp ============
#include "nivel1.h"
#include "juego.h"
#include "jugador.h"
#include "enemigo.h"
#include "obstaculo.h"
#include "persona.h"

Nivel1::Nivel1(Juego* juego, QObject* parent)
    : NivelBase(juego, 1, parent)
{
    // Crear escena del mismo tamaño que la vista
    int ancho = juego->getVistaAncho();
    int alto = juego->getVistaAlto();

    crearEscena(ancho, alto);
}

void Nivel1::configurarNivel()
{
    // Crear jugador con movimiento libre
    crearJugador(sceneW / 2, sceneH - 100, TipoMovimiento::RECTILINEO);
}

/*
  Crea enemigos para el nivel 1
*/
void Nivel1::crearEnemigos()
{
    //int size = sceneH * 0.12;
    QTimer* gameTimer = nullptr;            // tick principal
    gameTimer = new QTimer(this);
    connect(gameTimer, &QTimer::timeout, this, &Nivel1::gameTick);
    gameTimer->start(16); // ~60 FPS



    spawnearOleada();

}

void Nivel1::spawnearOleada()
{
    int size = sceneH * 0.12;
    if (!escena) return;
    int tipoOleada = 0;
    //int tipoOleada = QRandomGenerator::global()->bounded(2);  // 0 a 5

    if(tipoOleada == 0){
        int posX = 50;

        for (int i = 0; i < 8; ++i) {
            posX += 150;

            Enemigo* e1 = new Enemigo(size, size, sceneW, sceneH, TipoMovimiento::RECTILINEO,1);
            e1->setPos(posX, sceneH * 0.1);
            e1->setSpeed(0);
            enemigos.append(e1);
            escena->addItem(e1);

            // guardamos referencias para control de la oleada
            listaEnemigos.append(e1);
        }
    }
    else if(tipoOleada == 1){
        int posX = 300;
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
        }
    }
}

void Nivel1::gameTick()
{
    // avanza items (llama a Enemy::advance())
    escena->advance();

    // cleanup: elimina enemigos que llegaron al fondo visible
    cleanupOffscreen();
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
