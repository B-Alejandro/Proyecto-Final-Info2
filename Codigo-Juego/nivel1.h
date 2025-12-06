#ifndef NIVEL1_H
#define NIVEL1_H

#include "nivelbase.h"
#include "persona.h"
#include "GameOverScreen.h"
#include "victoriascreen.h"
#include "panelinfo.h"

// Declaraciones adelantadas
class PanelInfo;
class Tanque;
class Enemigo;
class Obstaculo;
class Proyectil; // 🔥 AÑADIDO

class Nivel1 : public NivelBase
{
    Q_OBJECT

public:
    Nivel1(Juego* juego, QObject* parent = 0);

    void manejarTecla(Qt::Key key);
    bool estaEnGameOver() const { return juegoEnPausa; }
    bool estaEnVictoria() const { return nivelGanado; }

signals:
    void volverAMenuPrincipal();

protected:
    void configurarNivel() override;
    void crearEnemigos() override;
    void crearObstaculos() override;
    void actualizar() override;

private:
    void configurarEscena();
    int vistaAncho;
    int vistaAlto;

    QList<Enemigo*> listaEnemigos;
    QList<Tanque*> listaTanques;
    QList<Obstaculo*> listaObstaculosMoviles;
    QList<Proyectil*> listaProyectiles; // 🔥 AÑADIDO: Lista para seguir los proyectiles

    int spawnDelayMs = 2000;
    int spawnMargin = 120;

    // Sistema de Game Over y Victoria
    GameOverScreen* pantallaGameOver;
    VictoriaScreen* pantallaVictoria;
    PanelInfo* infoPanel;
    bool juegoEnPausa;
    bool nivelGanado;

    // Sistema de puntuación y vida del jugador
    int puntosActuales;
    int puntosObjetivo;
    int vidaJugadorActual;

    // Funciones de oleadas
    void spawnearOleada();
    void gameTick();
    void cleanupOffscreen();
    void dispararProyectil(); // 🔥 AÑADIDO: Declaración de la función de ataque

    // Colisiones
    void revisarColision();
    void colisionDetectada(Enemigo* e);
    void colisionTanqueDetectada(Tanque* t);
    void colisionObstaculoDetectada(Obstaculo* o);
    void reiniciarNivel();

    // Estado del jugador
    void verificarEstadoJugador();

    // Sistema de puntos
    void agregarPuntos(int cantidad);
    void verificarVictoria();
    void actualizarInfoPanel();

    qreal scrollOffset;

private slots:
    void onEnemyDied(Persona* p);
    void onTankDied(Persona* p);
    void onJugadorMurio();
    void onObstaculoDestruido(Obstaculo* obs);
    void onJugadorDaniado(int vidaActual, int vidaMax);

    // Slots para manejar la pausa
    void manejarPausa();
    void manejarAccionPausa(PanelInfo::AccionPausa accion);
};

#endif // NIVEL1_H
