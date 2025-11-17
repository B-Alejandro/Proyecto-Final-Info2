#ifndef FISICA_H
#define FISICA_H

#include <QGraphicsItem>
#include <QList>

class Fisica
{
public:
    // Detecta si dos items chocan usando bounding boxes
    static bool colisionan(QGraphicsItem* a, QGraphicsItem* b);

    // Devuelve una lista de items que colisionan con "item"
    static QList<QGraphicsItem*> obtenerColisiones(QGraphicsItem* item);

    // Devuelve true si debajo del item hay una plataforma rectangular
    static bool colisionDebajo(QGraphicsItem* item);

    // Ajusta el movimiento horizontal y devuelve la nueva posicion X
    static double resolverColisionHorizontal(QGraphicsItem* item,
                                             double prevX,
                                             double sceneW);

    // Ajusta el movimiento vertical y devuelve la nueva posicion Y
    static double resolverColisionVertical(QGraphicsItem* item,
                                           double prevY,
                                           double sceneH,
                                           double& vy,
                                           bool& onGround);

    // Nuevo: Maneja el movimiento rectilíneo con colisiones
    static void aplicarMovimientoRectilineo(QGraphicsItem* item,
                                            double dx,
                                            double dy,
                                            double sceneW,
                                            double sceneH);

    // Nuevo: Maneja el movimiento con gravedad completo
    static void aplicarMovimientoConGravedad(QGraphicsItem* item,
                                             double dx,
                                             double& vy,
                                             double gravedad,
                                             bool& onGround,
                                             double sceneW,
                                             double sceneH);

    // Nuevo: Verifica si hay colisión con obstáculos rectangulares
    static bool hayColisionConObstaculos(QGraphicsItem* item);
};

#endif
