#ifndef PERSONA_H
#define PERSONA_H

#include <QGraphicsRectItem>
#include <QTimer>
#include <QKeyEvent>
#include <QPixmap>
#include <QRectF>
#include <QObject> // Asegurar que QObject está incluido

/*
 Persona.h
 Clase base para entidades con movimiento, fisica y animacion por sprites.
 Esta version declara boundingRect() para evitar warnings y errores de definicion fuera de linea.
*/

enum class TipoMovimiento {
    RECTILINEO,
    CON_GRAVEDAD
};

enum class EstadoAnimacion {
    IDLE,
    CORRIENDO,
    SALTANDO,
    MUERTO
};

class Persona : public QObject, public QGraphicsRectItem
{
    Q_OBJECT

public:
    void updateMovement();

    // Sobrescribe boundingRect para que Qt use el tamaño del sprite
    QRectF boundingRect() const override;

    Persona(qreal w, qreal h, qreal sceneWidth, qreal sceneHeight, TipoMovimiento tipo);

    // velocidad horizontal
    void setSpeed(double s) { speed = s; }
    double getSpeed() const { return speed; }

    // tipo de movimiento
    void setTipoMovimiento(TipoMovimiento tipo);
    TipoMovimiento getTipoMovimiento() const { return tipoMovimiento; }
    bool isOnGround() const { return onGround; }

    // sistema de vida
    void setVida(int vida) { vidaActual = vida; }
    int getVida() const { return vidaActual; }
    int getVidaMaxima() const { return vidaMaxima; }
    void recibirDanio(int cantidad);
    void curar(int cantidad);
    bool estaVivo() const { return vidaActual > 0; }
    bool estaInvulnerable() const { return invulnerable; }
    void iniciarInvulnerabilidad(int duration_ms);

    // manejo de sprites
    void setSprite(const QString& rutaImagen, int anchoFrame, int altoFrame, int numFrames);
    void setAnimacion(EstadoAnimacion estado);
    EstadoAnimacion getEstadoAnimacion() const { return estadoActual; }

    // direccion del sprite
    void setDireccionIzquierda(bool izquierda) { mirandoIzquierda = izquierda; }
    bool estaMirandoIzquierda() const { return mirandoIzquierda; }

    // pausar y reanudar animacion
    void pausarAnimacion();
    void reanudarAnimacion();
    bool estaAnimacionPausada() const { return animacionPausada; }

    // variables accesibles para entrada y fisica
    bool upPressed;
    bool downPressed;
    bool leftPressed;
    bool rightPressed;
    double vy;
    double g;
    bool onGround;

signals:
    void vidaCambiada(int vidaActual, int vidaMaxima);
    void murioPersona();
    void died(Persona* who);

protected:


    // paint renderiza el sprite
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

    // metodos de control
    virtual void handleInput();
    void updateMovementRectilineo();
    void updateMovementConGravedad();
    virtual void onEstadoAnimacionCambiado();

    // parametros principales
    TipoMovimiento tipoMovimiento;
    double sceneW;
    double sceneH;
    double speed;

    // timer de movimiento NO usado internamente; se gestiona desde NivelBase
    QTimer* timer;

    // sistema de sprites
    QPixmap spriteSheet;
    int anchoSprite;
    int altoSprite;
    int totalFrames;
    int frameActual;
    bool usarSprites;
    EstadoAnimacion estadoActual;
    QTimer* timerAnimacion;
    bool animacionPausada;
    bool mirandoIzquierda;

    // sistema de vida
    int vidaActual;
    int vidaMaxima;
    bool invulnerable;
    QTimer* timerInvulnerabilidad;

private slots:
    void actualizarAnimacion();
    void finalizarInvulnerabilidad();
};

#endif // PERSONA_H
