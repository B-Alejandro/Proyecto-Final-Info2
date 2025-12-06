#include "menuprincipal.h"
#include <QDebug>
#include <QBrush>
#include <QColor>
#include <QGuiApplication>
#include <QPixmap>
#include <QFont>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTimer>
#include <QGraphicsOpacityEffect>
#include <QGraphicsProxyWidget>
#include <QGraphicsDropShadowEffect>
#include <QHBoxLayout>
#include <QIcon>
#include <QUrl>
#include <QFontDatabase>
#include <QSignalMapper>
#include <QMediaPlayer>
#include <QAudioOutput>

// Nota: Las constantes INTERVALO_MS y DURACION_TRANSICION_MS han sido movidas a la clase en menuprincipal.h

// ===================================================================
// CONSTRUCTOR (MODIFICADO)
// ===================================================================

MenuPrincipal::MenuPrincipal(int ancho, int alto, QObject* parent)
    : QGraphicsScene(parent),
    proxyPanel(nullptr),
    opacityEffect(nullptr),
    transicionTimer(nullptr),
    opacidadActual(0.0),
    botonMusica(nullptr),
    musicaMenu(nullptr),
    audioOutput(nullptr)
{
    // 1. Configurar dimensiones y fondo
    setSceneRect(0, 0, ancho, alto);

    //  CARGAR FUENTE PERSONALIZADA
    QFontDatabase::addApplicationFont(":/Recursos/FuentesLetra/REVISTA.ttf");
    configurarFondo();

    // 3. Crear el panel contenedor central de botones
    QWidget* panelBotones = new QWidget();
    panelBotones->setFixedWidth(300);

    // ------------------------------------------------------------------
    //  MODIFICACIÓN 1: ESTILO DE PERIÓDICO/REVISTA PARA EL PANEL (CON HOVER)
    // ------------------------------------------------------------------

    panelBotones->setStyleSheet(
        "QWidget {"
        // PROPIEDADES DE FONDO Y TEXTURA
        "background-image: url(:/Recursos/Backgrounds/PERIDOIC-removebg-preview.png);"
        "background-repeat: no-repeat;"
        "background-attachment: scroll;"
        "background-position: center;"
        "background-size: 100% 100%;" // Ajuste de imagen

        // Fondo de respaldo/papel
        "background-color: rgb(245, 245, 220);"

        // PROPIEDADES DE BORDE
        "border: 4px solid rgb(50, 50, 50);"      // Borde Color Tinta BASE
        "border-radius: 0px;"
        "padding: 5px;"

        // PROPIEDADES DE FUENTE Y TEXTO
        "font-family: 'REVISTA', 'serif';"
        "color: rgb(20, 20, 20);"
        "}"

        // ✅ ILUMINACIÓN DEL PANEL COMPLETO AL HACER HOVER
        "QWidget:hover {"
        "border: 4px solid rgb(150, 150, 150);" // Borde gris claro para resaltar
        "background-color: rgb(255, 255, 240);" // Fondo ligeramente más brillante
        "}"
        );

    QVBoxLayout* layout = new QVBoxLayout(panelBotones);
    layout->setSpacing(5);
    layout->setContentsMargins(15, 15, 15, 15);

    // --- BOTONES DE NIVELES ---
    layout->addSpacing(10);
    layout->addWidget(crearBotonCaotico("NIVEL UNO: TOMA", panelBotones, nullptr, 1));
    layout->addWidget(crearBotonCaotico("NIVEL DOS: FUGA", panelBotones, nullptr, 2));
    layout->addWidget(crearBotonCaotico("NIVEL TRES: OPERACION", panelBotones, nullptr, 3));
    layout->addSpacing(10);

    // --- Botón de Salida Caótico ---
    layout->addWidget(crearBotonCaotico("SALIR DEL JUEGO 2", panelBotones, SLOT(onSalirClicked()), 0, true));

    panelBotones->setLayout(layout);

    // 4. SETUP DE LA VISTA Y LA TRANSICIÓN
    panelBotones->adjustSize();

    proxyPanel = addWidget(panelBotones);
    qreal x = (ancho - panelBotones->width()) / 2.0;
    qreal y = (alto - panelBotones->height()) / 2.0;
    proxyPanel->setPos(x, y);

    // -----------------------------------------------------------
    //  MODIFICACIÓN 2: ESTILO DE TÍTULO DE REVISTA CON TEXTURA Y AJUSTE
    // -----------------------------------------------------------

    QString tituloTexto = "1 MEMORIA DE ACERO 0";
    QLabel* titulo = new QLabel(tituloTexto.toUpper());

    // Usamos la fuente cargada "REVISTA"
    QFont fuenteTitulo("REVISTA", 24, QFont::ExtraBold);
    titulo->setFont(fuenteTitulo);
    titulo->setAlignment(Qt::AlignCenter);
    titulo->setFixedHeight(70);
    titulo->setFixedWidth(400);

    // APLICAR ESTILO: SOLO IMAGEN DE FONDO DE TEXTURA, ELIMINANDO CUALQUIER COLOR DE FONDO.
    titulo->setStyleSheet(
        "color: rgb(0, 0, 0);" // Color de Tinta Negra

        // **INCLUSIÓN DE TEXTURA**
        "background-image: url(:/Recursos/Backgrounds/PERIDOIC-removebg-preview.png);"
        "background-repeat: no-repeat;"
        "background-attachment: scroll;"
        "background-position: center;"
        "background-size: 100% 100%;" // Ajuste de imagen al tamaño del QLabel

        // Se mantiene el color de papel para rellenar las transparencias de la imagen.
        "background-color: rgb(245, 245, 220);"

        "padding: 10px;"
        "border-radius: 0px;"
        );

    QGraphicsProxyWidget* proxyTitulo = addWidget(titulo);

    // Posicionar el título
    qreal x_titulo = (ancho - titulo->width()) / 2.0;
    qreal y_titulo = proxyPanel->y() - titulo->height() - 30;
    proxyTitulo->setPos(x_titulo, y_titulo);

    // -----------------------------------------------------------

    QGraphicsDropShadowEffect* shadow = new QGraphicsDropShadowEffect();
    shadow->setBlurRadius(10);
    shadow->setColor(QColor(0, 0, 0, 180));
    // Aplicamos la sombra al panel y al título
    proxyPanel->setGraphicsEffect(shadow);
    proxyTitulo->setGraphicsEffect(shadow);

    opacityEffect = new QGraphicsOpacityEffect(this);
    opacityEffect->setOpacity(opacidadActual);

    // Aplicamos el efecto de opacidad a ambos elementos
    proxyPanel->setGraphicsEffect(opacityEffect);
    proxyTitulo->setGraphicsEffect(opacityEffect);

    proxyPanel->setVisible(true);
    proxyTitulo->setVisible(true);

    transicionTimer = new QTimer(this);
    connect(transicionTimer, &QTimer::timeout, this, &MenuPrincipal::actualizarOpacidad);

    // 2. CREAR EL BOTÓN DE MÚSICA (IZQUIERDA) y iniciar todo
    crearBotonMusica(ancho, alto);

    QTimer::singleShot(10, this, [this]{
        iniciarTransicionEntrada();
        iniciarMusicaMenu();
    });
}

