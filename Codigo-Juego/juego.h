#ifndef JUEGO_H
#define JUEGO_H

#include <QObject>
#include <QGraphicsView>
#include <QTimer>

class NivelBase;

class Juego : public QObject
{
    Q_OBJECT

public:
    Juego(QObject* parent = 0);
    ~Juego();

    void iniciar();
    void cambiarNivel(int id);
    QGraphicsView* getVista() { return vista; }
    int getVistaAncho() const { return vistaAncho; }
    int getVistaAlto() const { return vistaAlto; }

private slots:
    void actualizar();

private:
    void crearVista();
    void cargarNiveles();

private:
    QGraphicsView* vista;
    QTimer* timer;

    QList<NivelBase*> niveles;
    int nivelActual;

    int vistaAncho;
    int vistaAlto;
};

#endif
