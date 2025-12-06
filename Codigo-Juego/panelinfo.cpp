#include "panelinfo.h"
#include <QFont>
#include <QStringList>
#include <QGraphicsDropShadowEffect>
#include <QBrush>
#include <QColor>
#include <QFontDatabase>
#include <QDebug>
#include <QPen>
#include <QPixmap>
#include <QPainter>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>
#include <QCoreApplication>

// ===================================================================
// CONSTANTES DE ESTILO
// ===================================================================
const int ALTO_PANEL = 50;
const int MARGEN_LATERAL = 20;
const int MARGEN_PAUSA = 10;
const int ANCHO_BOTON_PAUSA = 70;
const int ANCHO_CONTENEDOR_MENU = 250;
const int ALTO_CONTENEDOR_MENU = 180;
const QString RUTA_IMAGEN = ":/Recursos/Backgrounds/PERIDOIC-removebg-preview.png";

// ===================================================================
// CONSTRUCTOR
// ===================================================================
PanelInfo::PanelInfo(qreal sceneWidth, QGraphicsItem *parent)
    : QObject(nullptr),
    QGraphicsRectItem(parent),
    anchoEscena(sceneWidth),
    proxyBotonPausa(nullptr),
    botonPausa(nullptr),
    fondoPausa(nullptr),
    contenedorMenu(nullptr)
{
    setRect(0, 0, anchoEscena, ALTO_PANEL);

    // Carga y escalado de la imagen
    QPixmap pixmap(RUTA_IMAGEN);
    if (pixmap.isNull()) {
        setBrush(QBrush(Qt::white));
    } else {
        const int EXCESO_ANCHO = 100;
        QPixmap scaledPixmap = pixmap.scaled(
            anchoEscena + EXCESO_ANCHO,
            ALTO_PANEL,
            Qt::IgnoreAspectRatio,
            Qt::SmoothTransformation
            );
        QBrush textureBrush(scaledPixmap);
        qreal desplazamientoX = -(EXCESO_ANCHO / 2.0);
        QTransform transform;
        transform.translate(desplazamientoX, 0);
        textureBrush.setTransform(transform);
        setBrush(textureBrush);
    }

    setPen(QPen(Qt::NoPen));
    textoHUD = new QGraphicsTextItem(this);

    configurarEstilo();
    inicializarBotonPausa();
    inicializarMenuPausa();
}

PanelInfo::~PanelInfo()
{
    // Los hijos de QGraphicsItem se destruyen automáticamente
}

// ===================================================================
// INICIALIZACIÓN DE COMPONENTES DE PAUSA
// ===================================================================
void PanelInfo::inicializarBotonPausa()
{
    botonPausa = new QPushButton("PAUSA");
    botonPausa->setObjectName("BotonPausaHUD");
    botonPausa->setStyleSheet(
        "QPushButton#BotonPausaHUD {"
        "background-color: #2c3e50;"
        "color: white;"
        "border: 2px solid #34495e;"
        "border-radius: 5px;"
        "padding: 5px;"
        "font-weight: bold;"
        "}"
        "QPushButton#BotonPausaHUD:hover {"
        "background-color: #34495e;"
        "}"
        );
    botonPausa->setFixedSize(ANCHO_BOTON_PAUSA, ALTO_PANEL - (MARGEN_PAUSA * 2));

    connect(botonPausa, &QPushButton::clicked, this, &PanelInfo::manejarBotonPausaClic);

    proxyBotonPausa = new QGraphicsProxyWidget(this);
    proxyBotonPausa->setWidget(botonPausa);
    proxyBotonPausa->setPos(anchoEscena - ANCHO_BOTON_PAUSA - MARGEN_LATERAL, MARGEN_PAUSA);

    setBotonPausaVisible(true);
}

void PanelInfo::inicializarMenuPausa()
{
    // 1. Fondo Oscuro
    fondoPausa = new QGraphicsRectItem(this);
    qreal alturaFondo = 2000;

    fondoPausa->setRect(0, ALTO_PANEL, anchoEscena, alturaFondo);
    fondoPausa->setBrush(QBrush(QColor(0, 0, 0, 180)));
    fondoPausa->setZValue(9998);
    fondoPausa->setVisible(false);

    // 2. Contenedor de Botones (Widget)
    QWidget* widgetMenu = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widgetMenu);
    layout->setSpacing(10);

    QString style =
        "QPushButton {"
        "background-color: #ecf0f1;"
        "color: #2c3e50;"
        "border: 2px solid #bdc3c7;"
        "border-radius: 8px;"
        "padding: 10px;"
        "font-size: 16pt;"
        "font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "background-color: #bdc3c7;"
        "}";

    // 3. Crear Botones
    QPushButton* btnReanudar = new QPushButton("Reanudar Nivel");
    btnReanudar->setProperty("accion", QVariant::fromValue(AccionPausa::Reanudar));

    QPushButton* btnReiniciar = new QPushButton("Reiniciar Nivel");
    btnReiniciar->setProperty("accion", QVariant::fromValue(AccionPausa::ReiniciarNivel));

    QPushButton* btnMenuPrincipal = new QPushButton("Volver al Menú Principal");
    btnMenuPrincipal->setProperty("accion", QVariant::fromValue(AccionPausa::VolverMenu));

    btnReanudar->setStyleSheet(style);
    btnReiniciar->setStyleSheet(style);
    btnMenuPrincipal->setStyleSheet(style);

    connect(btnReanudar, &QPushButton::clicked, this, &PanelInfo::manejarBotonMenuClic);
    connect(btnReiniciar, &QPushButton::clicked, this, &PanelInfo::manejarBotonMenuClic);
    connect(btnMenuPrincipal, &QPushButton::clicked, this, &PanelInfo::manejarBotonMenuClic);

    layout->addWidget(btnReanudar);
    layout->addWidget(btnReiniciar);
    layout->addWidget(btnMenuPrincipal);

    widgetMenu->setLayout(layout);
    widgetMenu->setFixedSize(ANCHO_CONTENEDOR_MENU, ALTO_CONTENEDOR_MENU);

    // 4. Crear Proxy y Posicionar
    contenedorMenu = new QGraphicsProxyWidget(fondoPausa);
    contenedorMenu->setWidget(widgetMenu);

    qreal menuX = (anchoEscena - ANCHO_CONTENEDOR_MENU) / 2.0;
    qreal menuY = 200;
    contenedorMenu->setPos(menuX, ALTO_PANEL + menuY);

    contenedorMenu->setZValue(9999);
    setMenuPausaVisible(false);
}

