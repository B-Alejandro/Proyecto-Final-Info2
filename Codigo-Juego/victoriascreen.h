#ifndef VICTORIASCREEN_H
#define VICTORIASCREEN_H

#include <QObject>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>

/**
 * Clase VictoriaScreen
 *
 * Pantalla que se muestra cuando el jugador alcanza
 * la puntuación objetivo y gana el nivel
 */
class VictoriaScreen : public QObject
{
    Q_OBJECT

public:
    explicit VictoriaScreen(QGraphicsScene* scene, QObject* parent = nullptr);
    ~VictoriaScreen();

    void mostrar(int puntosFinales);
    void ocultar();
    bool estaVisible() const { return visible; }

private:
    void crearElementos();

    QGraphicsScene* escena;
    QGraphicsRectItem* fondo;
    QGraphicsTextItem* textoVictoria;
    QGraphicsTextItem* textoPuntos;
    QGraphicsTextItem* textoReiniciar;
    QGraphicsTextItem* textoMenu;
    bool visible;
};

#endif // VICTORIASCREEN_H
