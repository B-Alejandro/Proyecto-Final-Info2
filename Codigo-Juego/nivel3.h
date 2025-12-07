// ============ nivel3.h - COMPLETO Y CORREGIDO ============
#ifndef NIVEL3_H
#define NIVEL3_H

#include "nivelbase.h"
#include <QList>

// Forward declarations
class Coleccionable;
class PanelInfo;     // Para el HUD
class VictoriaScreen; // Para la pantalla de victoria

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
    void onColeccionableRecolectado(Coleccionable* col);

private:
    void mostrarVictoria();

    /**
     * Crea un muro rectangular simple (QGraphicsRectItem) con estilo urbano.
     */
    void crearMuro(qreal x, qreal y, qreal w, qreal h);

    /**
     * Crea elementos decorativos de fondo (líneas de calle).
     */
    void crearDecoraciones(); // <<< CORRECCIÓN: DECLARACIÓN AÑADIDA

    // Sistema de coleccionables
    QList<Coleccionable*> listaColeccionables;
    int coleccionablesRecolectados;
    int totalColeccionables;
    bool nivelGanado;

    // Panel de Información
    PanelInfo* infoPanel;

    // Gestión de puntuación local
    int puntuacionActual;

    // Pantalla de Victoria
    VictoriaScreen* victoriaScreen;
};

#endif // NIVEL3_H
