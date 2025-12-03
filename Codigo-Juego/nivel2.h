#ifndef NIVEL2_H
#define NIVEL2_H

#include "nivelbase.h"
#include <QList>
#include <QTimer>
#include <QElapsedTimer>
#include <QGraphicsPixmapItem>

// Declaraciones adelantadas
class Juego;
class Enemigo;
class Obstaculo;
class GameOverScreen;
class QWidget;
class QLabel;

class Nivel2 : public NivelBase
{
    Q_OBJECT

public:
    Nivel2(Juego* juego, QObject* parent = nullptr);
    ~Nivel2() override;

    // --- CORRECCIÓN 1: Acceso público desde juego.cpp ---
    void manejarTecla(Qt::Key key) ;
    bool estaEnGameOver() const; // <-- CORRECCIÓN 2: Nuevo método de estado

protected:
    void configurarNivel() override;
    void crearEnemigos() override;
    void crearObstaculos() override;
    void actualizar() override;
    // (manejarTecla se movió)

signals:
    // --- CORRECCIÓN 3: Declarar la señal faltante ---
    void juegoTerminado();

private slots:
    void onJuegoTerminado();

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
    // ** MIEMBROS REORDENADOS para evitar -Wreorder **
    // ================================================================

    Enemigo* enemigoAtras;
    Obstaculo* suelo1;
    Obstaculo* suelo2;
    Obstaculo* suelo3;
    GameOverScreen* pantallaGameOver;
    bool juegoEnPausa;

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

    QElapsedTimer tiempoJugado;
    QElapsedTimer danoObstaculoTimer;

    QList<Obstaculo*> obstaculos;

    QWidget* hudWidget;
    QLabel* vidaLabel;
    QLabel* tiempoLabel;
    QLabel* scoreLabel;
};

#endif // NIVEL2_H
