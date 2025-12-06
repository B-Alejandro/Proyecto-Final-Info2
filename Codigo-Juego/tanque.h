#ifndef TANQUE_H
#define TANQUE_H

#include "persona.h"
#include <QTimer>
#include <QGraphicsPixmapItem>
#include <QPixmap>

class Tanque : public Persona
{
    Q_OBJECT

public:
    Tanque(qreal w, qreal h, qreal sceneWidth, qreal sceneHeight, qreal posicionX);
    ~Tanque();

    void setTiempoDisparo(int ms) { tiempoEntreDisparos = ms; }

    // ========================================================================
    // 🎨 TRES MÉTODOS PARA AGREGAR SPRITES (Elige el que prefieras)
    // ========================================================================

    // MÉTODO 1: Simple - Sprite como textura del brush (más fácil)
    void cargarSpriteTanque();

    // MÉTODO 2: Avanzado - Sprite como QGraphicsPixmapItem hijo (mejor calidad)
    void cargarSpriteComoPixmap();

    // MÉTODO 3: Custom - Sobrescribir paint() para control total
    void cargarSpriteCustomPaint(const QString& rutaSprite);

protected:
    // ✅ CRÍTICO: Sobrescribir handleInput() para mantener movimiento constante
    void handleInput() override;

    // Para MÉTODO 3: Sobrescribir paint
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

private:
    QTimer* timerDisparo;
    int tiempoEntreDisparos;
    qreal velocidadDescenso;
    bool ladoIzquierdo;

    // Para MÉTODO 2: QGraphicsPixmapItem hijo
    QGraphicsPixmapItem* spriteItem;

    // Para MÉTODO 3: Paint personalizado
    QPixmap spriteEstaticoTanque;
    bool spriteCargado = false;

private slots:
    void disparar();
};

#endif // TANQUE_H
