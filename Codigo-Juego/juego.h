#ifndef JUEGO_H
#define JUEGO_H

#include <QObject>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QTimer>
#include <QList>
#include "nivelbase.h"

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

    QList<NivelBase*> niveles;
    int nivelActual;  // Este debe ir al final
};

#endif
