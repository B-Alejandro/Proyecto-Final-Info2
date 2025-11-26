#ifndef NIVEL2_H
#define NIVEL2_H

#include "nivelbase.h"
#include "enemigo.h"
#include "obstaculo.h"
#include "GameOverScreen.h"
#include <QGraphicsPixmapItem>

/*
  Clase Nivel2 - CORREDOR INFINITO MEJORADO
  - Sistema dual de fondos con parallax
  - Sistema dual de suelos con reposicionamiento
  - Generación procedural de obstáculos
  - Limpieza automática de elementos fuera de vista
*/

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
    // *** SISTEMA DE CORREDOR INFINITO ***

    // Generación y limpieza de obstáculos
    void spawnearObstaculoAleatorio();
    void limpiarObstaculosLejanos();
    void reposicionarEscena();

    // Sistema de fondo dual con parallax
    void crearFondoDinamico();
    void actualizarFondo(qreal posicionJugador);

    // Sistema de suelo dual con reposicionamiento
    void crearSueloDual();
    void actualizarSuelo(qreal posicionJugador);

    // *** VARIABLES DE CONTROL ***
    qreal distanciaEntreObstaculos;
    qreal ultimaPosicionSpawn;
    int contadorObstaculos;

    // *** ENTIDADES DEL NIVEL ***
    Enemigo* enemigoAtras;
    Obstaculo* suelo;        // Suelo 1 (parte del sistema dual)
    Obstaculo* suelo2;       // Suelo 2 (parte del sistema dual)
    GameOverScreen* pantallaGameOver;
    bool juegoEnPausa;

    // *** DIMENSIONES ***
    int vistaAncho;
    int vistaAlto;

    // *** SISTEMA DE FONDO PARALLAX DUAL ***
    QGraphicsPixmapItem* fondo1;
    QGraphicsPixmapItem* fondo2;
    qreal anchoFondo;

    // *** SISTEMA DE SUELO DUAL ***
    qreal anchoSuelo;
};

#endif // NIVEL2_H
