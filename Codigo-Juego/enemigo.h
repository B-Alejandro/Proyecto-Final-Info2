#ifndef ENEMIGO_H
#define ENEMIGO_H

#include "persona.h"
#include <QTimer>

class Enemigo : public Persona
{
    Q_OBJECT

public:
    Enemigo(qreal w,
            qreal h,
            qreal sceneWidth,
            qreal sceneHeight,
            TipoMovimiento tipo,
            int nivel);

protected:
    void handleInput() override;

private slots:
    void changeDirection();

private:
    void randomizeDirection();
    void tryJump(); // Método anterior (salto aleatorio)
    void tryJumpIfObstacleAhead(); // Nuevo método: salto inteligente
    bool detectarObstaculoAdelante(); // Detecta obstáculos adelante


    QTimer* aiTimer;
    int changeDirectionTime;
    bool canJump;
    int numeroNivel;
};

#endif // ENEMIGO_H
