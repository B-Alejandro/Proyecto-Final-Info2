#ifndef NIVEL1_H
#define NIVEL1_H

#include "nivelbase.h"

/*
  Clase Nivel1
  Define un nivel simple con movimiento rectilíneo
*/
class Nivel1 : public NivelBase
{
    Q_OBJECT

public:
    Nivel1(QGraphicsScene* escena, QObject* parent = 0);

protected:
    /*
      Configura los elementos específicos de este nivel.
    */
    void configurarNivel() override;

    /*
      Crea los enemigos del nivel 1.
    */
    void crearEnemigos() override;

    /*
      Crea los obstáculos del nivel 1.
    */
    void crearObstaculos() override;

    /*
      Actualiza la lógica del nivel según el tiempo.
    */
    void actualizar() override;

private:

    QList<Enemigo*> listaEnemigos;          // enemigos vivos que gestionamos
    int spawnDelayMs = 800;                 // tiempo entre spawns cuando uno muere
    int spawnMargin = 120;                  // cuánto por encima del visible spawnear

    /*
     * FUNCIONES PARA LA APARICION DE ENEMIGOS.
     *
     * -Estas funciones son unicas de este nivel debido a sus propiedades de vision
     * y sus mecanicas
     *
     */

    //Aparicion de enemigos desde la parte superior de la pantalla
    void spawnEnemyAboveView();

    //tick principal del nivel
    void gameTick();

    //FUNCION PRIMORDIAL, elimina los enemigos que llegan al final de la pantalla
    void cleanupOffscreen();

};

#endif