// ===================================================================
// LÓGICA DE MÚSICA (SIN CAMBIOS FUNCIONALES, SOLO ESTÉTICOS)
// ===================================================================

void MenuPrincipal::iniciarMusicaMenu()
{
    musicaMenu = new QMediaPlayer(this);
    audioOutput = new QAudioOutput(this);
    musicaMenu->setAudioOutput(audioOutput);

    musicaMenu->setSource(QUrl("qrc:/ltima-fortaleza-en-ascenso.mp3"));

    audioOutput->setVolume(0.5);
    musicaMenu->setLoops(QMediaPlayer::Infinite);
    musicaMenu->play();
}

void MenuPrincipal::crearBotonMusica(int anchoVista, int altoVista)
{
    botonMusica = new QPushButton();
    botonMusica->setIcon(QIcon(":/Recursos/OtrosRecursos/sound-on.png"));
    botonMusica->setFixedSize(40, 40);

    // Estilo adaptado al look de revista (tinta/papel)
    botonMusica->setStyleSheet(
        "QPushButton {"
        "background-color: rgba(220, 220, 220, 180);" // Fondo gris claro (papel)
        "border: 2px solid rgb(0, 0, 0);" // Borde de tinta negra
        "border-radius: 5px;"
        "}"
        // ✅ HOVER para el botón de música
        "QPushButton:hover { background-color: rgba(255, 255, 255, 200); }" // Tono más brillante/blanco al pasar
        );

    connect(botonMusica, &QPushButton::clicked, this, &MenuPrincipal::onToggleMusicaClicked);

    QGraphicsProxyWidget* proxyMusica = addWidget(botonMusica);
    proxyMusica->setPos(20, 20);
}

void MenuPrincipal::onToggleMusicaClicked()
{
    if (!musicaMenu) return;

    if (musicaMenu->playbackState() == QMediaPlayer::PlayingState) {
        musicaMenu->pause();
        botonMusica->setIcon(QIcon(":/Recursos/OtrosRecursos/sound-off.png"));
        qDebug() << "Música pausada.";
    } else {
        musicaMenu->play();
        botonMusica->setIcon(QIcon(":/Recursos/OtrosRecursos/sound-on.png"));
        qDebug() << "Música reanudada.";
    }
}

// ===================================================================
// LÓGICA DE NIVELES Y BOTONES (MODIFICADO - CON HOVER)
// ===================================================================

void MenuPrincipal::onIniciarNivelClicked(int idNivel)
{
    emit iniciarNivel(idNivel);
}


