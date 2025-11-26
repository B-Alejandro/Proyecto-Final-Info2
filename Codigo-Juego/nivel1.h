// ============ nivel1.h ============
#ifndef NIVEL1_H
#define NIVEL1_H

#include "nivelbase.h"
#include "persona.h"
#include "GameOverScreen.h"  // *** NUEVO: Incluir Game Over ***

class Tanque;  // Forward declaration

class Nivel1 : public NivelBase
{
    Q_OBJECT

public:
    Nivel1(Juego* juego, QObject* parent = 0);

    // *** NUEVO: Manejo de teclas para Game Over ***
    void manejarTecla(Qt::Key key);
    bool estaEnGameOver() const { return juegoEnPausa; }

protected:
    void configurarNivel() override;
    void crearEnemigos() override;
    void crearObstaculos() override;

private:

    int vistaAncho;
    int vistaAlto;

    QList<Enemigo*> listaEnemigos;
    QList<Tanque*> listaTanques;
    int spawnDelayMs = 2000;
    int spawnMargin = 120;

    // *** NUEVO: Sistema de Game Over ***
    GameOverScreen* pantallaGameOver;
    bool juegoEnPausa;

    // Funciones de oleadas
    void spawnearOleada();
    void gameTick();
    void cleanupOffscreen();

    // Colisiones
    void revisarColision();
    void colisionDetectada(Enemigo* e);
    void colisionTanqueDetectada(Tanque* t);

    // *** NUEVO: Verificar estado del jugador ***
    void verificarEstadoJugador();

private slots:
    void onEnemyDied(Persona* p);
    void onTankDied(Persona* p);
    void onJugadorMurio();  // *** NUEVO ***
};

#endif // NIVEL1_H
