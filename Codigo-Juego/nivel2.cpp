
// ============ nivel2.cpp ============
#include "nivel2.h"
#include "juego.h"
#include "jugador.h"
#include "enemigo.h"
#include "obstaculo.h"
#include "persona.h"
#include <QGraphicsView>
#include <QBrush>
#include <QRandomGenerator>

Nivel2::Nivel2(Juego* juego, QObject* parent)
    : NivelBase(juego, 2, parent), enemigoAtras(0), suelo(0)
{
    // Guardar dimensiones de la vista
    vistaAncho = juego->getVistaAncho();
    vistaAlto = juego->getVistaAlto();

    // Crear escena 3 veces más grande que la vista
    // (3x en ancho para scroll horizontal, mantener alto)
    int anchoEscena = vistaAncho * 3;
    int altoEscena = vistaAlto;

    crearEscena(anchoEscena, altoEscena);
}

void Nivel2::configurarNivel()
{
    // Crear jugador a mitad de pantalla (50% del ancho de la vista)
    qreal posJugadorX = vistaAncho * 0.5;
    qreal alturaSuelo = sceneH * 0.85; // El suelo está al 85% de altura
    qreal posJugadorY = alturaSuelo - sceneH * 0.1; // Jugador encima del suelo

    // Usar movimiento CON_GRAVEDAD para que solo pueda saltar (sin movimiento horizontal)
    crearJugador(posJugadorX, posJugadorY, TipoMovimiento::CON_GRAVEDAD);

    // Centrar la vista inicialmente en el jugador
    if (escena && !escena->views().isEmpty()) {
        QGraphicsView* vista = escena->views().first();
        vista->centerOn(jugador);
    }
}

void Nivel2::crearEnemigos()
{
    if (!jugador) return;

    // Crear UN SOLO enemigo atrás del jugador (a la izquierda)
    // Posición inicial: 20% del ancho de la vista desde el inicio
    qreal posEnemigoX = vistaAncho * 0.2;
    qreal alturaSuelo = sceneH * 0.85;
    qreal posEnemigoY = alturaSuelo - sceneH * 0.08; // Encima del suelo

    int sizeEnemigo = sceneH * 0.08;
    enemigoAtras = new Enemigo(sizeEnemigo, sizeEnemigo, sceneW, sceneH, TipoMovimiento::CON_GRAVEDAD);
    enemigoAtras->setPos(posEnemigoX, posEnemigoY);
    enemigoAtras->setBrush(QBrush(Qt::red));
    enemigoAtras->setSpeed(4); // Velocidad constante hacia adelante

    escena->addItem(enemigoAtras);
    enemigos.append(enemigoAtras);
}

void Nivel2::crearObstaculos()
{
    // Crear el SUELO
    qreal posYSuelo = sceneH * 0.85; // El suelo empieza al 85% de altura
    qreal alturaSuelo = sceneH * 0.15; // 15% de altura para el suelo

    suelo = new Obstaculo(0, posYSuelo, sceneW, alturaSuelo, Qt::darkGreen);
    escena->addItem(suelo);
    obstaculos.append(suelo);

    // Crear obstáculos que se moverán hacia la izquierda
    // TODOS al nivel del suelo (apoyados sobre el suelo)
    for (int i = 0; i < 8; i++) {
        qreal posX = vistaAncho * (1.0 + i * 0.5); // Distribuidos hacia adelante

        // Obstáculos de diferentes tamaños
        int ancho = 50 + QRandomGenerator::global()->bounded(30);
        int alto = 50 + QRandomGenerator::global()->bounded(50);

        // Posición Y: justo encima del suelo
        qreal posY = posYSuelo - alto;

        Obstaculo* obs = new Obstaculo(posX, posY, ancho, alto, Qt::darkGray);
        obs->setBorderColor(Qt::black, 2);

        escena->addItem(obs);
        obstaculos.append(obs);
    }
}

void Nivel2::actualizar()
{
    // Llamar actualización base
    NivelBase::actualizar();

    // Implementar scroll automático
    if (jugador && escena && !escena->views().isEmpty()) {
        QGraphicsView* vista = escena->views().first();

        // Velocidad de movimiento de los obstáculos (hacia la izquierda)
        qreal velocidadObstaculos = 5.0; // Más rápido que antes

        // Mover TODOS los obstáculos hacia la izquierda (excepto el suelo que es fijo visualmente)
        for (Obstaculo* obs : obstaculos) {
            if (obs != suelo) { // No mover el suelo completo
                obs->setPos(obs->x() - velocidadObstaculos, obs->y());
            }
        }

        // El enemigo simplemente salta detrás del jugador (su física se maneja en Persona)
        // Mantenerlo siempre detrás a una distancia fija
        if (enemigoAtras) {
            qreal distanciaIdeal = vistaAncho * 0.3; // 30% del ancho de la vista detrás
            qreal posIdealX = jugador->x() - distanciaIdeal;

            // Si el enemigo se aleja mucho, reposicionarlo
            if (enemigoAtras->x() < posIdealX - 50) {
                enemigoAtras->setX(posIdealX - 50);
            }

            // Verificar si el enemigo alcanzó al jugador
            qreal distancia = jugador->x() - enemigoAtras->x();
            if (distancia < 30) {
                // El enemigo alcanzó al jugador - GAME OVER
                // emit juegoTerminado();
                // Por ahora, reiniciar posición del enemigo
                enemigoAtras->setPos(jugador->x() - distanciaIdeal, enemigoAtras->y());
            }
        }

        // Centrar la vista en el jugador (mantener jugador a mitad de pantalla)
        vista->centerOn(jugador->x(), sceneH / 2);

        // Regenerar obstáculos que salen por la izquierda
        qreal limiteIzquierdo = jugador->x() - vistaAncho;
        qreal posYSuelo = sceneH * 0.85;

        for (Obstaculo* obs : obstaculos) {
            if (obs != suelo && obs->x() < limiteIzquierdo) {
                // Reposicionar adelante con variación aleatoria
                qreal nuevoX = jugador->x() + vistaAncho + QRandomGenerator::global()->bounded(200);

                // Nuevo tamaño aleatorio
                int ancho = 50 + QRandomGenerator::global()->bounded(30);
                int alto = 50 + QRandomGenerator::global()->bounded(50);

                // Siempre al nivel del suelo
                qreal nuevoY = posYSuelo - alto;

                obs->setPos(nuevoX, nuevoY);
                // Actualizar el tamaño del rectángulo
                obs->setRect(0, 0, ancho, alto);
            }
        }

        // Verificar colisiones con obstáculos
        if (jugador) {
            QList<QGraphicsItem*> colisiones = jugador->collidingItems();
            for (QGraphicsItem* item : colisiones) {
                Obstaculo* obs = dynamic_cast<Obstaculo*>(item);
                if (obs && obs != suelo) {
                    // Colisión con obstáculo - GAME OVER o resetear
                    // emit juegoTerminado();
                }
            }
        }
    }
}
