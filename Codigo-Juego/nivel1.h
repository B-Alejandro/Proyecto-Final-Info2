// ============ nivel1.h ============
#ifndef NIVEL1_H
#define NIVEL1_H

#include "nivelbase.h"
#include "persona.h"
#include "GameOverScreen.h"
#include "hudnivel1.h"
#include "victoriascreen.h"

class Tanque;

class Nivel1 : public NivelBase
{
    Q_OBJECT

public:
    Nivel1(Juego* juego, QObject* parent = 0);

    void manejarTecla(Qt::Key key);
    bool estaEnGameOver() const { return juegoEnPausa; }
    bool estaEnVictoria() const { return nivelGanado; }

protected:
    void configurarNivel() override;
    void crearEnemigos() override;
    void crearObstaculos() override;
    void actualizar() override;  // *** NUEVO: Override para actualizar HUD ***

private:

    int vistaAncho;
    int vistaAlto;

    QList<Enemigo*> listaEnemigos;
    QList<Tanque*> listaTanques;
    QList<Obstaculo*> listaObstaculosMoviles;  // *** NUEVO ***

    int spawnDelayMs = 2000;
    int spawnMargin = 120;

    // Sistema de Game Over y Victoria
    GameOverScreen* pantallaGameOver;
    VictoriaScreen* pantallaVictoria;  // *** NUEVO ***
    HUDNivel1* hud;  // *** NUEVO: Interfaz visual ***
    bool juegoEnPausa;
    bool nivelGanado;  // *** NUEVO ***

    // *** NUEVO: Sistema de puntuación ***
    int puntosActuales;
    int puntosObjetivo;

    // Funciones de oleadas
    void spawnearOleada();
    void gameTick();
    void cleanupOffscreen();

    // Colisiones
    void revisarColision();
    void colisionDetectada(Enemigo* e);
    void colisionTanqueDetectada(Tanque* t);
    void colisionObstaculoDetectada(Obstaculo* o);  // *** NUEVO ***

    // Estado del jugador
    void verificarEstadoJugador();

    // *** NUEVO: Sistema de puntos ***
    void agregarPuntos(int cantidad);
    void verificarVictoria();

private slots:
    void onEnemyDied(Persona* p);
    void onTankDied(Persona* p);
    void onJugadorMurio();
    void onObstaculoDestruido(Obstaculo* obs);  // *** NUEVO ***
    void onJugadorDaniado(int vidaActual, int vidaMax);  // *** NUEVO ***
};

#endif // NIVEL1_H
