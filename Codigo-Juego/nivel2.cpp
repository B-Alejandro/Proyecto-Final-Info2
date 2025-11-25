
// nivel2.cpp - CORREGIDO y alineado con Persona/Enemigo corregidos
#include "nivel2.h"
#include "juego.h"
#include "jugador.h"
#include "enemigo.h"
#include "Obstaculo.h"
#include "persona.h"
#include "fisica.h"
#include "GameOverScreen.h"

#include <QGraphicsView>
#include <QBrush>
#include <QRandomGenerator>
#include <QDebug>
#include <QTimer>
#include <QPixmap>
#include <QPainter>
#include <QElapsedTimer>
#include <cmath>

Nivel2::Nivel2(Juego* juego, QObject* parent)
    : NivelBase(juego, 2, parent),
    enemigoAtras(nullptr),
    suelo(nullptr),
    pantallaGameOver(nullptr),
    juegoEnPausa(false),
    fondo1(nullptr),
    fondo2(nullptr),
    anchoFondo(0),
    vistaAncho(0),
    vistaAlto(0)
{
    if (juego) {
        vistaAncho = juego->getVistaAncho();
        vistaAlto = juego->getVistaAlto();
    }

    int anchoEscena = vistaAncho * 5;
    int altoEscena = vistaAlto;

    crearEscena(anchoEscena, altoEscena);

    connect(this, &Nivel2::juegoTerminado, this, &Nivel2::onJuegoTerminado);
}

Nivel2::~Nivel2()
{
    if (pantallaGameOver) {
        delete pantallaGameOver;
        pantallaGameOver = nullptr;
    }
}

void Nivel2::crearFondoDinamico()
{
    QPixmap fondoPixmap(":/Recursos/Backgrounds/unnamed (1).jpg");

    if (fondoPixmap.isNull()) {
        qDebug() << "No se pudo cargar la imagen del fondo, creando fondo de respaldo";
        fondoPixmap = QPixmap(sceneW, sceneH);
        fondoPixmap.fill(QColor(135, 206, 235));
    } else {
        qDebug() << "Imagen del fondo cargada correctamente";

        int vecesRepetir = (sceneW / fondoPixmap.width()) + 2;
        QPixmap fondoExtendido(fondoPixmap.width() * vecesRepetir, fondoPixmap.height());
        fondoExtendido.fill(Qt::transparent);
        QPainter painter(&fondoExtendido);
        for (int i = 0; i < vecesRepetir; i++) {
            painter.drawPixmap(i * fondoPixmap.width(), 0, fondoPixmap);
        }
        painter.end();
        fondoPixmap = fondoExtendido;
        fondoPixmap = fondoPixmap.scaledToHeight(sceneH, Qt::SmoothTransformation);
    }

    anchoFondo = fondoPixmap.width();

    if (fondo1) {
        escena->removeItem(fondo1);
        delete fondo1;
        fondo1 = nullptr;
    }
    if (fondo2) {
        escena->removeItem(fondo2);
        delete fondo2;
        fondo2 = nullptr;
    }

    fondo1 = new QGraphicsPixmapItem(fondoPixmap);
    fondo2 = new QGraphicsPixmapItem(fondoPixmap);

    fondo1->setPos(0, 0);
    fondo2->setPos(anchoFondo, 0);

    fondo1->setZValue(-1000);
    fondo2->setZValue(-1000);

    if (escena) {
        escena->addItem(fondo1);
        escena->addItem(fondo2);
    }

    qDebug() << "Fondo dinamico creado";
}

void Nivel2::actualizarFondo(qreal posicionCamara)
{
    if (!fondo1 || !fondo2) return;
    if (anchoFondo <= 0) return;

    qreal desplazamiento = posicionCamara * 0.5;
    qreal posRelativa = std::fmod(desplazamiento, anchoFondo);
    if (posRelativa < 0) posRelativa += anchoFondo;

    fondo1->setX(desplazamiento - posRelativa);
    fondo2->setX(fondo1->x() + anchoFondo);

    if (fondo2->x() < posicionCamara - anchoFondo) {
        fondo2->setX(fondo1->x() + anchoFondo);
    }
    if (fondo1->x() < posicionCamara - anchoFondo) {
        fondo1->setX(fondo2->x() + anchoFondo);
    }
}

