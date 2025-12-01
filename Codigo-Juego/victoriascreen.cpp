#include "victoriascreen.h"
#include <QFont>
#include <QBrush>
#include <QPen>
#include <QGraphicsView>

VictoriaScreen::VictoriaScreen(QGraphicsScene* scene, QObject* parent)
    : QObject(parent)
    , escena(scene)
    , fondo(nullptr)
    , textoVictoria(nullptr)
    , textoPuntos(nullptr)
    , textoReiniciar(nullptr)
    , textoMenu(nullptr)
    , visible(false)
{
    crearElementos();
}

VictoriaScreen::~VictoriaScreen()
{
    // Los items se eliminan automáticamente con la escena
}

void VictoriaScreen::crearElementos()
{
    if (!escena) return;

    QRectF sceneRect = escena->sceneRect();

    // Crear fondo semi-transparente
    fondo = new QGraphicsRectItem(sceneRect);
    fondo->setBrush(QBrush(QColor(0, 100, 0, 200))); // Verde oscuro
    fondo->setPen(QPen(Qt::NoPen));
    fondo->setZValue(10000);
    fondo->setVisible(false);
    escena->addItem(fondo);

    // Texto "¡VICTORIA!"
    textoVictoria = new QGraphicsTextItem("¡VICTORIA!");
    QFont fontVictoria("Arial", 60, QFont::Bold);
    textoVictoria->setFont(fontVictoria);
    textoVictoria->setDefaultTextColor(QColor(255, 215, 0)); // Dorado
    textoVictoria->setZValue(10001);
    textoVictoria->setVisible(false);
    escena->addItem(textoVictoria);

    // Texto de puntos
    textoPuntos = new QGraphicsTextItem("Puntos: 0");
    QFont fontPuntos("Arial", 28, QFont::Bold);
    textoPuntos->setFont(fontPuntos);
    textoPuntos->setDefaultTextColor(Qt::white);
    textoPuntos->setZValue(10001);
    textoPuntos->setVisible(false);
    escena->addItem(textoPuntos);

    // Texto "Presiona R para Reiniciar"
    textoReiniciar = new QGraphicsTextItem("Presiona R para Reiniciar");
    QFont fontOpciones("Arial", 20);
    textoReiniciar->setFont(fontOpciones);
    textoReiniciar->setDefaultTextColor(Qt::white);
    textoReiniciar->setZValue(10001);
    textoReiniciar->setVisible(false);
    escena->addItem(textoReiniciar);

    // Texto "Presiona ESC para Menú"
    textoMenu = new QGraphicsTextItem("Presiona ESC para Menu");
    textoMenu->setFont(fontOpciones);
    textoMenu->setDefaultTextColor(Qt::white);
    textoMenu->setZValue(10001);
    textoMenu->setVisible(false);
    escena->addItem(textoMenu);
}

void VictoriaScreen::mostrar(int puntosFinales)
{
    if (visible) return;

    visible = true;

    // Actualizar texto de puntos
    if (textoPuntos) {
        QString texto = QString("¡Puntuación Final: %1!").arg(puntosFinales);
        textoPuntos->setPlainText(texto);
    }

    // Obtener la vista para centrar en la cámara
    if (!escena->views().isEmpty()) {
        QGraphicsView* vista = escena->views().first();
        QPointF centroVista = vista->mapToScene(vista->viewport()->rect().center());

        qreal centerX = centroVista.x();
        qreal centerY = centroVista.y();

        // Posicionar "¡VICTORIA!"
        if (textoVictoria) {
            QRectF rectVictoria = textoVictoria->boundingRect();
            textoVictoria->setPos(centerX - rectVictoria.width() / 2,
                                  centerY - rectVictoria.height() - 120);
            textoVictoria->setVisible(true);
        }

        // Posicionar puntos
        if (textoPuntos) {
            QRectF rectPuntos = textoPuntos->boundingRect();
            textoPuntos->setPos(centerX - rectPuntos.width() / 2,
                                centerY - rectPuntos.height() - 40);
            textoPuntos->setVisible(true);
        }

        // Posicionar "Presiona R"
        if (textoReiniciar) {
            QRectF rectReiniciar = textoReiniciar->boundingRect();
            textoReiniciar->setPos(centerX - rectReiniciar.width() / 2,
                                   centerY + 20);
            textoReiniciar->setVisible(true);
        }

        // Posicionar "Presiona ESC"
        if (textoMenu) {
            QRectF rectMenu = textoMenu->boundingRect();
            textoMenu->setPos(centerX - rectMenu.width() / 2,
                              centerY + 60);
            textoMenu->setVisible(true);
        }
    }

    if (fondo) fondo->setVisible(true);
}

void VictoriaScreen::ocultar()
{
    if (!visible) return;

    visible = false;

    if (fondo) fondo->setVisible(false);
    if (textoVictoria) textoVictoria->setVisible(false);
    if (textoPuntos) textoPuntos->setVisible(false);
    if (textoReiniciar) textoReiniciar->setVisible(false);
    if (textoMenu) textoMenu->setVisible(false);
}
