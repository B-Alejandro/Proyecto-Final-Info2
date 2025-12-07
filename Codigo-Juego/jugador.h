// ============ jugador.h ============
#ifndef JUGADOR_H
#define JUGADOR_H

#include "persona.h"
#include <QKeyEvent>
#include <QPixmap>

class Jugador : public Persona
{
    Q_OBJECT
public:
    void saltar();
    Jugador(qreal w, qreal h, qreal sceneWidth, qreal sceneHeight, TipoMovimiento tipo);

    void cargarSpritesnivel2();

    // Metodo para activar animacion de muerte
    void activarAnimacionMuerte();
void cargarSpritesNivel1();
    void cargarSpritesNivel3();
protected:
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;

    // Sobrescribir metodo para cambiar sprites automaticamente
    void onEstadoAnimacionCambiado() override;

private:
    QPixmap spriteIdle;    // *** AGREGADO ***
    QPixmap spriteCorrer;
    QPixmap spriteSaltar;
    QPixmap spriteMuerte;

    void cambiarSpritePorEstado();
};

#endif // JUGADOR_H
