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

// ===================================================================
// CONSTRUCTOR
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

    // 2. CARGAR FUENTE PERSONALIZADA
    QFontDatabase::addApplicationFont(":/Recursos/FuentesLetra/REVISTA.ttf");
    configurarFondo();

    // 3. Crear el panel contenedor central de botones
    QWidget* panelBotones = new QWidget();
    panelBotones->setFixedWidth(300);

    // Estilo de periódico/revista para el panel
    panelBotones->setStyleSheet(
        "QWidget {"
        // PROPIEDADES DE FONDO Y TEXTURA
        "background-image: url(:/Recursos/Backgrounds/PERIDOIC-removebg-preview.png);"
        "background-repeat: no-repeat;"
        "background-attachment: scroll;"
        "background-position: center;"
        "background-size: 100% 100%;"

        // Fondo de respaldo/papel
        "background-color: rgb(245, 245, 220);"

        // PROPIEDADES DE BORDE
        "border: 4px solid rgb(50, 50, 50);"
        "border-radius: 0px;"
        "padding: 5px;"

        // PROPIEDADES DE FUENTE Y TEXTO
        "font-family: 'REVISTA', 'serif';"
        "color: rgb(20, 20, 20);"
        "}"

        // Iluminación del panel completo al hacer hover
        "QWidget:hover {"
        "border: 4px solid rgb(150, 150, 150);"
        "background-color: rgb(255, 255, 240);"
        "}"
        );

    QVBoxLayout* layout = new QVBoxLayout(panelBotones);
    layout->setSpacing(5);
    layout->setContentsMargins(15, 15, 15, 15);

    // --- BOTONES DE NIVELES ---
    layout->addSpacing(10);

    // 🔥 CORRECCIÓN CRÍTICA: Los IDs deben ser 0, 1, 2 (índices del array en juego.cpp)
    layout->addWidget(crearBotonCaotico("NIVEL UNO: TOMA", panelBotones, nullptr, 0));
    layout->addWidget(crearBotonCaotico("NIVEL DOS: FUGA", panelBotones, nullptr, 1));
    layout->addWidget(crearBotonCaotico("NIVEL TRES: OPERACION", panelBotones, nullptr, 2));

    layout->addSpacing(10);

    // --- Botón de Salida ---
    layout->addWidget(crearBotonCaotico("SALIR DEL JUEGO", panelBotones, SLOT(onSalirClicked()), -1, true));

    panelBotones->setLayout(layout);

    // 4. SETUP DE LA VISTA Y LA TRANSICIÓN
    panelBotones->adjustSize();

    proxyPanel = addWidget(panelBotones);
    qreal x = (ancho - panelBotones->width()) / 2.0;
    qreal y = (alto - panelBotones->height()) / 2.0;
    proxyPanel->setPos(x, y);

    // --- TÍTULO ---
    QString tituloTexto = "1 MEMORIA DE ACERO 0";
    QLabel* titulo = new QLabel(tituloTexto.toUpper());

    // Usamos la fuente cargada "REVISTA"
    QFont fuenteTitulo("REVISTA", 24, QFont::ExtraBold);
    titulo->setFont(fuenteTitulo);
    titulo->setAlignment(Qt::AlignCenter);
    titulo->setFixedHeight(70);
    titulo->setFixedWidth(400);

    // Aplicar estilo con textura
    titulo->setStyleSheet(
        "color: rgb(0, 0, 0);"

        // Inclusión de textura
        "background-image: url(:/Recursos/Backgrounds/PERIDOIC-removebg-preview.png);"
        "background-repeat: no-repeat;"
        "background-attachment: scroll;"
        "background-position: center;"
        "background-size: 100% 100%;"

        // Color de papel para rellenar transparencias
        "background-color: rgb(245, 245, 220);"

        "padding: 10px;"
        "border-radius: 0px;"
        );

    QGraphicsProxyWidget* proxyTitulo = addWidget(titulo);

    // Posicionar el título
    qreal x_titulo = (ancho - titulo->width()) / 2.0;
    qreal y_titulo = proxyPanel->y() - titulo->height() - 30;
    proxyTitulo->setPos(x_titulo, y_titulo);

    // Efectos de sombra
    QGraphicsDropShadowEffect* shadow = new QGraphicsDropShadowEffect();
    shadow->setBlurRadius(10);
    shadow->setColor(QColor(0, 0, 0, 180));

    // Aplicar sombra al panel y al título
    proxyPanel->setGraphicsEffect(shadow);
    proxyTitulo->setGraphicsEffect(shadow);

    opacityEffect = new QGraphicsOpacityEffect(this);
    opacityEffect->setOpacity(opacidadActual);

    // Aplicar efecto de opacidad a ambos elementos
    proxyPanel->setGraphicsEffect(opacityEffect);
    proxyTitulo->setGraphicsEffect(opacityEffect);

    proxyPanel->setVisible(true);
    proxyTitulo->setVisible(true);

    transicionTimer = new QTimer(this);
    connect(transicionTimer, &QTimer::timeout, this, &MenuPrincipal::actualizarOpacidad);

    // 5. Crear el botón de música y iniciar todo
    crearBotonMusica(ancho, alto);

    QTimer::singleShot(10, this, [this]{
        iniciarTransicionEntrada();
        iniciarMusicaMenu();
    });
}

