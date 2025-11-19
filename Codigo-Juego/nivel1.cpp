
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

void Nivel1::crearEnemigos()
{
    // Implementar creación de enemigos específicos del nivel 1
}

void Nivel1::crearObstaculos()
{
    // Implementar creación de obstáculos específicos del nivel 1
}
