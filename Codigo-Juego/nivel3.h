#ifndef NIVEL3_H
#define NIVEL3_H

#include "nivelbase.h"

/*
  Clase Nivel3
  Define un nivel con múltiples enemigos y obstáculos
*/
class Nivel3 : public NivelBase
{
    Q_OBJECT

public:
    Nivel3(QGraphicsScene* escena, QObject* parent = 0);

protected:
    /*
      Configura los elementos específicos de este nivel.
    */
    void configurarNivel() override;

    /*
      Crea los enemigos del nivel 3.
    */
    void crearEnemigos() override;

    /*
      Crea los obstáculos del nivel 3.
    */
    void crearObstaculos() override;

    /*
      Actualiza la lógica del nivel según el tiempo.
    */
    void actualizar() override;
};

#endif
