// ============ nivel2.h ============
#ifndef NIVEL2_H
#define NIVEL2_H

#include "nivelbase.h"
#include "enemigo.h"
#include "obstaculo.h"
#include "GameOverScreen.h"

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

    // Manejo de teclas para Game Over
    void manejarTecla(Qt::Key key);

    bool estaEnGameOver() const { return juegoEnPausa; }

signals:
    void juegoTerminado();

private slots:
    void onJuegoTerminado();

private:
    Enemigo* enemigoAtras;
    Obstaculo* suelo;
    GameOverScreen* pantallaGameOver;
    bool juegoEnPausa;

    // Dimensiones de la vista
    int vistaAncho;
    int vistaAlto;
};

#endif // NIVEL2_H
