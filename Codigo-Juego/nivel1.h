
// ============ nivel1.h ============
#ifndef NIVEL1_H
#define NIVEL1_H

#include "nivelbase.h"
#include "persona.h"

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

    QList<Enemigo*> listaEnemigos;          // enemigos vivos que gestionamos
    int spawnDelayMs = 2000;                 // tiempo entre spawns cuando uno muere
    int spawnMargin = 120;                  // cuánto por encima del visible spawnear

    /*
     * FUNCIONES PARA LA APARICION DE ENEMIGOS.
     *
     * -Estas funciones son unicas de este nivel debido a sus propiedades de vision
     * y sus mecanicas
     *
     */

    //Aparicion de enemigos desde la parte superior de la pantalla
    void spawnearOleada();

    //tick principal del nivel
    void gameTick();

    //FUNCION PRIMORDIAL, elimina los enemigos que llegan al final de la pantalla
    void cleanupOffscreen();

    //REVISAR si el jugador y un enemigo colisionaron, para elminar dicho enemigo y aplicar la penalizacion de vida al jugador
    void revisarColision();
    void colisionDetectada(Enemigo* e);

private slots:
    void onEnemyDied(Persona* p);
};

#endif // NIVEL1_H
