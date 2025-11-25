#ifndef ENEMIGO_H
#define ENEMIGO_H

#include "persona.h"
#include <QTimer>
#include <QPixmap>

/*
  Clase Enemigo
  Representa un enemigo controlado por IA simple.
  Posee sprites para diferentes estados y puede activarse
  o iniciar persecucion cuando el jugador se mueve.
*/

class Enemigo : public Persona
{
    Q_OBJECT

public:
    /*
      Constructor
      w,h: dimensiones del enemigo
      sceneWidth, sceneHeight: dimensiones del escenario
      tipo: tipo de movimiento (con gravedad o rectilineo)
      nivel: numero de nivel para ajustar la IA
    */
    Enemigo(qreal w,
            qreal h,
            qreal sceneWidth,
            qreal sceneHeight,
            TipoMovimiento tipo,
            int nivel);

    /*
      Carga todos los sprites del enemigo y calcula sus frames.
      NOTA: Este método se llama "cargarSprites" (sin "nivel2")
    */
    void cargarSprites();

    /*
      Activa animacion de muerte y detiene el movimiento.
    */
    void activarAnimacionMuerte();

    /*
      Activa la persecucion hacia el jugador.
      Se activa solo cuando el jugador se mueve.
    */
    void activarPersecucion();

    /*
      Devuelve true si el enemigo esta activo.
    */
    bool estaActivo() const { return enemigoActivo; }

protected:
    /*
      Maneja la logica interna cuando el enemigo tiene gravedad.
      Aqui se evalua si debe saltar o no ante un obstaculo.
    */
    void handleInput() override;

    /*
      Sobrescribe el metodo base para actualizar el sprite
      segun el nuevo estado de animacion.
    */
    void onEstadoAnimacionCambiado() override;

private slots:
    /*
      Cambia la direccion del enemigo segun IA.
      Solo aplica cuando esta activo.
    */
    void changeDirection();

private:
    void randomizeDirection();
    void tryJump();
    void tryJumpIfObstacleAhead();
    bool detectarObstaculoAdelante();
    void cambiarSpritePorEstado();

    // Timer para IA aleatoria
    QTimer* aiTimer;

    // Tiempo entre cambios de direccion
    int changeDirectionTime;

    // Indica si puede saltar
    bool canJump;

    // Numero de nivel
    int numeroNivel;

    // Controla si el enemigo esta activo
    bool enemigoActivo;

    // Sprites de cada estado
    QPixmap spriteIdle;
    QPixmap spriteCorrer;
    QPixmap spriteSaltar;
    QPixmap spriteMuerte;
};

#endif // ENEMIGO_H
