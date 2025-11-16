#ifndef JUGADOR_H
#define JUGADOR_H

#include "persona.h"
#include <QKeyEvent>

class Jugador : public Persona
{
    Q_OBJECT

public:
    Jugador(qreal w, qreal h,
            qreal sceneWidth, qreal sceneHeight,
            TipoMovimiento tipo = TipoMovimiento::RECTILINEO);

protected:
    // Sobrescribir los eventos de teclado para controlar al jugador
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
};

#endif // JUGADOR_H
