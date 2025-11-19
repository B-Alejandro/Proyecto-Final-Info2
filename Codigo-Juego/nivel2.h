// ============ nivel2.h ============
#ifndef NIVEL2_H
#define NIVEL2_H

#include "nivelbase.h"

class Nivel2 : public NivelBase
{
    Q_OBJECT

public:
    Nivel2(Juego* juego, QObject* parent = 0);

protected:
    void configurarNivel() override;
    void crearEnemigos() override;
    void crearObstaculos() override;
    void actualizar() override;

private:
    int vistaAncho;
    int vistaAlto;
    Enemigo* enemigoAtras; // Enemigo que persigue por detrás
    Obstaculo* suelo; // Suelo del nivel
};

#endif // NIVEL2_H