// ===================================================================
// CONTROL DE VISIBILIDAD
// ===================================================================
void PanelInfo::setMenuPausaVisible(bool visible)
{
    qDebug() << (visible ? "👁️ Mostrando menú de pausa" : "🚫 Ocultando menú de pausa");

    if (fondoPausa) {
        fondoPausa->setVisible(visible);
    }

    if (contenedorMenu) {
        contenedorMenu->setVisible(visible);
    }

    if (textoHUD) {
        textoHUD->setVisible(!visible);
    }
}

void PanelInfo::setBotonPausaVisible(bool visible)
{
    qDebug() << (visible ? "👁️ Mostrando botón de pausa" : "🚫 Ocultando botón de pausa");

    if (proxyBotonPausa) {
        proxyBotonPausa->setVisible(visible);
    }
}

// ===================================================================
// MANEJADORES DE CLIC (SLOTS)
// ===================================================================
void PanelInfo::manejarBotonPausaClic()
{
    qDebug() << "🔵 Botón de PAUSA presionado";
    emit pausaPresionada();
}

void PanelInfo::manejarBotonMenuClic()
{
    QPushButton* boton = qobject_cast<QPushButton*>(sender());
    if (!boton) {
        qDebug() << "❌ Error: No se pudo obtener el botón";
        return;
    }

    QVariant accionVariant = boton->property("accion");
    if (!accionVariant.isValid()) {
        qDebug() << "❌ Error: La propiedad 'accion' no es válida";
        return;
    }

    AccionPausa accion = accionVariant.value<AccionPausa>();

    qDebug() << "🎯 Botón del menú presionado - Acción:" << static_cast<int>(accion);

    emit accionPausaSeleccionada(accion);
}

// ===================================================================
// CONFIGURACIÓN DE ESTILO
// ===================================================================
void PanelInfo::configurarEstilo()
{
    int fontId = QFontDatabase::addApplicationFont(":/Recursos/FuentesLetra/REVISTA.ttf");
    QString family = "Consolas";

    if (fontId != -1) {
        family = QFontDatabase::applicationFontFamilies(fontId).at(0);
    } else {
        qDebug() << "ADVERTENCIA: No se pudo cargar REVISTA.ttf. Usando Consolas.";
    }

    QFont font(family, 14);
    font.setBold(true);

    textoHUD->setFont(font);
    textoHUD->setDefaultTextColor(Qt::black);

    QGraphicsDropShadowEffect* shadow = new QGraphicsDropShadowEffect();
    shadow->setBlurRadius(2);
    shadow->setColor(QColor(100, 100, 100));
    shadow->setOffset(1, 1);
    textoHUD->setGraphicsEffect(shadow);

    setZValue(9999);
}

// ===================================================================
// ACTUALIZACIÓN DE DATOS
// ===================================================================
void PanelInfo::actualizar(const QMap<QString, QString>& dataset)
{
    const QString FUENTE_NUMERICA = "Consolas";

    QStringList lineas;
    const QString COLOR_TEXTO = "black";

    for (auto it = dataset.constBegin(); it != dataset.constEnd(); ++it) {
        QString clave = it.key();
        QString valor = it.value();

        QString valorFormateado = QString("<font face=\"%1\" color=\"%2\">%3</font>")
                                      .arg(FUENTE_NUMERICA, COLOR_TEXTO, valor);

        lineas.append(QString("%1: %2").arg(clave, valorFormateado));
    }

    QString separador = QString(" <font face=\"%1\" color=\"%2\">|</font> ").arg(FUENTE_NUMERICA, COLOR_TEXTO);
    QString textoFinal = lineas.join(separador);

    textoHUD->setHtml(textoFinal);

    qreal textoY = (ALTO_PANEL - textoHUD->boundingRect().height()) / 2.0;

    textoHUD->setPos(MARGEN_LATERAL, textoY);
}
