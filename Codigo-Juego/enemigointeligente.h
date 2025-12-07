#ifndef ENEMIGOINTELIGENTE_H
#define ENEMIGOINTELIGENTE_H

#include "enemigo.h"
#include <QGraphicsEllipseItem>
#include <QTimer>
#include <QPainterPath>

/*
  Clase EnemigoInteligente - TORRETA ESTÁTICA

  Enemigo 100% estático que dispara proyectiles.
  Sistema de agresividad progresiva:
  - Detección 1: Disparo lento (3s) - 1 proyectil
  - Detección 2: Disparo normal (2s) - 1 proyectil
  - Detección 3: Disparo rápido (1.5s) - 2 proyectiles
  - Detección 4+: Disparo muy rápido (1s) - 3 proyectiles

  HITBOX REDUCIDA: 60% del tamaño original para mejor jugabilidad
*/

class EnemigoInteligente : public Enemigo
{
    Q_OBJECT

public:
    EnemigoInteligente(qreal w,
                       qreal h,
                       qreal sceneWidth,
                       qreal sceneHeight,
                       TipoMovimiento tipo,
                       int nivel,
                       qreal radioDeteccion = 200.0);

    ~EnemigoInteligente();

    qreal getRadioDeteccion() const { return radioDeteccion; }
    void setRadioDeteccion(qreal radio);

    bool estaEnAlerta() const { return jugadorDetectado; }
    int getContadorDetecciones() const { return contadorDetecciones; }
    void reiniciarContador() { contadorDetecciones = 0; }
    void mostrarAreaDeteccion(bool mostrar);

    // *** NUEVOS: Overrides para hitbox más pequeña ***
    QRectF boundingRect() const override;
    QPainterPath shape() const override;

protected:
    void advance(int phase) override;
    void handleInput() override;  // Override para mantenerlo estático

private slots:
    void dispararProyectiles();

private:
    bool detectarJugador();
    class Jugador* obtenerJugador();

    void onJugadorDetectado();
    void onJugadorPerdido();
    void actualizarAgresividad();

    qreal radioDeteccion;
    bool jugadorDetectado;
    bool jugadorDetectadoAnterior;
    int contadorDetecciones;

    // Sistema de disparo progresivo
    QPointF ultimaPosicionJugador;
    QTimer* timerDisparo;
    int intervaloDisparo;        // Milisegundos entre disparos
    int cantidadProyectiles;     // Cantidad de proyectiles por ráfaga

    QGraphicsEllipseItem* areaDeteccionVisual;
    bool mostrarArea;
};

#endif // ENEMIGOINTELIGENTE_H