// ⬇️ FUNCIÓN MODIFICADA CON ESTILOS DE TINTA DE PERIÓDICO Y HOVER
QPushButton* MenuPrincipal::crearBotonCaotico(const QString& texto, QWidget* contenedor, const char* slot, int idNivel, bool esBotonSalir)
{
    QPushButton* btn = new QPushButton(texto, contenedor);

    QString styleSheet;

    if (esBotonSalir)
    {
        // 🚨 ESTILO PARA EL BOTÓN DE SALIR (TINTA ROJA/ÉNFASIS)
        styleSheet =
            "QPushButton {"
            "background-color: rgb(240, 240, 240);"
            "color: rgb(180, 0, 0);" // Tinta Roja de Advertencia
            "border: 3px solid rgb(180, 0, 0);" // Borde de Tinta Roja
            "border-radius: 4px;"

            "padding: 8px 15px;"
            "font-size: 14pt;"
            "font-family: 'REVISTA', 'serif';"
            "font-weight: bold;"
            "text-transform: uppercase;"

            "}"
            "QPushButton:hover {"
            "background-color: rgb(255, 240, 240);" // ✅ ILUMINACIÓN: Fondo casi blanco/rosado brillante
            "border-color: rgb(255, 0, 0);" // Borde rojo más vivo
            "border-style: solid;"
            "}"
            "QPushButton:pressed {"
            "background-color: rgb(200, 150, 150);"
            "padding-top: 10px;"
            "}";
    }
    else
    {
        // 📰 ESTILO PARA BOTONES DE NIVEL (TINTA NEGRA DE ARTÍCULO)
        styleSheet =
            "QPushButton {"
            // Fondo transparente para heredar la textura de papel del panel
            "background-color: transparent;"
            "color: rgb(20, 20, 20);" // Tinta Negra
            "border: 2px solid rgb(20, 20, 20);" // Borde Tinta Fina
            "border-radius: 0px;" // Borde cuadrado

            "padding: 5px 10px;"
            "font-size: 11pt;"
            "font-family: 'REVISTA', 'serif';"
            "font-weight: bold;"
            "text-transform: uppercase;"
            "}"
            "QPushButton:hover {"
            "background-color: rgba(255, 255, 255, 200);" // ✅ ILUMINACIÓN: Blanco casi opaco sobre la textura
            "border-style: solid;"
            "}"
            "QPushButton:pressed {"
            "background-color: rgb(180, 180, 180);"
            "padding-top: 7px;"
            "}"
            "QPushButton:disabled {"
            "background-color: transparent;"
            "border-color: rgb(120, 120, 120);"
            "color: rgb(120, 120, 120);"
            "}";
    }

    btn->setStyleSheet(styleSheet);

    // Conexión con QSignalMapper
    if (idNivel > 0) {
        QSignalMapper *mapper = new QSignalMapper(this);
        connect(btn, SIGNAL(clicked()), mapper, SLOT(map()));
        mapper->setMapping(btn, idNivel);
        connect(mapper, SIGNAL(mappedInt(int)), this, SLOT(onIniciarNivelClicked(int)));
    } else if (slot) {
        connect(btn, SIGNAL(clicked()), this, slot);
    }

    // El layout se encarga de agregar el widget si ya tiene uno.
    if (contenedor && contenedor->layout()) {
        contenedor->layout()->addWidget(btn);
    }

    return btn;
}


// ===================================================================
// FUNCIONES DE UTILIDAD (SIN CAMBIOS FUNCIONALES)
// ===================================================================

void MenuPrincipal::configurarFondo()
{
    QPixmap fondoPixmap(":/Recursos/Backgrounds/Fondo.jpg");

    if (!fondoPixmap.isNull()) {
        QPixmap fondoEscalado = fondoPixmap.scaled(
            sceneRect().size().toSize(),
            Qt::IgnoreAspectRatio,
            Qt::SmoothTransformation
            );
        setBackgroundBrush(QBrush(fondoEscalado));
    } else {
        setBackgroundBrush(QBrush(QColor(30, 0, 0)));
    }
}

void MenuPrincipal::iniciarTransicionEntrada()
{
    opacidadActual = 0.0;
    if (opacityEffect) {
        opacityEffect->setOpacity(opacidadActual);
    }

    transicionTimer->start(INTERVALO_MS);
    qDebug() << "Iniciando transición de entrada...";
}

void MenuPrincipal::actualizarOpacidad()
{
    qreal incremento = (qreal)INTERVALO_MS / DURACION_TRANSICION_MS;
    opacidadActual += incremento;

    if (opacidadActual >= 1.0) {
        opacidadActual = 1.0;
        transicionTimer->stop();
        qDebug() << "Transición de entrada finalizada.";
    }

    if (opacityEffect) {
        opacityEffect->setOpacity(opacidadActual);
    }
}
