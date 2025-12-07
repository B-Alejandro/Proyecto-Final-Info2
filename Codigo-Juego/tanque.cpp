#include "tanque.h"
#include "proyectiltanque.h"
#include <QBrush>
#include <QGraphicsScene>
#include <QDebug>
#include <QPainter>

Tanque::Tanque(qreal w,
               qreal h,
               qreal sceneWidth,
               qreal sceneHeight,
               qreal posicionX)
    : Persona(w, h, sceneWidth, sceneHeight, TipoMovimiento::RECTILINEO)
    , tiempoEntreDisparos(500)
    , velocidadDescenso(3.0)
    , spriteItem(nullptr)
    , spriteCargado(false)
{
    setVida(5);

    // Color de fallback (se oculta si se carga sprite)
    setBrush(QBrush(QColor(139, 69, 19)));

    ladoIzquierdo = (posicionX < 0);

    //Configurar movimiento constante hacia abajo
    downPressed = true;
    upPressed = false;
    leftPressed = false;
    rightPressed = false;
    speed = velocidadDescenso;

    // Configurar timer de disparo
    timerDisparo = new QTimer(this);
    connect(timerDisparo, &QTimer::timeout, this, &Tanque::disparar);

    QTimer::singleShot(200, this, [this]() {
        if (estaVivo()) {
            disparar(); // ¡Disparo inmediato!
            timerDisparo->start(tiempoEntreDisparos);
        }
    });

    qDebug() << "Tanque creado en X:" << posicionX
             << "Lado izquierdo:" << ladoIzquierdo
             << "Vida:" << getVida()
             << "Speed:" << speed;
}

Tanque::~Tanque()
{
    if (timerDisparo) {
        timerDisparo->stop();
    }
}

// ✅ SOLUCIÓN: Sobrescribir handleInput() para mantener movimiento constante
void Tanque::handleInput()
{
    // CRÍTICO: Mantener siempre el movimiento hacia abajo activo
    downPressed = true;
    upPressed = false;
    leftPressed = false;
    rightPressed = false;

    // Debug periódico para verificar movimiento
    static int debugCounter = 0;
    if (debugCounter++ % 120 == 0) {
        qDebug() << "🎮 Tanque - Y:" << y()
                 << "Vida:" << getVida()
                 << "downPressed:" << downPressed
                 << "speed:" << speed;
    }
}

void Tanque::disparar()
{
    if (!scene() || !estaVivo()) {
        if (timerDisparo) {
            timerDisparo->stop();
        }
        return;
    }

    qDebug() << "💥 Tanque disparando desde posición:" << x() << y();

    qreal anchoProyectil = 15;
    qreal altoProyectil = 15;

    QRectF rectTanque = boundingRect();
    QPointF posicionInicial = scenePos() + QPointF(
                                  rectTanque.width() / 2 - anchoProyectil / 2,
                                  rectTanque.height() / 2
                                  );

    ProyectilTanque* proyectil = new ProyectilTanque(
        anchoProyectil,
        altoProyectil,
        sceneW,
        sceneH,
        ladoIzquierdo
        );

    proyectil->setOwner(this);
    proyectil->setPos(posicionInicial);

    scene()->addItem(proyectil);

    qDebug() << "🚀 Proyectil de tanque creado en:" << posicionInicial;
}

// ========================================================================
// 🎨 MÉTODO 1: Sprite como textura (Simple, compatible con código actual)
// ========================================================================
void Tanque::cargarSpriteTanque()
{
    QPixmap spriteTanque(":/Recursos/Tanque/Tanque.png");

    if (spriteTanque.isNull()) {
        qDebug() << "⚠️ No se pudo cargar sprite del tanque, usando color sólido";
        setBrush(QBrush(QColor(139, 69, 19))); // Marrón oscuro como fallback
        return;
    }

    // Escalar el sprite al tamaño del tanque
    QPixmap spriteEscalado = spriteTanque.scaled(
        boundingRect().width(),
        boundingRect().height(),
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation
        );

    // Aplicar el sprite como textura
    QBrush texturaBrush(spriteEscalado);
    setBrush(texturaBrush);

    qDebug() << "✅ Sprite del tanque cargado como textura";
}

// ========================================================================
// 🎨 MÉTODO 2: Sprite como QGraphicsPixmapItem hijo (Mejor calidad visual)
// ========================================================================
void Tanque::cargarSpriteComoPixmap()
{
    QPixmap spriteTanque(":/Recursos/Tanque/Tanque.png");

    if (spriteTanque.isNull()) {
        qDebug() << "⚠️ No se pudo cargar sprite del tanque";
        return;
    }

    // Escalar el sprite
    QPixmap spriteEscalado = spriteTanque.scaled(
        boundingRect().width(),
        boundingRect().height(),
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation
        );

    // Si ya existe un sprite, eliminarlo primero
    if (spriteItem) {
        delete spriteItem;
    }

    // Crear un item de pixmap como hijo del tanque
    spriteItem = new QGraphicsPixmapItem(spriteEscalado, this);
    spriteItem->setPos(0, 0); // Posición relativa al tanque

    // Hacer el rectángulo invisible para que solo se vea el sprite
    setPen(Qt::NoPen);
    setBrush(Qt::NoBrush);

    qDebug() << "✅ Sprite del tanque cargado como QGraphicsPixmapItem";
}

// ========================================================================
// 🎨 MÉTODO 3: Sobrescribir paint() para dibujar sprite directamente
// ========================================================================
void Tanque::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    // Si no hay sprite cargado, usar el método padre
    if (!spriteCargado) {
        Persona::paint(painter, option, widget);
        return;
    }

    // Dibujar el sprite estático del tanque
    painter->drawPixmap(0, 0, spriteEstaticoTanque);

    // Opcional: Dibujar borde de debug
    // painter->setPen(QPen(Qt::red, 2));
    // painter->drawRect(boundingRect());
}

// Método auxiliar para cargar sprite con paint personalizado
void Tanque::cargarSpriteCustomPaint(const QString& rutaSprite)
{
    spriteEstaticoTanque = QPixmap(rutaSprite);

    if (spriteEstaticoTanque.isNull()) {
        qDebug() << "⚠️ No se pudo cargar sprite:" << rutaSprite;
        spriteCargado = false;
        return;
    }

    // Escalar al tamaño del tanque
    spriteEstaticoTanque = spriteEstaticoTanque.scaled(
        boundingRect().width(),
        boundingRect().height(),
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation
        );

    spriteCargado = true;
    update(); // Forzar redibujado

    qDebug() << " Sprite del tanque cargado con paint() personalizado";
}