void Nivel2::configurarNivel()
{
    crearFondoDinamico();

    qreal alturaSuelo = sceneH * 0.85;

    // Crear jugador con tamaño inicial provisional
    qreal anchoJugador = sceneH * 0.15;
    qreal altoJugador  = sceneH * 0.15;

    jugador = new Jugador(anchoJugador, altoJugador, sceneW, sceneH, TipoMovimiento::CON_GRAVEDAD);
    jugador->setVida(100);
    jugador->setZValue(100);

    if (escena) escena->addItem(jugador);

    Jugador* j = dynamic_cast<Jugador*>(jugador);
    if (j)
    {
        j->cargarSpritesnivel2();

        // Obtener altura real del sprite
        int altoReal = static_cast<int>(j->boundingRect().height());

        // Reposicionar correctamente sobre el suelo
        qreal posJugadorX = 300;
        qreal posJugadorY = alturaSuelo - altoReal;

        jugador->setPos(posJugadorX, posJugadorY);

        qDebug() << "Jugador posicionado correctamente en" << posJugadorX << posJugadorY;
    }

    if (escena) {
        pantallaGameOver = new GameOverScreen(escena, this);
    }

    if (escena && !escena->views().isEmpty() && jugador) {
        QGraphicsView* vista = escena->views().first();

        // reducir flicker
        vista->setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
        vista->setRenderHint(QPainter::Antialiasing, false);
        vista->setRenderHint(QPainter::SmoothPixmapTransform, true);

        // asegurar foco en el jugador para recibir teclas
        jugador->setFlag(QGraphicsItem::ItemIsFocusable);
        jugador->setFocus();

        vista->centerOn(jugador);
    }
}

void Nivel2::crearEnemigos()
{
    if (!jugador) return;

    qreal alturaSuelo = sceneH * 0.85;
    qreal sizeEnemigo = sceneH * 0.15;

    // Posicionar enemigo VISIBLE en la pantalla, a la izquierda del jugador
    qreal posEnemigoX = jugador->x() - vistaAncho * 0.3;
    qreal posEnemigoY = alturaSuelo - sizeEnemigo;

    qDebug() << "=== CREANDO ENEMIGO ===";
    qDebug() << "Scene Height:" << sceneH;
    qDebug() << "Altura suelo:" << alturaSuelo;
    qDebug() << "Tamaño enemigo:" << sizeEnemigo;
    qDebug() << "Posicion enemigo X:" << posEnemigoX << "Y:" << posEnemigoY;
    qDebug() << "Y del suelo deberia ser:" << alturaSuelo;

    enemigoAtras = new Enemigo(sizeEnemigo,
                               sizeEnemigo,
                               sceneW,
                               sceneH,
                               TipoMovimiento::CON_GRAVEDAD,
                               2);

    // Cargar sprites ANTES de establecer animacion y posicion final
    enemigoAtras->cargarSprites();

    // obtener alto real y posicionar sobre suelo
    int altoRealEnemigo = static_cast<int>(enemigoAtras->boundingRect().height());
    posEnemigoY = alturaSuelo - altoRealEnemigo;
    enemigoAtras->setPos(posEnemigoX, posEnemigoY);

    // Configuracion visual
    enemigoAtras->setBrush(QBrush(Qt::red));
    enemigoAtras->setVida(100);
    enemigoAtras->setZValue(100);
    enemigoAtras->setDireccionIzquierda(false);

    // CRITICO: Configurar fisica ANTES de agregar a la escena
    enemigoAtras->setSpeed(0);  // Sin velocidad hasta que se active
    enemigoAtras->vy = 0.0;     // Velocidad vertical en 0
    enemigoAtras->onGround = true;  // Marcado como en el suelo
    enemigoAtras->g = 0.5;      // Gravedad igual que el jugador

    // Asegurar que las teclas estan apagadas
    enemigoAtras->leftPressed = false;
    enemigoAtras->rightPressed = false;
    enemigoAtras->upPressed = false;
    enemigoAtras->downPressed = false;

    qDebug() << "CONFIGURACION FISICA DEL ENEMIGO:";
    qDebug() << "  Speed:" << enemigoAtras->getSpeed();
    qDebug() << "  vy:" << enemigoAtras->vy;
    qDebug() << "  onGround:" << enemigoAtras->onGround;
    qDebug() << "  g:" << enemigoAtras->g;
    qDebug() << "  Posicion Y:" << enemigoAtras->y();
    qDebug() << "  Altura suelo esperada:" << alturaSuelo;

    enemigoAtras->setAnimacion(EstadoAnimacion::IDLE);

    // Agregar a la escena
    if (escena) {
        escena->addItem(enemigoAtras);
    }
    enemigos.append(enemigoAtras);

    qDebug() << "Enemigo creado y agregado a la escena";
    qDebug() << "Esperando movimiento del jugador para activar...\\n";
}

