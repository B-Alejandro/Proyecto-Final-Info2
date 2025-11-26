#ifndef EXPLOSION_H
#define EXPLOSION_H

#include <QGraphicsObject>
#include <QTimer>

class Persona;

/**
 * Clase Explosion
 *
 * Área de daño temporal que:
 * - Se crea cuando un proyectil de tanque explota
 * - Daña a todos los que estén dentro del radio
 * - Desaparece después de un breve tiempo
 * - Tiene efecto visual de expansión
 */
class Explosion : public QGraphicsObject
{
    Q_OBJECT

public:
    /**
     * Constructor de la explosión
     * @param x Posición X del centro
     * @param y Posición Y del centro
     * @param radio Radio del área de daño
     * @param danio Cantidad de daño que inflige
     */
    explicit Explosion(qreal x,
                       qreal y,
                       qreal radio,
                       int danio,
                       QGraphicsItem* parent = nullptr);

    QRectF boundingRect() const override;
    void paint(QPainter* painter,
               const QStyleOptionGraphicsItem* option,
               QWidget* widget) override;

    void setOwner(Persona* owner);

protected:
    void advance(int phase) override;

private slots:
    /**
     * Elimina la explosión después de su duración
     */
    void autodestruir();

private:
    /**
     * Aplica daño a todas las entidades en el radio
     */
    void aplicarDanio();

    qreal m_radio;
    qreal m_radioActual;     // Radio actual (para animación de expansión)
    int m_danio;
    Persona* m_owner;
    QTimer* timerVida;

    // Animación
    int framesVividos;
    int framesMaximos;
    bool danioAplicado;      // Para aplicar daño solo una vez
};

#endif // EXPLOSION_H
