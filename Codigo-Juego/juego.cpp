#include "juego.h"
#include "nivel1.h"
#include "nivel2.h"
#include "nivel3.h"

#include <QScreen>
#include <QGuiApplication>

Juego::Juego(QObject* parent)
    : QObject(parent), nivelActual(-1), vistaAncho(0), vistaAlto(0)
{
    crearVista();
    cargarNiveles();

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(actualizar()));
}

Juego::~Juego()
{
    qDeleteAll(niveles);
}

void Juego::crearVista()
{
    QScreen* screen = QGuiApplication::primaryScreen();

    vistaAncho = screen->geometry().width();
    vistaAlto = screen->availableGeometry().height() - 30;

    // Crear solo la vista sin escena
    vista = new QGraphicsView();

    // Configura render
    vista->setRenderHint(QPainter::Antialiasing);

    // Desactiva scrollbars para juego
    vista->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    vista->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // Permite que la ventana pueda cambiar de tamaño
    vista->setResizeAnchor(QGraphicsView::AnchorViewCenter);
    vista->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

    // Tamaño inicial
    vista->resize(vistaAncho, vistaAlto);

    // Muestra la vista
    vista->show();
}

void Juego::cargarNiveles()
{
    // Crear los niveles (cada uno crea su propia escena)
    Nivel1* nivel1 = new Nivel1(this);
    niveles.append(nivel1);

    Nivel2* nivel2 = new Nivel2(this);
    niveles.append(nivel2);

    Nivel3* nivel3 = new Nivel3(this);
    niveles.append(nivel3);
}

void Juego::iniciar()
{
    // Cargar el primer nivel automáticamente
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

    // Asignar la escena del nivel a la vista
    vista->setScene(niveles[nivelActual]->getEscena());

    // Cargar elementos del nivel
    niveles[nivelActual]->cargarElementos();
}

void Juego::actualizar()
{
    if (nivelActual >= 0 && nivelActual < niveles.size())
    {
        niveles[nivelActual]->actualizar();
    }
}
