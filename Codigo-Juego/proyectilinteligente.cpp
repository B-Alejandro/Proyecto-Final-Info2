#include "proyectilInteligente.h"
#include "persona.h"
#include <QPainter>
#include <QGraphicsScene>
#include <QDebug>
#include <cmath>

ProyectilInteligente::ProyectilInteligente(QPointF puntoInicio,
                                           QPointF puntoObjetivo,
                                           qreal velocidad,
                                           int danio,
                                           QGraphicsItem* parent)
    : QGraphicsObject(parent)
    , m_ancho(14)
    , m_alto(14)
    , m_velocidad(velocidad)
    , m_danio(danio)
    , m_owner(nullptr)
    , m_framesVividos(0)
    , m_framesMaximos(300)  // ~5 segundos a 60 FPS
{
    // Calcular vector de dirección
    qreal dx = puntoObjetivo.x() - puntoInicio.x();
    qreal dy = puntoObjetivo.y() - puntoInicio.y();

    // Calcular distancia
    qreal distancia = std::sqrt(dx * dx + dy * dy);

    // Evitar división por cero
    if (distancia < 1.0) {
        dx = 1.0;
        dy = 0.0;
        distancia = 1.0;
    }

    // Normalizar dirección
    m_direccion = QPointF(dx / distancia, dy / distancia);

    // Calcular componentes de velocidad
    m_velocidadX = m_direccion.x() * m_velocidad;
    m_velocidadY = m_direccion.y() * m_velocidad;

    // Posicionar en el punto de inicio
    setPos(puntoInicio);

    // Configuración de item
    setAcceptedMouseButtons(Qt::NoButton);
    setFlag(ItemIsFocusable, false);
    setZValue(100);  // Por encima de otros elementos

    qDebug() << "   💥 ProyectilInteligente creado";
    qDebug() << "      Inicio:" << puntoInicio;
    qDebug() << "      Objetivo:" << puntoObjetivo;
    qDebug() << "      Dirección:" << m_direccion;
    qDebug() << "      Velocidad:" << m_velocidad;
}

QRectF ProyectilInteligente::boundingRect() const
{
    // Centrado en el origen
    return QRectF(-m_ancho / 2, -m_alto / 2, m_ancho, m_alto);
}

void ProyectilInteligente::paint(QPainter* painter,
                                 const QStyleOptionGraphicsItem*,
                                 QWidget*)
{
    // Guardar estado del painter
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);

    // Dibujar proyectil como esfera roja brillante
    QRadialGradient gradient(0, 0, m_ancho / 2);
    gradient.setColorAt(0, QColor(255, 255, 100));     // Centro amarillo
    gradient.setColorAt(0.5, QColor(255, 100, 0));     // Medio naranja
    gradient.setColorAt(1, QColor(200, 0, 0));         // Borde rojo oscuro

    painter->setBrush(gradient);
    painter->setPen(QPen(QColor(150, 0, 0), 2));
    painter->drawEllipse(boundingRect());

    // Efecto de "estela" en la dirección opuesta
    QPointF estela = -m_direccion * (m_ancho / 2);
    painter->setPen(QPen(QColor(255, 100, 0, 150), 3));
    painter->drawLine(QPointF(0, 0), estela);

    painter->restore();
}

void ProyectilInteligente::setOwner(Persona* owner)
{
    m_owner = owner;
}

void ProyectilInteligente::advance(int phase)
{
    // Fase 0: preparación
    if (!phase) return;

    // Incrementar contador de frames
    m_framesVividos++;

    // Auto-destrucción por tiempo
    if (m_framesVividos >= m_framesMaximos) {
        if (scene()) {
            scene()->removeItem(this);
        }
        deleteLater();
        return;
    }

    // Mover el proyectil en la dirección calculada
    moveBy(m_velocidadX, m_velocidadY);

    // Verificar límites de la escena
    if (scene()) {
        QRectF sceneRect = scene()->sceneRect();
        QPointF pos = scenePos();

        // Si sale de la escena, destruir
        if (pos.x() < sceneRect.left() - 50 ||
            pos.x() > sceneRect.right() + 50 ||
            pos.y() < sceneRect.top() - 50 ||
            pos.y() > sceneRect.bottom() + 50) {

            scene()->removeItem(this);
            deleteLater();
            return;
        }
    }

    // Detectar colisiones
    QList<QGraphicsItem*> colisiones = collidingItems(Qt::IntersectsItemShape);

    for (QGraphicsItem* item : colisiones) {
        Persona* p = dynamic_cast<Persona*>(item);

        if (!p) continue;           // No es una persona
        if (p == m_owner) continue; // Es el dueño (no auto-dañarse)
        if (!p->estaVivo()) continue; // Ya está muerto

        // ¡IMPACTO!
        qDebug() << "   🎯 IMPACTO del proyectil! Daño:" << m_danio;

        p->recibirDanio(m_danio);

        // Destruir proyectil después del impacto
        if (scene()) {
            scene()->removeItem(this);
        }
        deleteLater();
        return;
    }
}
