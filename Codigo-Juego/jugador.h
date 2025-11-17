#ifndef JUGADOR_H
#define JUGADOR_H

#include "persona.h"

class Jugador : public Persona
{
public:
    Jugador(qreal w,
            qreal h,
            qreal sceneWidth,
            qreal sceneHeight,
            TipoMovimiento tipo);

protected:
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;
};

#endif
