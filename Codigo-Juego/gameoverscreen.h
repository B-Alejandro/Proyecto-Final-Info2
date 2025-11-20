// ============ GameOverScreen.h ============
#ifndef GAMEOVERSCREEN_H
#define GAMEOVERSCREEN_H

#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <QGraphicsScene>

class GameOverScreen : public QObject
{
    Q_OBJECT
public:
    explicit GameOverScreen(QGraphicsScene* scene, QObject* parent = nullptr);
    ~GameOverScreen();

    void mostrar();
    void ocultar();
    bool estaVisible() const { return visible; }

signals:
    void reiniciarClicked();
    void menuClicked();

private:
    QGraphicsScene* escena;
    QGraphicsRectItem* fondo;
    QGraphicsTextItem* textoGameOver;
    QGraphicsTextItem* textoCapturado;
    QGraphicsTextItem* textoReiniciar;
    QGraphicsTextItem* textoMenu;
    bool visible;

    void crearElementos();
};

#endif // GAMEOVERSCREEN_H
