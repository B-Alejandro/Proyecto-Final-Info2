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
            TipoMovimiento tipo);

protected:
    void handleInput() override;

private slots:
    void changeDirection();

private:
    void randomizeDirection();
    void tryJump();

private:
    QTimer* aiTimer;
    int changeDirectionTime;
    bool canJump;
};

#endif
