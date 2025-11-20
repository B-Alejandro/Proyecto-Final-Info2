#include "juego.h"
#include "nivel1.h"
#include "nivel2.h"
#include "nivel3.h"

#include <QScreen>
#include <QGuiApplication>

/*
  Clase Juego
  Maneja la vista principal, el cambio de niveles y el timer del juego.
*/

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
    /*
      Libera memoria de los niveles almacenados.
    */
    qDeleteAll(niveles);
}

/*
  Funcion: crearVista
  Crea la vista grafica principal y configura sus parametros.
*/
void Juego::crearVista()
{
    QScreen* screen = QGuiApplication::primaryScreen();

    vistaAncho = screen->geometry().width();
    vistaAlto = screen->availableGeometry().height() - 30;

    vista = new QGraphicsView();

    vista->setRenderHint(QPainter::Antialiasing);
    vista->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    vista->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    vista->setResizeAnchor(QGraphicsView::AnchorViewCenter);
    vista->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

    vista->resize(vistaAncho, vistaAlto);
    vista->show();
}

/*
  Funcion: cargarNiveles
  Instancia todos los niveles del juego.
*/
void Juego::cargarNiveles()
{
    Nivel1* nivel1 = new Nivel1(this);
    niveles.append(nivel1);

    Nivel2* nivel2 = new Nivel2(this);
    niveles.append(nivel2);

    Nivel3* nivel3 = new Nivel3(this);
    niveles.append(nivel3);
}

/*
  Funcion: iniciar
  Comienza el juego cargando el primer nivel y activando el timer.
*/
void Juego::iniciar()
{
    if (!niveles.isEmpty())
    {
        cambiarNivel(1);
    }

    timer->start(16);
}

/*
  Funcion: cambiarNivel
  Cambia al nivel solicitado, limpiando el anterior y configurando la vista.
*/
void Juego::cambiarNivel(int id)
{
    if (id < 0 || id >= niveles.size())
        return;

    if (nivelActual >= 0 && nivelActual < niveles.size()) {
        niveles[nivelActual]->limpiar();
    }

    nivelActual = id;

    vista->setScene(niveles[nivelActual]->getEscena());

    niveles[nivelActual]->cargarElementos();
}

/*
  Funcion: actualizar
  Llamado periodicamente por el timer para avanzar la logica del nivel.
*/
void Juego::actualizar()
{
    if (nivelActual >= 0 && nivelActual < niveles.size())
    {
        niveles[nivelActual]->actualizar();
    }
}