void Nivel2::crearObstaculos()
{
    qreal posYSuelo = sceneH * 0.85;
    qreal alturaSuelo = sceneH * 0.15;

    qDebug() << "=== CREANDO OBSTACULOS ===";
    qDebug() << "Suelo Y:" << posYSuelo << "Altura:" << alturaSuelo;

    // Crear suelo completo
    suelo = new Obstaculo(0, posYSuelo, sceneW, alturaSuelo, Qt::darkGreen);
    suelo->setTextura(":/Recursos/Backgrounds/tierra.jpg", true);
    suelo->setZValue(-10);
    if (escena) escena->addItem(suelo);
    obstaculos.append(suelo);

    qDebug() << "Suelo creado de 0 a" << sceneW;

    QStringList texturas = {
        ":/Recursos/Objects/Obstaculos2.png"
    };

    // Obstaculos con posiciones mas espaciadas
    struct ObstaculoInfo {
        qreal posX;
        int ancho;
        int alto;
    };

    QList<ObstaculoInfo> obstaculosInfo = {
        {600, 60, 60},
        {1000, 70, 70},
        {1400, 65, 65},
        {1800, 75, 75},
        {2200, 60, 60},
        {2600, 80, 80},
        {3000, 65, 65},
        {3400, 70, 70},
        {3800, 75, 75},
        {4200, 60, 60},
        {4600, 70, 70}
    };

    for (const ObstaculoInfo& info : obstaculosInfo) {
        // Posicionar SOBRE el suelo correctamente
        qreal posY = posYSuelo - info.alto;

        Obstaculo* obs = new Obstaculo(info.posX, posY, info.ancho, info.alto, Qt::darkGray);
        obs->setZValue(10);

        if (QRandomGenerator::global()->bounded(2) == 0) {
            QString textura = texturas[QRandomGenerator::global()->bounded(texturas.size())];
            obs->setTextura(textura, false);
        }

        obs->setBorderColor(Qt::black, 2);
        if (escena) escena->addItem(obs);
        obstaculos.append(obs);

        qDebug() << "Obstaculo en X:" << info.posX << "Y:" << posY
                 << "Ancho:" << info.ancho << "Alto:" << info.alto;
    }

    qDebug() << "Total obstaculos creados:" << obstaculosInfo.size();
}

