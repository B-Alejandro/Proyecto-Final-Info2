// ============ nivel3.h ============
#ifndef NIVEL3_H
#define NIVEL3_H

#include "nivelbase.h"

class Nivel3 : public NivelBase
{
    Q_OBJECT

public:
    Nivel3(Juego* juego, QObject* parent = 0);

protected:
    void configurarNivel() override;
    void crearEnemigos() override;
    void crearObstaculos() override;

    void actualizar() override;
};

#endif // NIVEL3_H
