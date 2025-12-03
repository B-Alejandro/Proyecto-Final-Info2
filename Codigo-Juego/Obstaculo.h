// ============ obstaculo.h ============
#ifndef OBSTACULO_H
#define OBSTACULO_H

#include <QGraphicsRectItem>
#include <QObject>
#include <QColor>
#include <QPixmap>
#include <QPainter>

// Identificador para clasificar el tipo de objeto fisico
enum class TipoFisico {
    PERSONA,
    PROYECTIL,
    OBSTACULO
};

class Obstaculo : public QObject, public QGraphicsRectItem
{
    Q_OBJECT

public:
    Obstaculo(qreal x,
              qreal y,
              qreal w,
              qreal h,
              QColor color,
              bool esMovil = false); // Constructor unificado con flag móvil

    void setColor(const QColor& color);
    void setBorderColor(const QColor& color, int width);

    // Metodo para obtener el tipo fisico
    TipoFisico getTipoFisico() const { return TipoFisico::OBSTACULO; }

    // Métodos para agregar textura/imagen
    void setTextura(const QString& rutaImagen, bool repetir = true);
    void setTextura(const QPixmap& pixmap, bool repetir = true);
    void limpiarTextura(); // Volver a usar solo color

    // *** Sistema de vida y movimiento ***
    void setMovil(bool movil) { esMovil = movil; }
    bool isMovil() const { return esMovil; }

    void setVida(int vida) { vidaActual = vida; vidaMaxima = vida; }
    int getVida() const { return vidaActual; }
    bool estaVivo() const { return vidaActual > 0; }
    void recibirDanio(int cantidad);

    void setVelocidad(qreal vel) { velocidad = vel; }
    qreal getVelocidad() const { return velocidad; }

    void setDanioColision(int danio) { danioAlJugador = danio; }
    int getDanioColision() const { return danioAlJugador; }

    // Actualizar posición (llamar cada frame)
    void actualizar(qreal sceneH);

signals:
    // REVERTIDO: Vuelve a usar Obstaculo* para que coincida con el slot en nivel1.cpp
    void obstaculoMuerto(Obstaculo* obs);

protected:
    // Sobrescribir paint para dibujar la textura
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:
    QColor currentColor;
    bool tieneTextura;
    QPixmap texturaPixmap;
    bool repetirTextura;

    // Atributos de obstáculo móvil, vida y daño (unificados)
    bool esMovil;
    int vidaActual = 0;
    int vidaMaxima = 0;
    qreal velocidad = 0;
    int danioAlJugador = 0;
};

#endif // OBSTACULO_H
