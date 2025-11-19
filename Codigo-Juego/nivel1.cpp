#include "nivel1.h"
//#include "jugador.h"
#include "enemigo.h"
#include "obstaculo.h"
#include <QColor>

/*
  Constructor
*/
Nivel1::Nivel1(QGraphicsScene* escena, QObject* parent)
    : NivelBase(escena, 1, parent)
{
}

/*
  Configuración específica del nivel 1
*/
void Nivel1::configurarNivel()
{
    // Crear jugador con movimiento rectilíneo
    crearJugador(700, sceneH - sceneH * 0.1 - 40, TipoMovimiento::RECTILINEO);
}

/*
  Crea enemigos para el nivel 1
*/
void Nivel1::crearEnemigos()
{
    int size = sceneH * 0.12;
    QTimer* gameTimer = nullptr;            // tick principal
    gameTimer = new QTimer(this);
    connect(gameTimer, &QTimer::timeout, this, &Nivel1::gameTick);
    gameTimer->start(16); // ~60 FPS


    int spawnDelayMs = 800;                 // tiempo entre spawns cuando uno muere
    int spawnMargin = 120;                  // cuánto por encima de la pantalla visible se puede spawnear

    spawnEnemyAboveView();

    /*Enemigo* e1 = new Enemigo(size, size, sceneW, sceneH, TipoMovimiento::RECTILINEO,1);
    e1->setPos(sceneW * 0.4, sceneH * 0.1);
    e1->setSpeed(0);
    enemigos.append(e1);
    escena->addItem(e1);*/
}

void Nivel1::spawnEnemyAboveView()
{
    int size = sceneH * 0.12;
    if (!escena) return;

    /* rect visible en coordenadas de escena
    QRectF visible = vista->mapToScene(vista->viewport()->rect()).boundingRect();
    qreal spawnY = visible.top() - spawnMargin;

    qreal minX = escena->sceneRect().left();
    qreal maxX = escena->sceneRect().width() - enemyWidth;*/

    //qreal spawnX = QRandomGenerator::global()->bounded(sceneH, sceneW);

    Enemigo* e1 = new Enemigo(size, size, sceneW, sceneH, TipoMovimiento::RECTILINEO,1);
    e1->setPos(sceneW * 0.4, sceneH * 0.1);
    e1->setSpeed(0);
    enemigos.append(e1);
    escena->addItem(e1);
    listaEnemigos.append(e1);
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
    for (Enemigo* e : copia) {
        if (!e) continue;
        qreal ey = e->scenePos().y();

        // si el enemigo pasó el fondo visible + margen
        if (ey > sceneH - cleanupMargin) {
            listaEnemigos.removeOne(e);
            escena->removeItem(e);
            e->deleteLater(); // seguro para Qt
            // programar spawn del siguiente enemigo después de un delay
            QTimer::singleShot(spawnDelayMs, this, &Nivel1::spawnEnemyAboveView);
        }
    }
}



/*
  Crea obstáculos para el nivel 1
*/
void Nivel1::crearObstaculos()
{
    int sueloAltura = 40;

    Obstaculo* suelo = new Obstaculo(0, sceneH - sueloAltura, sceneW, sueloAltura, QColor(139, 69, 19));
    suelo->setBorderColor(Qt::black, 2);
    obstaculos.append(suelo);
    escena->addItem(suelo);
}

/*
  Actualización por frame del nivel 1
*/
void Nivel1::actualizar()
{
    // El jugador se actualiza automáticamente con eventos de teclado
    // Aquí puedes agregar lógica adicional del nivel (colisiones, etc.)
}
