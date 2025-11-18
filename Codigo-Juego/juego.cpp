#include "juego.h"
#include "nivel1.h"
#include "nivel2.h"
#include "nivel3.h"

#include <QScreen>
#include <QGuiApplication>
#include <QBrush>
#include <QColor>

Juego::Juego(QObject* parent)
    : QObject(parent), nivelActual(-1), escena(0), vista(0)
{
    crearVista();
    cargarNiveles();

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(actualizar()));
}

Juego::~Juego()
{
    // Los niveles se eliminan automaticamente por ser hijos de QObject
    // La escena y vista se eliminan por el parent
}

/*
  Crea la vista y escena principales.
*/
void Juego::crearVista()
{
    QScreen* screen = QGuiApplication::primaryScreen();

    int w = screen->geometry().width();
    int h = screen->availableGeometry().height() - 30;

    // Crea escena del tamaño inicial
    escena = new QGraphicsScene(0, 0, w, h, this);

    // Crea la vista
    vista = new QGraphicsView(escena);

    // Configura render
    vista->setRenderHint(QPainter::Antialiasing);

    // Desactiva scrollbars para juego
    vista->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    vista->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // Permite que la ventana pueda cambiar de tamaño
    // sin bloquear la vista
    vista->setResizeAnchor(QGraphicsView::AnchorViewCenter);
    vista->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

    // Tamaño inicial
    vista->resize(w, h);

    // Muestra la vista
    vista->show();
}

/*
  Crea los niveles disponibles usando Nivel1, Nivel2, Nivel3.
*/
void Juego::cargarNiveles()
{
    // En cargarNiveles():
    niveles.append(new Nivel1(escena, this)); // Añadir 'this' como parent
    niveles.append(new Nivel2(escena, this));
    niveles.append(new Nivel3(escena, this));
    // Con esta alternativa, el destructor del nivel se llama automáticamente al destruir Juego.
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
    niveles[nivelActual]->cargarElementos();
}

void Juego::actualizar()
{
    if (nivelActual >= 0 && nivelActual < niveles.size())
    {
        niveles[nivelActual]->actualizar();
    }
}
