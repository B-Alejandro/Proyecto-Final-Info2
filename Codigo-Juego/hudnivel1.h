#ifndef HUDNIVEL1_H
#define HUDNIVEL1_H

#include <QObject>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <QGraphicsPixmapItem>

/**
 * Clase HUDNivel1
 *
 * Interfaz visual que muestra:
 * - Vidas del jugador (corazones)
 * - Puntuación actual
 * - Puntuación objetivo
 */
class HUDNivel1 : public QObject
{
    Q_OBJECT

public:
    explicit HUDNivel1(QGraphicsScene* scene, QObject* parent = nullptr);
    ~HUDNivel1();

    /**
     * Actualiza la cantidad de vidas mostradas
     * @param vidas Cantidad actual de vidas (0-5)
     */
    void actualizarVidas(int vidas);

    /**
     * Actualiza la puntuación mostrada
     * @param puntos Puntos actuales
     * @param objetivo Puntos necesarios para ganar
     */
    void actualizarPuntuacion(int puntos, int objetivo);

    /**
     * Actualiza la posición del HUD para seguir la cámara
     * @param camaraX Posición X de la cámara
     */
    void actualizarPosicion(qreal camaraX, qreal camaraY);

private:
    void crearElementos();
    void crearCorazones();
    void crearTextoPuntuacion();

    QGraphicsScene* escena;

    // Elementos de vidas
    QList<QGraphicsRectItem*> corazones;  // Corazones visuales
    QGraphicsTextItem* textoVidas;

    // Elementos de puntuación
    QGraphicsTextItem* textoPuntos;
    QGraphicsRectItem* fondoHUD;

    int vidasActuales;
    int puntosActuales;
    int puntosObjetivo;

    // Posición base del HUD
    qreal offsetX;
    qreal offsetY;
};

#endif // HUDNIVEL1_H
