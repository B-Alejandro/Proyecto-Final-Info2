
// ============ nivel1.h ============
#ifndef NIVEL1_H
#define NIVEL1_H

#include "nivelbase.h"

class Nivel1 : public NivelBase
{
    Q_OBJECT

public:
    Nivel1(Juego* juego, QObject* parent = 0);

protected:
    void configurarNivel() override;
    void crearEnemigos() override;
    void crearObstaculos() override;
};

#endif // NIVEL1_H
