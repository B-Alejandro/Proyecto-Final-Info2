#include "proyectiltanque.h"
#include "explosion.h"
#include "persona.h"
#include <QPainter>
#include <QGraphicsScene>
#include <QDebug>
#include <cmath>

ProyectilTanque::ProyectilTanque(qreal width,
                                 qreal height,
                                 qreal sceneWidth,
                                 qreal sceneHeight,
                                 bool desdeIzquierda,
                                 QGraphicsItem* parent)
    : QGraphicsObject(parent)
    , m_w(width)
    , m_h(height)
    , sceneW(sceneWidth)
    , sceneH(sceneHeight)
    , desdeIzquierda(desdeIzquierda)
    , m_owner(nullptr)
    , explotado(false)
    , tiempoVuelo(0)
{
    // Configurar movimiento parabólico
    // *** AUMENTADA: Velocidad horizontal para mayor alcance ***
    velocidadX = desdeIzquierda ? 12.0 : -12.0;

    // *** AUMENTADA: Velocidad vertical inicial para arco más alto ***
    velocidadY = -8.0;

    // *** REDUCIDA: Gravedad más suave para trayectoria más larga ***
    gravedad = 0.25;

    setAcceptedMouseButtons(Qt::NoButton);
    setFlag(ItemIsFocusable, false);

    qDebug() << "ProyectilTanque creado. Desde izquierda:" << desdeIzquierda
             << "VelX:" << velocidadX << "VelY:" << velocidadY;
}

QRectF ProyectilTanque::boundingRect() const
{
    return QRectF(-m_w/2.0, -m_h/2.0, m_w, m_h);
}

void ProyectilTanque::paint(QPainter* painter,
                            const QStyleOptionGraphicsItem*,
                            QWidget*)
{
    // Dibujar proyectil como esfera naranja/roja
    painter->setBrush(QColor(255, 100, 0));
    painter->setPen(QPen(QColor(255, 50, 0), 2));
    painter->drawEllipse(boundingRect());

    // Efecto de "fuego"
    painter->setBrush(QColor(255, 200, 0, 150));
    QRectF innerRect = boundingRect().adjusted(3, 3, -3, -3);
    painter->drawEllipse(innerRect);
}

void ProyectilTanque::setOwner(Persona* owner)
{
    m_owner = owner;
}

void ProyectilTanque::advance(int phase)
{
    if (!phase || explotado) return;

    // Actualizar física parabólica
    tiempoVuelo += 0.016; // Aproximadamente 16ms por frame

    // Aplicar gravedad
    velocidadY += gravedad;

    // Mover el proyectil
    moveBy(velocidadX, velocidadY);

    // Verificar límites de la escena
    if (scene()) {
        QRectF sr = scene()->sceneRect();
        QPointF pos = scenePos();

        // Si sale de los límites horizontales o toca el suelo, explotar
        bool fueraLimites = (pos.x() < sr.left() - 50) ||
                            (pos.x() > sr.right() + 50) ||
                            (pos.y() > sr.bottom() - 100); // Cerca del suelo

        if (fueraLimites) {
            qDebug() << "Proyectil fuera de límites en:" << pos;
            explotar();
            return;
        }
    }

    // Verificar colisiones con jugador
    QList<QGraphicsItem*> coll = collidingItems(Qt::IntersectsItemShape);
    for (QGraphicsItem* it : coll) {
        Persona* p = dynamic_cast<Persona*>(it);
        if (!p) continue;
        if (p == m_owner) continue; // No dañar al tanque que disparó

        qDebug() << "Proyectil colisionó con jugador/enemigo";
        explotar();
        return;
    }
}

void ProyectilTanque::explotar()
{
    if (explotado) return;
    explotado = true;

    if (!scene()) {
        deleteLater();
        return;
    }

    qDebug() << "🔥 Proyectil explotando en:" << scenePos();

    // *** AUMENTADO: Explosión más grande ***
    Explosion* explosion = new Explosion(
        scenePos().x(),
        scenePos().y(),
        150,  // Era 80, ahora 150 (radio casi el doble!)
        3     // 3 vidas de daño
        );

    explosion->setOwner(m_owner);
    scene()->addItem(explosion);

    qDebug() << "💥 Explosión creada con radio 150";

    // Eliminar el proyectil
    scene()->removeItem(this);
    deleteLater();
}
