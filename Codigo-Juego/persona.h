// ============ persona.h ============
#ifndef PERSONA_H
#define PERSONA_H

#include <QGraphicsRectItem>
#include <QTimer>
#include <QKeyEvent>
#include <QPixmap>

enum class TipoMovimiento {
    RECTILINEO,
    CON_GRAVEDAD
};

enum class EstadoAnimacion {
    IDLE,
    CORRIENDO,
    SALTANDO,
    MUERTO  // *** NUEVO ***
};

class Persona : public QObject, public QGraphicsRectItem
{
    Q_OBJECT
public:
    Persona(qreal w, qreal h, qreal sceneWidth, qreal sceneHeight, TipoMovimiento tipo);

    void setSpeed(double s) { speed = s; }
    double getSpeed() const { return speed; }
    void setTipoMovimiento(TipoMovimiento tipo);
    TipoMovimiento getTipoMovimiento() const { return tipoMovimiento; }
    bool isOnGround() const { return onGround; }

    // Manejo de sprites
    void setSprite(const QString& rutaImagen, int anchoFrame, int altoFrame, int numFrames);
    void setAnimacion(EstadoAnimacion estado);
    EstadoAnimacion getEstadoAnimacion() const { return estadoActual; }

    // *** NUEVO: Métodos para pausar/reanudar animaciones ***
    void pausarAnimacion();
    void reanudarAnimacion();
    bool estaAnimacionPausada() const { return animacionPausada; }

protected:
    virtual void handleInput();
    void updateMovementRectilineo();
    void updateMovementConGravedad();

    // Método virtual para notificar cambios de estado
    virtual void onEstadoAnimacionCambiado();

    // Renderizado
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    TipoMovimiento tipoMovimiento;
    double sceneW, sceneH;
    double speed;

    bool upPressed, downPressed, leftPressed, rightPressed;
    double vy;
    double g;
    bool onGround;

    QTimer* timer;

    // Sistema de sprites
    QPixmap spriteSheet;
    int anchoSprite;
    int altoSprite;
    int totalFrames;
    int frameActual;
    bool usarSprites;
    EstadoAnimacion estadoActual;
    QTimer* timerAnimacion;
    bool animacionPausada;  // *** NUEVO ***

private slots:
    void updateMovement();
    void actualizarAnimacion();
};

#endif // PERSONA_H
