// ============ GameOverScreen.cpp ============
#include "GameOverScreen.h"
#include <QFont>
#include <QBrush>
#include <QPen>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsView>
GameOverScreen::GameOverScreen(QGraphicsScene* scene, QObject* parent)
    : QObject(parent), escena(scene), fondo(nullptr),
    textoGameOver(nullptr), textoCapturado(nullptr),
    textoReiniciar(nullptr), textoMenu(nullptr), visible(false)
{
    crearElementos();
}

GameOverScreen::~GameOverScreen()
{
    // Los items se eliminan automáticamente con la escena
}

void GameOverScreen::crearElementos()
{
    if (!escena) return;

    QRectF sceneRect = escena->sceneRect();

    // Crear fondo semi-transparente que cubra TODA la escena
    fondo = new QGraphicsRectItem(sceneRect);
    fondo->setBrush(QBrush(QColor(0, 0, 0, 200))); // Negro semi-transparente
    fondo->setPen(QPen(Qt::NoPen));
    fondo->setZValue(10000); // Muy arriba para estar sobre todo
    fondo->setVisible(false);
    escena->addItem(fondo);

    // Los textos se posicionarán dinámicamente en mostrar()
    // para que siempre aparezcan en el centro de la vista actual

    // Crear texto "GAME OVER"
    textoGameOver = new QGraphicsTextItem("GAME OVER");
    QFont fontGameOver("Arial", 48, QFont::Bold);
    textoGameOver->setFont(fontGameOver);
    textoGameOver->setDefaultTextColor(Qt::red);
    textoGameOver->setZValue(10001);
    textoGameOver->setVisible(false);
    escena->addItem(textoGameOver);

    // Crear texto "Has sido capturado"
    textoCapturado = new QGraphicsTextItem("Has sido capturado");
    QFont fontCapturado("Arial", 24);
    textoCapturado->setFont(fontCapturado);
    textoCapturado->setDefaultTextColor(QColor(255, 100, 100)); // Rojo más claro
    textoCapturado->setZValue(10001);
    textoCapturado->setVisible(false);
    escena->addItem(textoCapturado);

    // Crear texto "Presiona R para Reiniciar"
    textoReiniciar = new QGraphicsTextItem("Presiona R para Reiniciar");
    QFont fontOpciones("Arial", 20);
    textoReiniciar->setFont(fontOpciones);
    textoReiniciar->setDefaultTextColor(Qt::white);
    textoReiniciar->setZValue(10001);
    textoReiniciar->setVisible(false);
    escena->addItem(textoReiniciar);

    // Crear texto "Presiona ESC para Menú"
    textoMenu = new QGraphicsTextItem("Presiona ESC para Menu");
    textoMenu->setFont(fontOpciones);
    textoMenu->setDefaultTextColor(Qt::white);
    textoMenu->setZValue(10001);
    textoMenu->setVisible(false);
    escena->addItem(textoMenu);
}

void GameOverScreen::mostrar()
{
    if (visible) return;

    visible = true;

    // Obtener la vista para saber dónde está centrada la cámara
    if (!escena->views().isEmpty()) {
        QGraphicsView* vista = escena->views().first();

        // Obtener el centro visible de la escena
        QPointF centroVista = vista->mapToScene(vista->viewport()->rect().center());

        qreal centerX = centroVista.x();
        qreal centerY = centroVista.y();

        // Posicionar "GAME OVER"
        if (textoGameOver) {
            QRectF rectGameOver = textoGameOver->boundingRect();
            textoGameOver->setPos(centerX - rectGameOver.width() / 2,
                                  centerY - rectGameOver.height() - 100);
            textoGameOver->setVisible(true);
        }

        // Posicionar "Has sido capturado"
        if (textoCapturado) {
            QRectF rectCapturado = textoCapturado->boundingRect();
            textoCapturado->setPos(centerX - rectCapturado.width() / 2,
                                   centerY - rectCapturado.height() - 20);
            textoCapturado->setVisible(true);
        }

        // Posicionar "Presiona R para Reiniciar"
        if (textoReiniciar) {
            QRectF rectReiniciar = textoReiniciar->boundingRect();
            textoReiniciar->setPos(centerX - rectReiniciar.width() / 2,
                                   centerY + 20);
            textoReiniciar->setVisible(true);
        }

        // Posicionar "Presiona ESC para Menú"
        if (textoMenu) {
            QRectF rectMenu = textoMenu->boundingRect();
            textoMenu->setPos(centerX - rectMenu.width() / 2,
                              centerY + 60);
            textoMenu->setVisible(true);
        }
    }

    if (fondo) fondo->setVisible(true);
}

void GameOverScreen::ocultar()
{
    if (!visible) return;

    visible = false;

    if (fondo) fondo->setVisible(false);
    if (textoGameOver) textoGameOver->setVisible(false);
    if (textoCapturado) textoCapturado->setVisible(false);
    if (textoReiniciar) textoReiniciar->setVisible(false);
    if (textoMenu) textoMenu->setVisible(false);
}
