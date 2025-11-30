#ifndef NIVEL2_H
#define NIVEL2_H

#include "nivelbase.h"
#include "enemigo.h"
#include "obstaculo.h"
#include "GameOverScreen.h"
#include <QGraphicsPixmapItem>
#include <QElapsedTimer>
#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>

class Nivel2 : public NivelBase
{
    Q_OBJECT
public:
    explicit Nivel2(Juego* juego, QObject* parent = nullptr);
    ~Nivel2();

    void configurarNivel() override;
    void crearEnemigos() override;
    void crearObstaculos() override;
    void actualizar() override;

    void manejarTecla(Qt::Key key);

    bool estaEnGameOver() const { return juegoEnPausa; }

signals:
    void juegoTerminado();

private slots:
    void onJuegoTerminado();

private:
    // HUD
    QWidget* hudWidget;
    QLabel* vidaLabel;
    QLabel* tiempoLabel;
    QLabel* scoreLabel;

    // Métodos privados
    void spawnearObstaculoAleatorio();
    void limpiarObstaculosLejanos();
    void reposicionarEscena();
    void crearFondoDinamico();
    void actualizarFondo(qreal posicionJugador);
    void crearSueloTriple();
    void actualizarSuelo(qreal posicionJugador);

    // *** VARIABLES EN ORDEN DE INICIALIZACIÓN ***

    // Entidades del nivel
    Enemigo* enemigoAtras;

    // Suelos (sistema triple)
    Obstaculo* suelo1;
    Obstaculo* suelo2;
    Obstaculo* suelo3;

    GameOverScreen* pantallaGameOver;
    bool juegoEnPausa;

    // Sistema de fondo parallax (sistema triple)
    QGraphicsPixmapItem* fondo1;
    QGraphicsPixmapItem* fondo2;
    QGraphicsPixmapItem* fondo3;
    qreal anchoFondo;

    // Sistema de suelo
    qreal anchoSuelo;

    // Dimensiones de vista
    int vistaAncho;
    int vistaAlto;

    // Control de generación de obstáculos
    qreal distanciaEntreObstaculos;
    qreal ultimaPosicionSpawn;
    int contadorObstaculos;

    // Timer para cooldown de daño por obstáculos
    QElapsedTimer danoObstaculoTimer;
    QGraphicsTextItem* textoVida;
    QGraphicsTextItem* textoTiempo;
    QElapsedTimer tiempoJugado;
    QTimer* timerVictoria; // Temporizador de 20 segundos
    const int DURACION_NIVEL_MS = 20000; // 20 segundos
};

#endif // NIVEL2_H