void Nivel2::actualizar()
{
    if (juegoEnPausa) {
        return;
    }

    // actualizar fisica del nivel (NivelBase se encarga de llamar updateMovement de cada entidad)
    NivelBase::actualizar();

    if (!jugador || !escena || escena->views().isEmpty()) {
        return;
    }

    QGraphicsView* vista = escena->views().first();

    // actualizar fondo parallax
    actualizarFondo(jugador->x());

    // detectar si jugador se movio
    static qreal ultimaPosicionJugador = jugador->x();
    bool jugadorSeMovio = (jugador->x() != ultimaPosicionJugador);
    ultimaPosicionJugador = jugador->x();

    // activar enemigo cuando jugador empiece a moverse
    if (jugadorSeMovio && enemigoAtras && !enemigoAtras->estaActivo()) {
        enemigoAtras->activarPersecucion();
    }

    // movimiento base del enemigo si esta activo: dejar que Enemigo maneje la direccion
    if (enemigoAtras && jugador->estaVivo()) {
        // correccion de suelo del enemigo
        qreal alturaSuelo = sceneH * 0.85;
        qreal alturaEnemigo = enemigoAtras->boundingRect().height();
        qreal posYCorrecta = alturaSuelo - alturaEnemigo;

        if (enemigoAtras->y() > posYCorrecta + 2) {
            enemigoAtras->setY(posYCorrecta);
            enemigoAtras->vy = 0;
            enemigoAtras->onGround = true;
        }

        if (qAbs(enemigoAtras->y() - posYCorrecta) < 5) {
            enemigoAtras->onGround = true;
            if (enemigoAtras->vy > 0) {
                enemigoAtras->vy = 0;
            }
        }
    }

    // deteccion de colision con cooldown y separacion
    if (enemigoAtras && jugador && jugador->estaVivo()) {
        QRectF rectJugador = jugador->sceneBoundingRect();
        QRectF rectEnemigo = enemigoAtras->sceneBoundingRect();

        static QElapsedTimer danoTimer;
        if (!danoTimer.isValid()) {
            danoTimer.start();
        }

        if (rectJugador.intersects(rectEnemigo)) {

            if (danoTimer.elapsed() > 300) {
                jugador->recibirDanio(50);
                danoTimer.restart();

                // separacion para evitar dano continuo
                qreal push = 30.0;

                if (jugador->x() < enemigoAtras->x()) {
                    jugador->setX(jugador->x() - push);
                    enemigoAtras->setX(enemigoAtras->x() + push * 0.5);
                } else {
                    jugador->setX(jugador->x() + push);
                    enemigoAtras->setX(enemigoAtras->x() - push * 0.5);
                }

                // corregir Y en piso
                qreal alturaSuelo2 = sceneH * 0.85;

                jugador->setY(alturaSuelo2 - jugador->boundingRect().height());
                enemigoAtras->setY(alturaSuelo2 - enemigoAtras->boundingRect().height());
            }

            if (!jugador->estaVivo()) {

                Jugador* j = dynamic_cast<Jugador*>(jugador);
                if (j) {
                    j->activarAnimacionMuerte();
                }

                if (enemigoAtras) {
                    enemigoAtras->activarAnimacionMuerte();
                }

                juegoEnPausa = true;

                QTimer::singleShot(900, this, [this]() {
                    emit juegoTerminado();
                });

                return;
            }
        }
    }

    // movimiento de camara
    qreal camaraX = jugador->x();
    qreal mitadVista = vistaAncho * 0.5;

    if (camaraX < mitadVista) {
        camaraX = mitadVista;
    } else if (camaraX > sceneW - mitadVista) {
        camaraX = sceneW - mitadVista;
    }

    vista->centerOn(camaraX, sceneH * 0.5);
}

void Nivel2::onJuegoTerminado()
{
    qDebug() << "onJuegoTerminado llamado";

    if (pantallaGameOver) {
        qDebug() << "Mostrando pantalla de Game Over";
        pantallaGameOver->mostrar();
    } else {
        qDebug() << "ERROR: pantallaGameOver es NULL";
    }
}

void Nivel2::manejarTecla(Qt::Key key)
{
    if (!juegoEnPausa) return;

    qDebug() << "Tecla presionada en Game Over:" << key;

    if (key == Qt::Key_R) {
        qDebug() << "Reiniciando nivel...";
        if (juego) juego->iniciar();

        if (pantallaGameOver) {
            pantallaGameOver->ocultar();
        }

        juegoEnPausa = false;

        if (jugador) {
            Jugador* j = dynamic_cast<Jugador*>(jugador);
            if (j) {
                j->reanudarAnimacion();
                j->setAnimacion(EstadoAnimacion::IDLE);
                j->setVida(100);
            }

            qreal posJugadorX = 300;
            qreal alturaSuelo = sceneH * 0.85;
            qreal posJugadorY = alturaSuelo - sceneH * 0.15;
            jugador->setPos(posJugadorX, posJugadorY);
        }

        if (enemigoAtras) {
            enemigoAtras->reanudarAnimacion();
            enemigoAtras->setVida(100);
            enemigoAtras->setSpeed(0);
            enemigoAtras->setAnimacion(EstadoAnimacion::IDLE);
            enemigoAtras->setDireccionIzquierda(false);

            qreal alturaSuelo = sceneH * 0.85;
            qreal sizeEnemigo = sceneH * 0.15;
            qreal posEnemigoX = jugador->x() - vistaAncho * 0.3;
            qreal posEnemigoY = alturaSuelo - sizeEnemigo;

            enemigoAtras->setPos(posEnemigoX, posEnemigoY);
        }

        if (fondo1 && fondo2) {
            fondo1->setPos(0, 0);
            fondo2->setPos(anchoFondo, 0);
        }

        if (escena && !escena->views().isEmpty() && jugador) {
            QGraphicsView* vista = escena->views().first();
            vista->centerOn(jugador);
        }

        qDebug() << "Nivel reiniciado correctamente";
    }
    else if (key == Qt::Key_Escape) {
        qDebug() << "Volver al menu...";
    }
}
