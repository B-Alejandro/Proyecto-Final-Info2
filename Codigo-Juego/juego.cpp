#include "juego.h"
#include "nivel1.h"
#include "nivel2.h"
#include "nivel3.h"
#include "menuprincipal.h"

#include <QScreen>
#include <QGuiApplication>
#include <QApplication>
#include <QDebug>
#include <QCoreApplication>
#include <QKeyEvent>
#include <QGraphicsView>
#include <QPainter>

/*
  Clase Juego
  Maneja la vista principal, el cambio de niveles y el timer del juego.
*/

// ===================================================================
// CONSTRUCTOR
// ===================================================================

Juego::Juego(QObject* parent)
    : QObject(parent), nivelActual(-1), vistaAncho(0), vistaAlto(0), menuPrincipal(nullptr)
{
    crearVista();

    // 1. Inicialización del Menú Principal
    menuPrincipal = new MenuPrincipal(vistaAncho, vistaAlto, this);

    // 2. Conexión de señales del Menú a Juego
    connect(menuPrincipal, &MenuPrincipal::iniciarNivel,
            this, &Juego::cambiarNivel);

    connect(menuPrincipal, &MenuPrincipal::salirJuego,
            QCoreApplication::instance(), &QCoreApplication::quit);

    cargarNiveles();

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Juego::actualizar);

    qDebug() << "✅ Juego inicializado correctamente";
}

Juego::~Juego()
{
    qDeleteAll(niveles);
}

// ===================================================================
// INICIALIZACIÓN
// ===================================================================

void Juego::crearVista()
{
    QScreen* screen = QGuiApplication::primaryScreen();

    // Dimensiones de la vista
    vistaAncho = screen->geometry().width();
    vistaAlto = screen->availableGeometry().height() - 30;

    vista = new QGraphicsView();
    vista->setRenderHint(QPainter::Antialiasing);
    vista->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    vista->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    vista->setFixedSize(vistaAncho, vistaAlto);

    // Instalar filtro para manejar teclas globales (como ESC)
    vista->installEventFilter(this);

    qDebug() << "🖥️ Vista creada -" << vistaAncho << "x" << vistaAlto;
}

void Juego::cargarNiveles()
{
    qDebug() << "========================================";
    qDebug() << "📚 Cargando niveles...";

    qDeleteAll(niveles);
    niveles.clear();

    // Nivel 1 (índice 0)
    Nivel1* nivel1 = new Nivel1(this, this);
    niveles.append(nivel1);
    connect(nivel1, &Nivel1::volverAMenuPrincipal, this, &Juego::mostrarMenuPrincipal);
    qDebug() << "✅ Nivel 1 cargado en índice 0";

    // Nivel 2 (índice 1)
    Nivel2* nivel2 = new Nivel2(this, this);
    niveles.append(nivel2);
    // Conexión si Nivel2 tiene la señal volverAMenuPrincipal
    // connect(nivel2, &Nivel2::volverAMenuPrincipal, this, &Juego::mostrarMenuPrincipal);
    qDebug() << "✅ Nivel 2 cargado en índice 1";

    // Nivel 3 (índice 2)
    Nivel3* nivel3 = new Nivel3(this, this);
    niveles.append(nivel3);
    // Conexión si Nivel3 tiene la señal volverAMenuPrincipal
    // connect(nivel3, &Nivel3::volverAMenuPrincipal, this, &Juego::mostrarMenuPrincipal);
    qDebug() << "✅ Nivel 3 cargado en índice 2";

    qDebug() << "📊 Total de niveles cargados:" << niveles.size();
    qDebug() << "========================================";
}

void Juego::iniciar()
{
    qDebug() << "🚀 Iniciando juego...";
    mostrarMenuPrincipal();
    vista->show();
}

// ===================================================================
// CAMBIO DE ESCENAS
// ===================================================================

