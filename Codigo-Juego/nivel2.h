#ifndef NIVEL2_H
#define NIVEL2_H

#include "nivelbase.h"
#include <QList>
#include <QTimer>
#include <QElapsedTimer>
#include <QGraphicsPixmapItem>
#include <QWidget>
#include <QLabel>
#include "persona.h"

// INCLUSIONES CRÍTICAS
#include "gameoverscreen.h"
#include "victoriascreen.h" // Incluido para la lógica de victoria/fin del juego

// Declaraciones adelantadas
class Juego;
class Enemigo;
class Obstaculo;

class Nivel2 : public NivelBase
{
    Q_OBJECT

public:
    Nivel2(Juego* juego, QObject* parent = nullptr);
    ~Nivel2() override;

    // Métodos públicos para la comunicación externa
    void manejarTecla(Qt::Key key);
    bool estaEnGameOver() const;
    bool estaEnVictoria() const { return nivelGanado; } // Propiedad de HEAD

protected:
    void configurarNivel() override;
    void crearEnemigos() override;
    void crearObstaculos() override;
    void actualizar() override;
    void reiniciarNivel(); // Método presente en HEAD

signals:
    void juegoTerminado();
    void volverAMenuPrincipal();

private slots:
    void onJuegoTerminado();
    void verificarVictoriaPorTiempo();
    void onEnemyDied(Persona* p); // Declaración del slot que maneja la muerte de cualquier Persona (como enemigo)

private:
    // --- Gestión de Corredor Infinito Triple (Métodos auxiliares) ---
    void crearFondoDinamico();
    void actualizarFondo(qreal posicionJugador);
    void crearSueloTriple();
    void actualizarSuelo(qreal posicionJugador);
    void spawnearObstaculoAleatorio();
    void limpiarObstaculosLejanos();
    void reposicionarEscena();

    // ================================================================
    // ** MIEMBROS DE ESTADO Y UI (Reordenados para evitar -Wreorder) **
    // ================================================================
    GameOverScreen* pantallaGameOver;
    VictoriaScreen* pantallaVictoria;
    QTimer* timerVictoria; // Timer para la lógica de victoria por tiempo

    bool juegoEnPausa;
    bool nivelGanado;

    // Timers de juego y cooldown
    QElapsedTimer danoObstaculoTimer;
    QElapsedTimer tiempoJugado;

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
    QList<Obstaculo*> obstaculos;

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
};

#endif // NIVEL2_H
