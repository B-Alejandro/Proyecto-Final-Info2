#include "Juego.h"
#include "Nivel.h"

#include <QScreen>
#include <QGuiApplication>
#include <QBrush>
#include <QColor>

Juego::Juego(QObject* parent)
    : QObject(parent), nivelActual(-1)
{
    crearVista();
    cargarNiveles();

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(actualizar()));
}

Juego::~Juego()
{
}
void Juego::crearVista()
{
    QScreen* screen = QGuiApplication::primaryScreen();

    int w = screen->geometry().width();
    int h = screen->availableGeometry().height() - 30;

    // crea escena del tamano inicial
    escena = new QGraphicsScene(0, 0, w, h, this);

    // crea la vista
    vista = new QGraphicsView(escena);

    // configura render
    vista->setRenderHint(QPainter::Antialiasing);

    // desactiva scrollbars para juego
    vista->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    vista->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // permite que la ventana pueda cambiar de tamano
    // sin bloquear la vista
    vista->setResizeAnchor(QGraphicsView::AnchorViewCenter);
    vista->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

    // tamano inicial
    vista->resize(w, h);

    // muestra la vista
    vista->show();
}


void Juego::cargarNiveles()
{
    // Crear nivel 1
    Nivel* nivel1 = new Nivel(escena,1);
    niveles.append(nivel1);

    // Posible carga de mas niveles en el futuro
}

void Juego::iniciar()
{
    // Cargar el primer nivel automaticamente
    if (!niveles.isEmpty())
    {
        cambiarNivel(0);
    }

    timer->start(16); // ~60 FPS
}

void Juego::cambiarNivel(int id)
{
    if (id < 0 || id >= niveles.size())
        return;

    nivelActual = id;

    escena->clear();

    niveles[nivelActual]->cargar();
}

void Juego::actualizar()
{
    if (nivelActual >= 0 && nivelActual < niveles.size())
    {
        niveles[nivelActual]->actualizar();
    }
}
