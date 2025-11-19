
// ============ nivel3.cpp ============
#include "nivel3.h"
#include "juego.h"
#include "jugador.h"
#include "enemigo.h"
#include "obstaculo.h"
#include "persona.h"

Nivel3::Nivel3(Juego* juego, QObject* parent)
    : NivelBase(juego, 3, parent)
{
    // Crear escena del mismo tamaño que la vista
    int ancho = juego->getVistaAncho();
    int alto = juego->getVistaAlto();

    crearEscena(ancho, alto);
}

void Nivel3::configurarNivel()
{
    // Crear jugador con movimiento vertical
    crearJugador(sceneW / 2, sceneH / 2, TipoMovimiento::RECTILINEO);
}

void Nivel3::crearEnemigos()
{
    // Implementar creación de enemigos específicos del nivel 3
}

void Nivel3::crearObstaculos()
{
    // Implementar creación de obstáculos específicos del nivel 3
}
