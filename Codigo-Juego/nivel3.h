// ============ nivel3.h - CON COLECCIONABLES ============
#ifndef NIVEL3_H
#define NIVEL3_H

#include "nivelbase.h"
#include <QList>

// Forward declaration
class Coleccionable;

class Nivel3 : public NivelBase
{
    Q_OBJECT

public:
    Nivel3(Juego* juego, QObject* parent = 0);

protected:
    void configurarNivel() override;
    void crearEnemigos() override;
    void crearObstaculos() override;
    void actualizar() override;

private slots:
    /**
     * Slot que se ejecuta cuando el jugador recoge un coleccionable
     */
    void onColeccionableRecolectado(Coleccionable* col);

private:
    /**
     * Muestra el mensaje de victoria
     */
    void mostrarVictoria();

    // Sistema de coleccionables
    QList<Coleccionable*> listaColeccionables;
    int coleccionablesRecolectados;
    int totalColeccionables;
    bool nivelGanado;
};

#endif // NIVEL3_H