void Juego::mostrarMenuPrincipal()
{
    qDebug() << "========================================";
    qDebug() << "🏠 Volviendo al Menú Principal";

    // Detener la lógica del juego
    timer->stop();
    qDebug() << "⏸️ Timer de juego detenido";

    // Limpiar el nivel actual si lo hubiera
    if (nivelActual >= 0 && nivelActual < niveles.size())
    {
        qDebug() << "🧹 Limpiando nivel:" << nivelActual;
        niveles[nivelActual]->limpiar();
    }

    // Cambiar la escena a la del menú
    vista->setScene(menuPrincipal);
    nivelActual = -1; // -1 representa el menú principal
    qDebug() << "🎬 Escena cambiada al menú";

    // Iniciar el menú
    if (menuPrincipal) {
        menuPrincipal->iniciarTransicionEntrada();

        // Solo iniciar música si no está sonando
        if (menuPrincipal->musicaMenu) {
            if (menuPrincipal->musicaMenu->playbackState() != QMediaPlayer::PlayingState) {
                menuPrincipal->iniciarMusicaMenu();
                qDebug() << "🎵 Música del menú iniciada";
            } else {
                qDebug() << "🎵 Música del menú ya está sonando";
            }
        }
    }

    qDebug() << "✅ Menú Principal mostrado correctamente";
    qDebug() << "========================================";
}

void Juego::cambiarNivel(int id)
{
    qDebug() << "========================================";
    qDebug() << "🎮 Juego::cambiarNivel() llamado con ID:" << id;
    qDebug() << "📊 Niveles disponibles:" << niveles.size();

    // 🔥 CORRECCIÓN: Usar método público para detener la música
    if (menuPrincipal) {
        menuPrincipal->detenerMusica();
    }

    // Validar ID
    if (id < 0 || id >= niveles.size())
    {
        qWarning() << "❌ ERROR: ID de nivel no válido:" << id;
        qWarning() << "   Rango válido: 0 -" << (niveles.size() - 1);
        return;
    }

    // Limpiar el nivel anterior (si lo hay)
    if (nivelActual >= 0 && nivelActual < niveles.size())
    {
        qDebug() << "🧹 Limpiando nivel anterior:" << nivelActual;
        niveles[nivelActual]->limpiar();
    }

    nivelActual = id;
    qDebug() << "✅ Nivel actual establecido a:" << nivelActual;

    // 1. Configurar la vista con la escena del nuevo nivel
    QGraphicsScene* escenaNivel = niveles[nivelActual]->getEscena();
    if (!escenaNivel) {
        qWarning() << "❌ ERROR: La escena del nivel" << id << "es nullptr";
        return;
    }

    vista->setScene(escenaNivel);
    qDebug() << "🎬 Escena del nivel configurada";

    // 2. Cargar los elementos (jugador, enemigos, etc.)
    niveles[nivelActual]->cargarElementos();
    qDebug() << "🎮 Elementos del nivel cargados";

    // 3. Iniciar la lógica de juego
    timer->start(1000 / 60); // Iniciar el timer a 60 FPS
    qDebug() << "⏱️ Timer de juego iniciado a 60 FPS";
    qDebug() << "✅ NIVEL" << (id + 1) << "INICIADO CORRECTAMENTE";
    qDebug() << "========================================";
}

void Juego::actualizar()
{
    if (nivelActual >= 0 && nivelActual < niveles.size())
    {
        niveles[nivelActual]->actualizar();
    }
}

// ===================================================================
// MANEJO DE EVENTOS
// ===================================================================

bool Juego::eventFilter(QObject* obj, QEvent* event)
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
        Qt::Key key = static_cast<Qt::Key>(keyEvent->key());

        // 🔥 CORRECCIÓN: Delegar TODAS las teclas al nivel activo
        if (nivelActual >= 0 && nivelActual < niveles.size()) {

            // Si es Nivel 1
            if (nivelActual == 0) {
                Nivel1* nivel1 = dynamic_cast<Nivel1*>(niveles[nivelActual]);
                if (nivel1) {
                    // Delegar SIEMPRE al nivel (él decide qué hacer con cada tecla)
                    nivel1->manejarTecla(key);
                    return true; // Evento manejado por el nivel
                }
            }
            // Si es Nivel 2
            else if (nivelActual == 1) {
                Nivel2* nivel2 = dynamic_cast<Nivel2*>(niveles[nivelActual]);
                if (nivel2 && nivel2->estaEnGameOver()) {
                    nivel2->manejarTecla(key);
                    return true;
                }
            }
            // Si es Nivel 3
            else if (nivelActual == 2) {
                // Implementación similar cuando Nivel3 tenga el método manejarTecla
            }
        }
    }

    return QObject::eventFilter(obj, event);
}
