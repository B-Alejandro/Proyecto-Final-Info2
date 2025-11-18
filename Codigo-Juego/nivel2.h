#ifndef NIVEL2_H
#define NIVEL2_H

#include "nivelbase.h"

/*
  Clase Nivel2
  Define un nivel con gravedad y scroll horizontal
*/
class Nivel2 : public NivelBase
{
    Q_OBJECT

public:
    Nivel2(QGraphicsScene* escena, QObject* parent = 0);

protected:
    /*
      Configura los elementos específicos de este nivel.
    */
    void configurarNivel() override;

    /*
      Crea los enemigos del nivel 2.
    */
    void crearEnemigos() override;

    /*
      Crea los obstáculos del nivel 2.
    */
    void crearObstaculos() override;

    /*
      Actualiza con scroll horizontal.
    */
    void actualizar() override;
};

#endif
