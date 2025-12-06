#ifndef PROYECTILINTELIGENTE_H
#define PROYECTILINTELIGENTE_H

#include <QGraphicsObject>
#include <QPointF>

class Persona;

/**
 * Clase ProyectilInteligente
 *
 * Proyectil direccional que se mueve en línea recta hacia
 * un punto objetivo específico (donde fue detectado el jugador).
 *
 * NO hereda de Proyectil para tener control total del movimiento.
 */
class ProyectilInteligente : public QGraphicsObject
{
    Q_OBJECT

public:
    /**
     * Constructor
     * @param puntoInicio: Posición inicial (centro del enemigo)
     * @param puntoObjetivo: Posición objetivo (donde estaba el jugador)
     * @param velocidad: Velocidad del proyectil en píxeles/frame
     * @param danio: Cantidad de daño que inflige
     */
    explicit ProyectilInteligente(QPointF puntoInicio,
                                  QPointF puntoObjetivo,
                                  qreal velocidad = 8.0,
                                  int danio = 1,
                                  QGraphicsItem* parent = nullptr);

    QRectF boundingRect() const override;

    void paint(QPainter* painter,
               const QStyleOptionGraphicsItem* option,
               QWidget* widget) override;

    /**
     * Establece el dueño del proyectil (para no dañarse a sí mismo)
     */
    void setOwner(Persona* owner);

protected:
    /**
     * Actualización por frame - mueve el proyectil y detecta colisiones
     */
    void advance(int phase) override;

private:
    qreal m_ancho;          // Ancho del proyectil
    qreal m_alto;           // Alto del proyectil
    qreal m_velocidad;      // Velocidad de movimiento
    int m_danio;            // Daño que inflige

    QPointF m_direccion;    // Vector normalizado de dirección
    qreal m_velocidadX;     // Componente X de la velocidad
    qreal m_velocidadY;     // Componente Y de la velocidad

    Persona* m_owner;       // Quien disparó (para no auto-dañarse)
    int m_framesVividos;    // Frames desde creación (para auto-destrucción)
    int m_framesMaximos;    // Frames máximos de vida (300 = ~5 segundos)
};

#endif // PROYECTILINTELIGENTE_H