// ===================================================================
// LÓGICA DE MÚSICA
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

    qDebug() << "🎵 Música del menú iniciada";
}

// 🔥 NUEVO MÉTODO PÚBLICO para detener la música
void MenuPrincipal::detenerMusica()
{
    if (musicaMenu) {
        musicaMenu->stop();
        qDebug() << "🔇 Música del menú detenida";
    }
}

void MenuPrincipal::crearBotonMusica(int anchoVista, int altoVista)
{
    botonMusica = new QPushButton();
    botonMusica->setIcon(QIcon(":/Recursos/OtrosRecursos/sound-on.png"));
    botonMusica->setFixedSize(40, 40);

    // Estilo adaptado al look de revista (tinta/papel)
    botonMusica->setStyleSheet(
        "QPushButton {"
        "background-color: rgba(220, 220, 220, 180);"
        "border: 2px solid rgb(0, 0, 0);"
        "border-radius: 5px;"
        "}"
        "QPushButton:hover { background-color: rgba(255, 255, 255, 200); }"
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
        qDebug() << "🔇 Música pausada.";
    } else {
        musicaMenu->play();
        botonMusica->setIcon(QIcon(":/Recursos/OtrosRecursos/sound-on.png"));
        qDebug() << "🔊 Música reanudada.";
    }
}

// ===================================================================
// LÓGICA DE NIVELES Y BOTONES
// ===================================================================

void MenuPrincipal::onIniciarNivelClicked(int idNivel)
{
    qDebug() << "🎮 MenuPrincipal: Iniciando nivel con ID:" << idNivel;
    emit iniciarNivel(idNivel);
}

void MenuPrincipal::onSalirClicked()
{
    qDebug() << "👋 Saliendo del juego...";
    emit salirJuego();
}

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
            "color: rgb(180, 0, 0);"
            "border: 3px solid rgb(180, 0, 0);"
            "border-radius: 4px;"

            "padding: 8px 15px;"
            "font-size: 14pt;"
            "font-family: 'REVISTA', 'serif';"
            "font-weight: bold;"
            "text-transform: uppercase;"

            "}"
            "QPushButton:hover {"
            "background-color: rgb(255, 240, 240);"
            "border-color: rgb(255, 0, 0);"
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
            "color: rgb(20, 20, 20);"
            "border: 2px solid rgb(20, 20, 20);"
            "border-radius: 0px;"

            "padding: 5px 10px;"
            "font-size: 11pt;"
            "font-family: 'REVISTA', 'serif';"
            "font-weight: bold;"
            "text-transform: uppercase;"
            "}"
            "QPushButton:hover {"
            "background-color: rgba(255, 255, 255, 200);"
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
    // 🔥 CORRECCIÓN: Verificar que idNivel sea >= 0 (no > 0)
    if (idNivel >= 0) {
        QSignalMapper *mapper = new QSignalMapper(this);
        connect(btn, SIGNAL(clicked()), mapper, SLOT(map()));
        mapper->setMapping(btn, idNivel);
        connect(mapper, SIGNAL(mappedInt(int)), this, SLOT(onIniciarNivelClicked(int)));

        qDebug() << "✅ Botón nivel creado:" << texto << "-> ID:" << idNivel;
    } else if (slot) {
        connect(btn, SIGNAL(clicked()), this, slot);
        qDebug() << "✅ Botón acción creado:" << texto;
    }

    // El layout se encarga de agregar el widget si ya tiene uno.
    if (contenedor && contenedor->layout()) {
        contenedor->layout()->addWidget(btn);
    }

    return btn;
}

// ===================================================================
// FUNCIONES DE UTILIDAD
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
        qDebug() << "⚠️ No se pudo cargar el fondo del menú";
    }
}

void MenuPrincipal::iniciarTransicionEntrada()
{
    opacidadActual = 0.0;
    if (opacityEffect) {
        opacityEffect->setOpacity(opacidadActual);
    }

    transicionTimer->start(INTERVALO_MS);
    qDebug() << "🎬 Iniciando transición de entrada...";
}

void MenuPrincipal::actualizarOpacidad()
{
    qreal incremento = (qreal)INTERVALO_MS / DURACION_TRANSICION_MS;
    opacidadActual += incremento;

    if (opacidadActual >= 1.0) {
        opacidadActual = 1.0;
        transicionTimer->stop();
        qDebug() << "✅ Transición de entrada finalizada.";
    }

    if (opacityEffect) {
        opacityEffect->setOpacity(opacidadActual);
    }
}
