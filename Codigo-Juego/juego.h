#ifndef JUEGO_H
#define JUEGO_H

#include <QObject>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QTimer>

class Nivel;

class Juego : public QObject
{
    Q_OBJECT

public:
    Juego(QObject* parent = 0);
    ~Juego();

    void iniciar();
    void cambiarNivel(int id);

private slots:
    void actualizar();

private:
    void crearVista();
    void cargarNiveles();

private:
    QGraphicsView* vista;
    QGraphicsScene* escena;
    QTimer* timer;

    QList<Nivel*> niveles;
    int nivelActual;
};

#endif
