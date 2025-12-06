#ifndef JUEGO_H
#define JUEGO_H

#include <QObject>
#include <QGraphicsView>
#include <QTimer>
#include <QKeyEvent>
#include <QEvent>

// Declaraciones adelantadas
class NivelBase;
class MenuPrincipal; // *** AGREGADO ***

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

    // *** NUEVO: Método para volver al menú principal ***
    void mostrarMenuPrincipal();

protected:
    // Capturar eventos de teclado globales (para la tecla ESC)
    bool eventFilter(QObject* obj, QEvent* event) override;

private slots:
    void actualizar();

private:
    void crearVista();
    void cargarNiveles();

private:
    QGraphicsView* vista;
    QTimer* timer;

    QList<NivelBase*> niveles;
    int nivelActual; // -1: Menu, 0: Nivel1, 1: Nivel2, ...

    int vistaAncho;
    int vistaAlto;

    // *** MIEMBRO AGREGADO ***
    MenuPrincipal* menuPrincipal;
};

#endif
