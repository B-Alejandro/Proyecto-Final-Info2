#ifndef ENEMIGOINTELIGENTE_H
#define ENEMIGOINTELIGENTE_H

#include "enemigo.h"
#include <QGraphicsEllipseItem>
#include <QTimer>

/*
  Clase EnemigoInteligente

  Enemigo con área de detección circular que reacciona progresivamente:
  - Detecciones 1-3: Persigue al jugador (velocidad reducida)
  - Detecciones 4+: Dispara proyectiles a la posición detectada
*/

class EnemigoInteligente : public Enemigo
{
    Q_OBJECT

public:
    /*
      Constructor del enemigo inteligente

      @param w, h: dimensiones del enemigo
      @param sceneWidth, sceneHeight: tamaño del escenario
      @param tipo: tipo de movimiento (gravedad o rectilíneo)
      @param nivel: nivel del juego
      @param radioDeteccion: radio del área de detección en píxeles
    */
    EnemigoInteligente(qreal w,
                       qreal h,
                       qreal sceneWidth,
                       qreal sceneHeight,
                       TipoMovimiento tipo,
                       int nivel,
                       qreal radioDeteccion = 200.0);

    ~EnemigoInteligente();

    /*
      Getters y Setters para el área de detección
    */
    qreal getRadioDeteccion() const { return radioDeteccion; }
    void setRadioDeteccion(qreal radio);

    /*
      Verifica si el enemigo está en estado de alerta
    */
    bool estaEnAlerta() const { return jugadorDetectado; }

    /*
      Obtiene el contador de detecciones
    */
    int getContadorDetecciones() const { return contadorDetecciones; }

    /*
      Reinicia el contador de detecciones
    */
    void reiniciarContador() { contadorDetecciones = 0; }

    /*
      Muestra/oculta el área de detección visual (para debug)
    */
    void mostrarAreaDeteccion(bool mostrar);

protected:
    /*
      Override del método de actualización para verificar
      la presencia del jugador en el área
    */
    void advance(int phase) override;

    /*
      Override del handleInput para persecución inteligente
    */
    void handleInput() override;

private slots:
    /*
      Dispara un proyectil hacia la última posición conocida del jugador
    */
    void dispararAObjetivo();

private:
    /*
      Verifica si el jugador está dentro del área de detección
      @return true si el jugador fue detectado
    */
    bool detectarJugador();

    /*
      Obtiene el puntero al jugador en la escena
    */
    class Jugador* obtenerJugador();

    /*
      Reacción cuando el jugador entra al área
    */
    void onJugadorDetectado();

    /*
      Reacción cuando el jugador sale del área
    */
    void onJugadorPerdido();

    /*
      Inicia el modo persecución (detecciones 1-3)
    */
    void activarPersecucion();

    /*
      Desactiva el modo persecución
    */
    void desactivarPersecucion();

    /*
      Inicia el modo francotirador (detecciones 4+)
    */
    void activarModoFrancotirador();

    // Radio del área de detección
    qreal radioDeteccion;
    // Estado de detección
    bool jugadorDetectado;
    bool jugadorDetectadoAnterior;
    // Sistema de conteo
    int contadorDetecciones;
    // Estados de comportamiento
    bool modoPersecucion;
    bool modoFrancotirador;
    qreal velocidadOriginal;

    // Sistema de disparo
    QPointF ultimaPosicionJugador;
    QTimer* timerDisparo;
    int intervaloDisparo;  // Milisegundos entre disparos

    // Visualización del área (para debug)
    QGraphicsEllipseItem* areaDeteccionVisual;
    bool mostrarArea;
};

#endif // ENEMIGOINTELIGENTE_H
