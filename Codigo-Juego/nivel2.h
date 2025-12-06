#ifndef NIVEL2_H
#define NIVEL2_H

#include "nivelbase.h"
#include <QList>
#include <QTimer>
#include <QElapsedTimer> // Necesario para danoObstaculoTimer y tiempoJugado
#include <QGraphicsPixmapItem>

// INCLUSIONES CRÍTICAS
#include "victoriascreen.h"
#include "persona.h"
#include <QWidget>
#include <QLabel>
#include "gameoverscreen.h" // <-- CORRECCIÓN CASE-SENSITIVITY (Minúsculas)

// Declaraciones adelantadas...

class Nivel2 : public NivelBase
{
    Q_OBJECT

public:
    Nivel2(Juego* juego, QObject* parent = nullptr);
    ~Nivel2() override;

    void manejarTecla(Qt::Key key) ;
    bool estaEnGameOver() const;
    bool estaEnVictoria() const { return nivelGanado; }

protected:
    void configurarNivel() override;
    void crearEnemigos() override;
    void crearObstaculos() override;
    void actualizar() override;
    void reiniciarNivel();

signals:
    void juegoTerminado();
    void volverAMenuPrincipal();

private slots:
    void onJuegoTerminado();
    void verificarVictoriaPorTiempo();
    void onEnemyDied(Persona* p); // <-- Declaración del slot que faltaba implementar

private:
    // ... (Métodos auxiliares)

    // ================================================================
    // ** MIEMBROS DE ESTADO Y UI (Reordenados para -Wreorder-ctor) **
    // ================================================================
    GameOverScreen* pantallaGameOver;
    VictoriaScreen* pantallaVictoria;
    QTimer* timerVictoria;

    bool juegoEnPausa;
    bool nivelGanado;

    // Timers de juego y cooldown
    QElapsedTimer danoObstaculoTimer;
    QElapsedTimer tiempoJugado; // <-- Variable declarada para el error anterior

    // Elementos del HUD
    QWidget* hudWidget;
    QLabel* vidaLabel;
    QLabel* tiempoLabel;
    QLabel* scoreLabel;

    // ================================================================
    // ** RESTO DE MIEMBROS DEL NIVEL **
    // ================================================================

    Enemigo* enemigoAtras;
    Obstaculo* suelo1;
    Obstaculo* suelo2;
    Obstaculo* suelo3;

    QGraphicsPixmapItem* fondo1;
    QGraphicsPixmapItem* fondo2;
    QGraphicsPixmapItem* fondo3;

    int anchoFondo;
    int anchoSuelo;
    int vistaAncho;
    int vistaAlto;

    qreal distanciaEntreObstaculos;
    qreal ultimaPosicionSpawn;
    int contadorObstaculos;
private:
    // ** CORRECCIÓN 1: Agregar los métodos auxiliares faltantes **
    // --- Gestión de Corredor Infinito Triple (Métodos auxiliares) ---
    void crearFondoDinamico();
    void actualizarFondo(qreal posicionJugador);
    void crearSueloTriple();
    void actualizarSuelo(qreal posicionJugador);
    void spawnearObstaculoAleatorio();
    void limpiarObstaculosLejanos();
    void reposicionarEscena();
    // Métodos de colisión...
};

#endif // NIVEL2_H
