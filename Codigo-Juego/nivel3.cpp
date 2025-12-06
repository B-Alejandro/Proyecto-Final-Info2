// ============ nivel3.cpp - CON COLECCIONABLES Y VICTORIA ============
#include "nivel3.h"
#include "juego.h"
#include "jugador.h"
#include "enemigointeligente.h"
#include "coleccionable.h"
#include <QDebug>
#include <QTimer>

Nivel3::Nivel3(Juego* juego, QObject* parent)
    : NivelBase(juego, 3, parent)
    , coleccionablesRecolectados(0)
    , totalColeccionables(5)
    , nivelGanado(false)
{
    // Crear escena grande para tener espacio
    int ancho = juego->getVistaAncho() ;  // Más espacio horizontal
    int alto = juego->getVistaAlto();

    crearEscena(ancho, alto);

    qDebug() << "\n╔════════════════════════════════════════════════════╗";
    qDebug() << "║  NIVEL 3: RECOLECTA Y ESCAPA                      ║";
    qDebug() << "╚════════════════════════════════════════════════════╝\n";
}

void Nivel3::configurarNivel()
{
    qDebug() << "⚙️  Configurando nivel...";

    // Crear jugador MÁS PEQUEÑO en la esquina inferior izquierda
    qreal posJugadorX = sceneW * 0.15;
    qreal posJugadorY = sceneH * 0.7;

    crearJugador(posJugadorX, posJugadorY, TipoMovimiento::RECTILINEO);

    if (jugador) {
        // *** REDUCIR TAMAÑO: 50% más pequeño ***
        qreal escala = 0.5;
        jugador->setScale(escala);

        jugador->setSpeed(7.0);
        jugador->setVida(100);
        qDebug() << "✓ Jugador creado (TAMAÑO REDUCIDO 50%)";
        qDebug() << "  Posición:" << posJugadorX << posJugadorY;
        qDebug() << "  Velocidad:" << jugador->getSpeed();
    }
}

void Nivel3::crearEnemigos()
{
    qDebug() << "\n🤖 Creando Enemigo Inteligente...";

    // *** TAMAÑO REDUCIDO: 50% más pequeño ***
    qreal sizeEnemigo = sceneH * 0.06;  // Era 0.12, ahora 0.06

    // Enemigo centinela
    EnemigoInteligente* centinela = new EnemigoInteligente(
        sizeEnemigo,
        sizeEnemigo,
        sceneW,
        sceneH,
        TipoMovimiento::RECTILINEO,
        3,
        250.0  // Radio de detección reducido
        );

    // Posicionar en el centro
    centinela->setPos(sceneW * 0.5, sceneH * 0.5);
    centinela->setVida(50);
    centinela->setSpeed(0);
    centinela->mostrarAreaDeteccion(true);
    centinela->setBrush(QBrush(QColor(100, 100, 255)));

    if (escena) escena->addItem(centinela);
    enemigos.append(centinela);

    qDebug() << "✓ Centinela creado (TAMAÑO REDUCIDO 50%):";
    qDebug() << "  Posición:" << centinela->pos();
    qDebug() << "  Radio detección:" << centinela->getRadioDeteccion() << "px";
    qDebug() << "";
}

void Nivel3::crearObstaculos()
{
    qDebug() << "🌟 Creando coleccionables...\n";

    // Coleccionable 1
    Coleccionable* col1 = new Coleccionable(sceneW * 0.2, sceneH * 0.3, 20.0, 0);
    escena->addItem(col1);
    listaColeccionables.append(col1);
    connect(col1, &Coleccionable::coleccionableRecolectado,
            this, &Nivel3::onColeccionableRecolectado);

    // Coleccionable 2
    Coleccionable* col2 = new Coleccionable(sceneW * 0.2, sceneH * 0.2, 20.0, 1);
    escena->addItem(col2);
    listaColeccionables.append(col2);
    connect(col2, &Coleccionable::coleccionableRecolectado,
            this, &Nivel3::onColeccionableRecolectado);

    // Coleccionable 3
    Coleccionable* col3 = new Coleccionable(sceneW * 0.2, sceneH * 0.4, 20.0, 2);
    escena->addItem(col3);
    listaColeccionables.append(col3);
    connect(col3, &Coleccionable::coleccionableRecolectado,
            this, &Nivel3::onColeccionableRecolectado);

    // Coleccionable 4
    Coleccionable* col4 = new Coleccionable(sceneW * 0.2, sceneH * 0.8, 20.0, 0);
    escena->addItem(col4);
    listaColeccionables.append(col4);
    connect(col4, &Coleccionable::coleccionableRecolectado,
            this, &Nivel3::onColeccionableRecolectado);

    // Coleccionable 5
    Coleccionable* col5 = new Coleccionable(sceneW * 0.2, sceneH * 0.1, 20.0, 1);
    escena->addItem(col5);
    listaColeccionables.append(col5);
    connect(col5, &Coleccionable::coleccionableRecolectado,
            this, &Nivel3::onColeccionableRecolectado);

    qDebug() << "✓ 5 coleccionables creados\n";
}

// ════════════════════════════════════════════════════════════
//  ACTUALIZACIÓN POR FRAME
// ════════════════════════════════════════════════════════════
void Nivel3::actualizar()
{
    if (nivelGanado) return;

    NivelBase::actualizar();

    if (escena) {
        escena->advance();
    }

    // Verificar colisiones con coleccionables
    for (Coleccionable* col : listaColeccionables) {
        if (col && !col->fueRecolectado()) {
            col->verificarColision();
        }
    }
}


// ════════════════════════════════════════════════════════════
// 🎯 SISTEMA DE COLECCIONABLES
// ════════════════════════════════════════════════════════════
void Nivel3::onColeccionableRecolectado(Coleccionable* col)
{
    if (!col) return;

    coleccionablesRecolectados++;

    qDebug() << "\n╔════════════════════════════════════════════════════╗";
    qDebug() << "║  ✨ COLECCIONABLE RECOLECTADO!                     ║";
    qDebug() << "╚════════════════════════════════════════════════════╝";
    qDebug() << "   📊 Progreso:" << coleccionablesRecolectados << "/" << totalColeccionables;

    // Eliminar de la lista de forma segura
    listaColeccionables.removeOne(col);

    // Eliminar de la escena
    if (escena) {
        escena->removeItem(col);
    }

    // Programar eliminación del objeto
    col->deleteLater();

    // Verificar victoria
    if (coleccionablesRecolectados >= totalColeccionables) {
        nivelGanado = true;
        mostrarVictoria();
    } else {
        qDebug() << "   💡 Faltan:" << (totalColeccionables - coleccionablesRecolectados);
        qDebug() << "";
    }
}

void Nivel3::mostrarVictoria()
{
    qDebug() << "\n";
    qDebug() << "╔════════════════════════════════════════════════════╗";
    qDebug() << "║                                                    ║";
    qDebug() << "║          ★ ★ ★  ¡VICTORIA!  ★ ★ ★                ║";
    qDebug() << "║                                                    ║";
    qDebug() << "║  ¡Has recolectado todos los objetos!              ║";
    qDebug() << "║                                                    ║";
    qDebug() << "║  🎉 ¡NIVEL COMPLETADO! 🎉                         ║";
    qDebug() << "║                                                    ║";
    qDebug() << "╚════════════════════════════════════════════════════╝\n";

    // Opcional: Detener el juego o mostrar pantalla de victoria
    // Por ahora solo mostramos el mensaje
}
