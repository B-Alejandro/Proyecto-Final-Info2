#ifndef NIVEL2_H
#define NIVEL2_H

#include "nivelbase.h"
#include "enemigo.h"
#include "obstaculo.h"
#include "GameOverScreen.h"
#include <QGraphicsPixmapItem>

/*
  Clase Nivel2
  - Implementa la logica del nivel 2
  - Maneja fondo dinamico, enemigos, obstaculos y la pantalla de Game Over
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

    // Manejo de teclas cuando se esta en estado Game Over (juego en pausa)
    void manejarTecla(Qt::Key key);

    bool estaEnGameOver() const { return juegoEnPausa; }

signals:
    void juegoTerminado();

private slots:
    void onJuegoTerminado();

private:
    // Metodo para crear el fondo dinamico y actualizarlo
    void crearFondoDinamico();
    void actualizarFondo(qreal posicionCamara);

    Enemigo* enemigoAtras;
    Obstaculo* suelo;
    GameOverScreen* pantallaGameOver;
    bool juegoEnPausa;

    // Dimensiones de la vista
    int vistaAncho;
    int vistaAlto;

    // Fondo dinamico
    QGraphicsPixmapItem* fondo1;
    QGraphicsPixmapItem* fondo2;
    qreal anchoFondo;
};

#endif // NIVEL2_H
