#ifndef PROYECTILTANQUE_H
#define PROYECTILTANQUE_H

#include <QGraphicsObject>
#include <QTimer>

class Persona;

/**
 * Clase ProyectilTanque
 *
 * Proyectil con movimiento parabólico que:
 * - Se mueve en trayectoria parabólica desde un extremo al otro
 * - Al llegar al borde o colisionar, explota
 * - La explosión crea un área de daño temporal
 */
class ProyectilTanque : public QGraphicsObject
{
    Q_OBJECT

public:
    /**
     * Constructor del proyectil parabólico
     * @param width Ancho del proyectil
     * @param height Alto del proyectil
     * @param sceneWidth Ancho de la escena
     * @param sceneHeight Alto de la escena
     * @param desdeIzquierda true si dispara desde la izquierda, false desde derecha
     */
    explicit ProyectilTanque(qreal width,
                             qreal height,
                             qreal sceneWidth,
                             qreal sceneHeight,
                             bool desdeIzquierda,
                             QGraphicsItem* parent = nullptr);

    QRectF boundingRect() const override;
    void paint(QPainter* painter,
               const QStyleOptionGraphicsItem* option,
               QWidget* widget) override;

    void setOwner(Persona* owner);

protected:
    void advance(int phase) override;

private:
    /**
     * Crea la explosión en la posición actual
     */
    void explotar();

    qreal m_w;
    qreal m_h;
    qreal sceneW;
    qreal sceneH;

    // Física del movimiento parabólico
    qreal velocidadX;      // Velocidad horizontal
    qreal velocidadY;      // Velocidad vertical inicial
    qreal gravedad;        // Gravedad del proyectil
    qreal tiempoVuelo;     // Tiempo transcurrido

    bool desdeIzquierda;   // Dirección de disparo
    Persona* m_owner;      // Quién disparó (para no dañarse a sí mismo)
    bool explotado;        // Para evitar múltiples explosiones
};

#endif // PROYECTILTANQUE_H
