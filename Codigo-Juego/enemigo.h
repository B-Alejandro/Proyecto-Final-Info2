#ifndef ENEMIGO_H
#define ENEMIGO_H

#include "persona.h"
#include <QTimer>
#include <QPixmap>

/*
  Clase Enemigo
  Representa un enemigo controlado por IA simple o avanzada.
  Incluye movimiento, sprites y sistema opcional de disparo.
*/

class Enemigo : public Persona
{
    Q_OBJECT

public:
    /*
      Constructor general del enemigo
      w, h: dimensiones
      sceneWidth, sceneHeight: tamano del escenario
      tipo: tipo de movimiento (gravedad o rectilineo)
      nivel: nivel del juego que ajusta la IA
    */
    QTimer* timerDisparo;
    Enemigo(qreal w,
            qreal h,
            qreal sceneWidth,
            qreal sceneHeight,
            TipoMovimiento tipo,
            int nivel);

    /*
      Carga sprites del enemigo y calcula frames.
    */
      QTimer* aiTimer;
    void cargarSprites();
    void cargarSpritesNivel1();
    /*
      Activa animacion de muerte y detiene movimiento.
    */
    void activarAnimacionMuerte();

    /*
      Activa persecucion hacia el jugador cuando este se mueve.
    */
    void activarPersecucion();

    /*
      Indica si el enemigo esta activo.
    */
    bool estaActivo() const { return enemigoActivo; }

protected:
    /*
      Logica interna cuando el enemigo usa gravedad.
      Decide si debe saltar al acercarse a un obstaculo.
    */
    void handleInput() override;

    /*
      Actualiza sprite segun el estado de animacion.
    */
    void onEstadoAnimacionCambiado() override;

private slots:
    /*
      Cambia la direccion del enemigo segun IA.
    */
    void changeDirection();

    /*
      Intenta disparar un proyectil segun su temporizador.
    */
    void intentarDisparar();

private:
    void randomizeDirection();
    void tryJump();
    void tryJumpIfObstacleAhead();
    bool detectarObstaculoAdelante();
    void cambiarSpritePorEstado();

    // Timer para IA aleatoria


    // Tiempo entre cambios de direccion
    int changeDirectionTime;

    // Habilidad para saltar
    bool canJump;

    // Numero de nivel
    int numeroNivel;

    // Estado activo o inactivo del enemigo
    bool enemigoActivo;

    // Sprites
    QPixmap spriteIdle;
    QPixmap spriteCorrer;
    QPixmap spriteSaltar;
    QPixmap spriteMuerte;

    // Sistema de disparo

    int tiempoEntreDisparos;
    void dispararProyectil();
};

#endif // ENEMIGO_H
