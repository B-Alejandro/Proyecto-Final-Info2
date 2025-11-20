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
    Jugador(qreal w, qreal h, qreal sceneWidth, qreal sceneHeight, TipoMovimiento tipo);

    void cargarSprites();

    // *** NUEVO: Método para activar animación de muerte ***
    void activarAnimacionMuerte();

protected:
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;

    // Sobrescribir método para cambiar sprites automáticamente
    void onEstadoAnimacionCambiado() override;

private:
    QPixmap spriteCorrer;
    QPixmap spriteSaltar;
    QPixmap spriteMuerte;  // *** NUEVO ***

    void cambiarSpritePorEstado();
};

#endif // JUGADOR_H
