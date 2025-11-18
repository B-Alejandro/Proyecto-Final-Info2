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
};

#endif
