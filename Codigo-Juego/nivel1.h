// ============ nivel1.h - MODIFICADO CON TANQUES ============
#ifndef NIVEL1_H
#define NIVEL1_H

#include "nivelbase.h"
#include "persona.h"

class Tanque;  // Forward declaration

class Nivel1 : public NivelBase
{
    Q_OBJECT

public:
    Nivel1(Juego* juego, QObject* parent = 0);

protected:
    void configurarNivel() override;
    void crearEnemigos() override;
    void crearObstaculos() override;

private:

    int vistaAncho;
    int vistaAlto;

    QList<Enemigo*> listaEnemigos;          // Enemigos vivos
    QList<Tanque*> listaTanques;            // *** NUEVO: Tanques vivos ***
    int spawnDelayMs = 2000;
    int spawnMargin = 120;

    // Funciones de oleadas
    void spawnearOleada();
    void gameTick();
    void cleanupOffscreen();

    // Colisiones
    void revisarColision();
    void colisionDetectada(Enemigo* e);
    void colisionTanqueDetectada(Tanque* t);  // *** NUEVA ***

private slots:
    void onEnemyDied(Persona* p);
    void onTankDied(Persona* p);  // *** NUEVO ***
};

#endif // NIVEL1_H
