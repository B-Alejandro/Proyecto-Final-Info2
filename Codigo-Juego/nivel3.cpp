// ============ nivel3.cpp - CON ENEMIGO INTELIGENTE ============
#include "nivel3.h"
#include "juego.h"
#include "jugador.h"
#include "enemigo.h"
#include "enemigoInteligente.h"  // *** NUEVO ***
#include "obstaculo.h"
#include "persona.h"
#include <QDebug>

Nivel3::Nivel3(Juego* juego, QObject* parent)
    : NivelBase(juego, 3, parent)
{
    int ancho = juego->getVistaAncho();
    int alto = juego->getVistaAlto();

    crearEscena(ancho, alto);

    qDebug() << "=== NIVEL 3: ESCENARIO DE PRUEBA ===";
    qDebug() << "Escena:" << ancho << "x" << alto;
}

//ACTUALIZADOR
void Nivel3::actualizar()
{
    // Llamar a la actualización base (mueve jugador y enemigos)
    NivelBase::actualizar();

    // CRÍTICO: Llamar a advance() para que los enemigos detecten
    if (escena) {
        escena->advance();
    }
}

void Nivel3::configurarNivel()
{
    qDebug() << "Configurando Nivel 3 con Enemigo Inteligente...";

    // Crear jugador en la esquina inferior izquierda
    qreal posJugadorX = sceneW * 0.2;
    qreal posJugadorY = sceneH * 0.7;

    crearJugador(posJugadorX, posJugadorY, TipoMovimiento::RECTILINEO);

    if (jugador) {
        jugador->setSpeed(7.0);  // Velocidad normal
        jugador->setVida(3);
        qDebug() << "Jugador creado en:" << posJugadorX << posJugadorY;
    }
}

void Nivel3::crearEnemigos()
{
    qDebug() << "=== CREANDO ENEMIGOS INTELIGENTES ===";

    qreal sizeEnemigo = sceneH * 0.12;

    // ════════════════════════════════════════════════════════════
    // 🎯 ENEMIGO INTELIGENTE CON SISTEMA PROGRESIVO
    // ════════════════════════════════════════════════════════════
    EnemigoInteligente* centinela = new EnemigoInteligente(
        sizeEnemigo,
        sizeEnemigo,
        sceneW,
        sceneH,
        TipoMovimiento::RECTILINEO,
        3,
        300.0  // Radio de detección: 300 píxeles
        );

    // Posicionar en el centro de la escena
    centinela->setPos(sceneW * 0.6, sceneH * 0.5);
    centinela->setVida(50);
    centinela->setSpeed(4.0);  // Estático por defecto
    centinela->mostrarAreaDeteccion(true);  // Mostrar área visual
    centinela->setBrush(QBrush(QColor(100, 100, 255)));  // Azul inicial

    if (escena) escena->addItem(centinela);
    enemigos.append(centinela);

    qDebug() << "✓ Centinela creado:";
    qDebug() << "  Posición:" << centinela->pos();
    qDebug() << "  Radio detección:" << centinela->getRadioDeteccion() << "px";
    qDebug() << "  Vida:" << centinela->getVida();
    qDebug() << "";

}

void Nivel3::crearObstaculos()
{

}
