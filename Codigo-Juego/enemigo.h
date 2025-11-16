#ifndef ENEMIGO_H
#define ENEMIGO_H

#include "persona.h"
#include <QTimer>

class Enemigo : public Persona
{
    Q_OBJECT

public:
    Enemigo(qreal w, qreal h,
            qreal sceneWidth, qreal sceneHeight,
            TipoMovimiento tipo = TipoMovimiento::CON_GRAVEDAD);

    // Configurar el comportamiento del enemigo
    void setChangeDirectionInterval(int ms) { changeDirectionTime = ms; }
    void setCanJump(bool can) { canJump = can; }

protected:
    void handleInput() override;  // IA del enemigo

private slots:
    void changeDirection();

private:
    QTimer *aiTimer;
    int changeDirectionTime;
    bool canJump;

    void randomizeDirection();
    void tryJump();
};

#endif // ENEMIGO_H
