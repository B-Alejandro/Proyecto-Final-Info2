#ifndef TANQUE_H
#define TANQUE_H

#include "persona.h"
#include <QTimer>

/**
 * Clase Tanque
 *
 * Enemigo especial que:
 * - Se mueve verticalmente hacia abajo (como enemigos nivel 1)
 * - Tiene más vida (5 puntos)
 * - Dispara proyectiles parabólicos periódicamente
 * - Solo aparece en los extremos de la pantalla
 */
class Tanque : public Persona
{
    Q_OBJECT

public:
    /**
     * Constructor del tanque
     * @param w Ancho del tanque
     * @param h Alto del tanque
     * @param sceneWidth Ancho de la escena
     * @param sceneHeight Alto de la escena
     * @param posicionX Posición X inicial (debe ser extremo izquierdo o derecho)
     */
    Tanque(qreal w,
           qreal h,
           qreal sceneWidth,
           qreal sceneHeight,
           qreal posicionX);

    ~Tanque();

    /**
     * Obtiene el tiempo entre disparos
     */
    int getTiempoDisparo() const { return tiempoEntreDisparos; }

    /**
     * Establece el tiempo entre disparos
     */
    void setTiempoDisparo(int ms) { tiempoEntreDisparos = ms; }

    /**
     * Verifica si el tanque está en el lado izquierdo
     */
    bool estaEnLadoIzquierdo() const { return ladoIzquierdo; }

protected:
    /**
     * Maneja el input del tanque (movimiento automático hacia abajo)
     */
    void handleInput() override;

private slots:
    /**
     * Ejecuta el disparo del proyectil parabólico
     */
    void disparar();

private:
    QTimer* timerDisparo;           // Timer para disparos periódicos
    int tiempoEntreDisparos;        // Tiempo en ms entre disparos
    bool ladoIzquierdo;             // true si está en el lado izquierdo
    qreal velocidadDescenso;        // Velocidad de caída vertical
};

#endif // TANQUE_H
