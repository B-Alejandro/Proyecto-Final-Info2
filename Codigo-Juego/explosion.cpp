#include "explosion.h"
#include "persona.h"
#include <QPainter>
#include <QGraphicsScene>
#include <QDebug>
#include <cmath>

Explosion::Explosion(qreal x,
                     qreal y,
                     qreal radio,
                     int danio,
                     QGraphicsItem* parent)
    : QGraphicsObject(parent)
    , m_radio(radio)
    , m_radioActual(radio * 0.3)  // Comienza pequeña
    , m_danio(danio)
    , m_owner(nullptr)
    , framesVividos(0)
    , framesMaximos(20)  // Dura aproximadamente 0.33 segundos
    , danioAplicado(false)
{
    setPos(x, y);
    setZValue(1000);  // Por encima de todo

    setAcceptedMouseButtons(Qt::NoButton);
    setFlag(ItemIsFocusable, false);

    // Timer para autodestrucción
    timerVida = new QTimer(this);
    connect(timerVida, &QTimer::timeout, this, &Explosion::autodestruir);
    timerVida->start(400);  // Desaparece después de 400ms

    qDebug() << "Explosión creada en:" << x << y << "Radio:" << radio;
}

QRectF Explosion::boundingRect() const
{
    qreal r = m_radio * 1.2;  // Un poco más grande para el efecto visual
    return QRectF(-r, -r, r * 2, r * 2);
}

void Explosion::paint(QPainter* painter,
                      const QStyleOptionGraphicsItem*,
                      QWidget*)
{
    painter->setRenderHint(QPainter::Antialiasing);

    // Calcular transparencia según edad
    int alpha = 255 - (framesVividos * 255 / framesMaximos);
    if (alpha < 0) alpha = 0;

    // Círculo exterior (amarillo/naranja)
    QRadialGradient gradientExterior(0, 0, m_radioActual);
    gradientExterior.setColorAt(0, QColor(255, 200, 0, alpha));
    gradientExterior.setColorAt(0.5, QColor(255, 100, 0, alpha));
    gradientExterior.setColorAt(1, QColor(255, 0, 0, alpha / 2));

    painter->setBrush(gradientExterior);
    painter->setPen(Qt::NoPen);
    painter->drawEllipse(QPointF(0, 0), m_radioActual, m_radioActual);

    // Círculo interior (centro brillante)
    qreal radioInterior = m_radioActual * 0.4;
    QRadialGradient gradientInterior(0, 0, radioInterior);
    gradientInterior.setColorAt(0, QColor(255, 255, 200, alpha));
    gradientInterior.setColorAt(1, QColor(255, 150, 0, alpha / 2));

    painter->setBrush(gradientInterior);
    painter->drawEllipse(QPointF(0, 0), radioInterior, radioInterior);
}

void Explosion::setOwner(Persona* owner)
{
    m_owner = owner;
}

void Explosion::advance(int phase)
{
    if (!phase) return;

    framesVividos++;

    // Animación de expansión
    qreal progreso = static_cast<qreal>(framesVividos) / framesMaximos;
    m_radioActual = m_radio * (0.3 + 0.7 * progreso);

    // Aplicar daño una vez cuando alcanza tamaño completo
    if (!danioAplicado && progreso >= 0.3) {
        aplicarDanio();
        danioAplicado = true;
    }

    update();
}

void Explosion::aplicarDanio()
{
    if (!scene()) return;

    qDebug() << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━";
    qDebug() << "💥 EXPLOSIÓN aplicando daño";
    qDebug() << "   Centro:" << scenePos();
    qDebug() << "   Radio:" << m_radioActual;

    // Obtener todos los items en el área
    QList<QGraphicsItem*> itemsEnArea = scene()->items(
        QRectF(scenePos().x() - m_radioActual,
               scenePos().y() - m_radioActual,
               m_radioActual * 2,
               m_radioActual * 2),
        Qt::IntersectsItemShape
        );

    qDebug() << "   Items en área:" << itemsEnArea.size();

    int daniosAplicados = 0;

    for (QGraphicsItem* item : itemsEnArea) {
        Persona* p = dynamic_cast<Persona*>(item);
        if (!p) continue;
        if (p == m_owner) {
            qDebug() << "   ⚠️ Ignorando owner (tanque)";
            continue;
        }

        // Calcular distancia al centro de la explosión
        QPointF centroPersona = p->scenePos() +
                                QPointF(p->boundingRect().width() / 2,
                                        p->boundingRect().height() / 2);
        QPointF centroExplosion = scenePos();

        qreal dx = centroPersona.x() - centroExplosion.x();
        qreal dy = centroPersona.y() - centroExplosion.y();
        qreal distancia = std::sqrt(dx * dx + dy * dy);

        qDebug() << "   🎯 Persona encontrada:";
        qDebug() << "      Posición:" << p->scenePos();
        qDebug() << "      Centro:" << centroPersona;
        qDebug() << "      Distancia:" << distancia;
        qDebug() << "      Radio límite:" << m_radioActual;

        // Solo dañar si está dentro del radio
        if (distancia <= m_radioActual) {
            int danioEnVidas = 3;
            qDebug() << "      ✅ DENTRO DEL RADIO - Aplicando" << danioEnVidas << "vidas de daño";
            p->recibirDanio(danioEnVidas);
            daniosAplicados++;
        } else {
            qDebug() << "      ❌ FUERA DEL RADIO - No daña";
        }
    }

    qDebug() << "💥 Total dañados:" << daniosAplicados;
    qDebug() << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
}

void Explosion::autodestruir()
{
    if (scene()) {
        scene()->removeItem(this);
    }
    deleteLater();
}
