// ============ fisica.h ============
#ifndef FISICA_H
#define FISICA_H

#include <QGraphicsItem>
#include <QList>

class Fisica
{
public:
    static bool colisionan(QGraphicsItem* a, QGraphicsItem* b);
    static QList<QGraphicsItem*> obtenerColisiones(QGraphicsItem* item);
    static bool colisionDebajo(QGraphicsItem* item);
    static bool hayColisionConObstaculos(QGraphicsItem* item);

    static double resolverColisionHorizontal(QGraphicsItem* item,
                                             double prevX,
                                             double sceneW);

    // Función para manejar el empuje / corrección de colisión horizontal
    static void corregirColisionHorizontal(QGraphicsItem* item,
                                           QGraphicsItem* otro,
                                           double prevX);

    static double resolverColisionVertical(QGraphicsItem* item,
                                           double prevY,
                                           double sceneH,
                                           double& vy,
                                           bool& onGround);

    static void aplicarMovimientoRectilineo(QGraphicsItem* item,
                                            double dx,
                                            double dy,
                                            double sceneW,
                                            double sceneH);

    static void aplicarMovimientoConGravedad(QGraphicsItem* item,
                                             double dx,
                                             double& vy,
                                             double gravedad,
                                             bool& onGround,
                                             double sceneW,
                                             double sceneH);
};

#endif // FISICA_H
