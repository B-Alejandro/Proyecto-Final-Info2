#include "hudnivel1.h"
#include <QFont>
#include <QBrush>
#include <QPen>
#include <QDebug>

HUDNivel1::HUDNivel1(QGraphicsScene* scene, QObject* parent)
    : QObject(parent)
    , escena(scene)
    , textoVidas(nullptr)
    , textoPuntos(nullptr)
    , fondoHUD(nullptr)
    , vidasActuales(5)
    , puntosActuales(0)
    , puntosObjetivo(100)
    , offsetX(20)
    , offsetY(20)
{
    crearElementos();
}

HUDNivel1::~HUDNivel1()
{
    // Los items se eliminan automáticamente con la escena
}

void HUDNivel1::crearElementos()
{
    if (!escena) return;

    // *** REDUCIDO: Fondo más pequeño ***
    fondoHUD = new QGraphicsRectItem(0, 0, 200, 50);  // Era 300x80, ahora 200x50
    fondoHUD->setBrush(QBrush(QColor(0, 0, 0, 150)));
    fondoHUD->setPen(QPen(QColor(255, 255, 255, 100), 2));
    fondoHUD->setZValue(9000);
    escena->addItem(fondoHUD);

    crearCorazones();
    crearTextoPuntuacion();
}

void HUDNivel1::crearCorazones()
{
    // *** REDUCIDO: Corazones más pequeños ***
    for (int i = 0; i < 5; ++i) {
        QGraphicsRectItem* corazon = new QGraphicsRectItem(0, 0, 12, 12);  // Era 20x20, ahora 12x12
        corazon->setBrush(QBrush(Qt::red));
        corazon->setPen(QPen(Qt::darkRed, 1));  // Borde más delgado
        corazon->setZValue(9001);

        escena->addItem(corazon);
        corazones.append(corazon);
    }

    // *** REDUCIDO: Texto más pequeño ***
    textoVidas = new QGraphicsTextItem("VIDAS:");
    QFont fontVidas("Arial", 10, QFont::Bold);  // Era 14, ahora 10
    textoVidas->setFont(fontVidas);
    textoVidas->setDefaultTextColor(Qt::white);
    textoVidas->setZValue(9001);
    escena->addItem(textoVidas);
}

void HUDNivel1::crearTextoPuntuacion()
{
    // *** REDUCIDO: Texto de puntuación más pequeño ***
    textoPuntos = new QGraphicsTextItem("PUNTOS: 0 / 100");
    QFont fontPuntos("Arial", 10, QFont::Bold);  // Era 14, ahora 10
    textoPuntos->setFont(fontPuntos);
    textoPuntos->setDefaultTextColor(QColor(255, 215, 0));
    textoPuntos->setZValue(9001);
    escena->addItem(textoPuntos);
}

void HUDNivel1::actualizarVidas(int vidas)
{
    if (vidas < 0) vidas = 0;
    if (vidas > 5) vidas = 5;

    vidasActuales = vidas;

    // Actualizar visibilidad de corazones
    for (int i = 0; i < corazones.size(); ++i) {
        if (i < vidas) {
            corazones[i]->setBrush(QBrush(Qt::red));
            corazones[i]->setOpacity(1.0);
        } else {
            corazones[i]->setBrush(QBrush(Qt::gray));
            corazones[i]->setOpacity(0.3);
        }
    }

    qDebug() << "HUD: Vidas actualizadas a" << vidas;
}

void HUDNivel1::actualizarPuntuacion(int puntos, int objetivo)
{
    puntosActuales = puntos;
    puntosObjetivo = objetivo;

    if (textoPuntos) {
        QString texto = QString("PUNTOS: %1 / %2").arg(puntos).arg(objetivo);
        textoPuntos->setPlainText(texto);

        // Cambiar color si está cerca del objetivo
        if (puntos >= objetivo * 0.8) {
            textoPuntos->setDefaultTextColor(QColor(0, 255, 0)); // Verde
        } else {
            textoPuntos->setDefaultTextColor(QColor(255, 215, 0)); // Dorado
        }
    }

    qDebug() << "HUD: Puntos actualizados a" << puntos << "/" << objetivo;
}

void HUDNivel1::actualizarPosicion(qreal camaraX, qreal camaraY)
{
    // *** NUEVO: Calcular posición debajo del jugador ***
    // El jugador está en camaraX (su posición real)
    // Queremos el HUD centrado debajo de él

    qreal hudCentroX = camaraX - 50;  // Centrar el HUD (ancho 300 / 2)
    qreal hudY = camaraY + 180;        // Debajo del jugador (ajusta si es necesario)

    // Posicionar fondo
    if (fondoHUD) {
        fondoHUD->setPos(hudCentroX - 10, hudY - 10);
    }

    // Posicionar texto de vidas
    if (textoVidas) {
        textoVidas->setPos(hudCentroX, hudY);
    }

    // Posicionar corazones
    qreal corazonX = hudCentroX + 70;
    qreal corazonY = hudY + 5;
    for (int i = 0; i < corazones.size(); ++i) {
        corazones[i]->setPos(corazonX + (i * 25), corazonY);
    }

    // Posicionar texto de puntuación
    if (textoPuntos) {
        textoPuntos->setPos(hudCentroX, hudY + 40);
    }
}
