// nivel2.cpp - VERSION CORREDOR INFINITO CON FONDO Y SUELO MEJORADOS

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

// ====================================================================
// CONSTRUCTOR
// ====================================================================
Nivel2::Nivel2(Juego* juego, QObject* parent)
    : NivelBase(juego, 2, parent),
    enemigoAtras(nullptr),
    suelo(nullptr),
    suelo2(nullptr),
    pantallaGameOver(nullptr),
    juegoEnPausa(false),
    fondo1(nullptr),
    fondo2(nullptr),
    anchoFondo(0),
    anchoSuelo(0),
    vistaAncho(0),
    vistaAlto(0),
    distanciaEntreObstaculos(400),
    ultimaPosicionSpawn(0),
    contadorObstaculos(0)
{
    if (juego) {
        vistaAncho = juego->getVistaAncho();
        vistaAlto = juego->getVistaAlto();
    }

    // Escena pequeña y reposicionable
    int anchoEscena = vistaAncho * 3;
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

// ====================================================================
// GESTIÓN DE FONDO PARALLAX - SISTEMA DE DOS FONDOS
// ====================================================================

void Nivel2::crearFondoDinamico()
{
    QPixmap fondoPixmap(":/Recursos/Backgrounds/unnamed (1).jpg");

    if (fondoPixmap.isNull()) {
        qDebug() << "No se pudo cargar la imagen del fondo, creando fondo de respaldo";
        fondoPixmap = QPixmap(vistaAncho * 3, sceneH);
        fondoPixmap.fill(QColor(135, 206, 235));
    } else {
        // Escalar el fondo a la altura de la escena
        fondoPixmap = fondoPixmap.scaledToHeight(sceneH, Qt::SmoothTransformation);

        // *** ANCHO DEL FONDO: 3 vistas para cubrir mejor ***
        int anchoDeseado = vistaAncho * 3;
        if (fondoPixmap.width() < anchoDeseado) {
            // Repetir horizontalmente
            QPixmap fondoExtendido(anchoDeseado, fondoPixmap.height());
            QPainter painter(&fondoExtendido);
            int veces = (anchoDeseado / fondoPixmap.width()) + 1;
            for (int i = 0; i < veces; i++) {
                painter.drawPixmap(i * fondoPixmap.width(), 0, fondoPixmap);
            }
            painter.end();
            fondoPixmap = fondoExtendido;
        }
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

    // Posicionar fondos: uno en el centro, otro adelante
    fondo1->setPos(0, 0);
    fondo2->setPos(anchoFondo, 0);

    fondo1->setZValue(-1000);
    fondo2->setZValue(-1000);

    if (escena) {
        escena->addItem(fondo1);
        escena->addItem(fondo2);
    }

    qDebug() << "Sistema de fondo dual creado. Ancho de cada fondo:" << anchoFondo;
}

void Nivel2::actualizarFondo(qreal posicionJugador)
{
    if (!fondo1 || !fondo2 || anchoFondo <= 0) return;

    // Factor parallax: el fondo se mueve más lento (30% de la velocidad del jugador)
    qreal factorParallax = 0.3;
    qreal posicionFondo = posicionJugador * factorParallax;

    // *** SISTEMA MEJORADO: Asegurar cobertura completa ***
    qreal inicioCamara = posicionJugador - (vistaAncho * 0.6);
    qreal finCamara = posicionJugador + (vistaAncho * 0.6);

    // Calcular base con módulo para movimiento suave
    qreal offset = std::fmod(posicionFondo, anchoFondo);
    if (offset < 0) offset += anchoFondo;

    qreal baseX = posicionFondo - offset;

    // Posicionar fondos
    fondo1->setX(baseX);
    fondo2->setX(baseX + anchoFondo);

    // Verificar cobertura y reposicionar si es necesario
    if (fondo2->x() + anchoFondo < finCamara) {
        fondo1->setX(fondo2->x() + anchoFondo);
    }
    if (fondo1->x() + anchoFondo < finCamara) {
        fondo2->setX(fondo1->x() + anchoFondo);
    }

    // Asegurar que haya fondo atrás también
    if (fondo1->x() > inicioCamara && fondo2->x() > inicioCamara) {
        if (fondo1->x() < fondo2->x()) {
            fondo1->setX(fondo2->x() - anchoFondo);
        } else {
            fondo2->setX(fondo1->x() - anchoFondo);
        }
    }
}

// ====================================================================
// GESTIÓN DE SUELO - SISTEMA DE DOS SUELOS
// ====================================================================

void Nivel2::crearSueloDual()
{
    qreal posYSuelo = sceneH * 0.85;
    qreal alturaSuelo = sceneH * 0.15;

    // *** ANCHO DEL SUELO: 3 vistas para cubrir bien y evitar huecos ***
    anchoSuelo = vistaAncho * 3;

    // Limpiar suelos previos si existen
    if (suelo) {
        escena->removeItem(suelo);
        delete suelo;
        suelo = nullptr;
    }
    if (suelo2) {
        escena->removeItem(suelo2);
        delete suelo2;
        suelo2 = nullptr;
    }

    // Crear dos suelos en secuencia
    suelo = new Obstaculo(0, posYSuelo, anchoSuelo, alturaSuelo, Qt::darkGreen);
    suelo->setTextura(":/Recursos/Backgrounds/tierra.jpg", true);
    suelo->setZValue(-10);

    suelo2 = new Obstaculo(anchoSuelo, posYSuelo, anchoSuelo, alturaSuelo, Qt::darkGreen);
    suelo2->setTextura(":/Recursos/Backgrounds/tierra.jpg", true);
    suelo2->setZValue(-10);

    if (escena) {
        escena->addItem(suelo);
        escena->addItem(suelo2);
    }

    // Agregar a la lista de obstáculos para colisiones
    obstaculos.append(suelo);
    obstaculos.append(suelo2);

    qDebug() << "Sistema de suelo dual creado. Ancho de cada suelo:" << anchoSuelo;
}

void Nivel2::actualizarSuelo(qreal posicionJugador)
{
    if (!suelo || !suelo2 || anchoSuelo <= 0) return;

    // *** SISTEMA MEJORADO: Mantener siempre suelo adelante y atrás ***
    // Calcular dónde deberían estar los suelos basándose en la cámara
    qreal inicioCamara = posicionJugador - (vistaAncho * 0.6);

    // Usar módulo para crear bucle continuo
    qreal offset = std::fmod(inicioCamara, anchoSuelo);
    if (offset < 0) offset += anchoSuelo;

    // Base alineada con la cámara
    qreal baseX = inicioCamara - offset;

    // Posicionar suelos en cascada
    suelo->setX(baseX);
    suelo2->setX(baseX + anchoSuelo);

    // *** SISTEMA DE TRIPLE VERIFICACIÓN ***
    // Asegurar que siempre haya suelo adelante
    qreal finCamara = posicionJugador + (vistaAncho * 0.6);

    // Si suelo2 no llega al final de la cámara, mover suelo1 adelante
    if (suelo2->x() + anchoSuelo < finCamara) {
        suelo->setX(suelo2->x() + anchoSuelo);
    }

    // Si suelo1 no llega al final de la cámara, mover suelo2 adelante
    if (suelo->x() + anchoSuelo < finCamara) {
        suelo2->setX(suelo->x() + anchoSuelo);
    }

    // Verificación final: asegurar cobertura completa
    if (suelo->x() > inicioCamara && suelo2->x() > inicioCamara) {
        // Ambos están adelante, mover el de más atrás
        if (suelo->x() < suelo2->x()) {
            suelo->setX(suelo2->x() - anchoSuelo);
        } else {
            suelo2->setX(suelo->x() - anchoSuelo);
        }
    }
}

// ====================================================================
// CONFIGURACIÓN INICIAL
// ====================================================================

void Nivel2::configurarNivel()
{
    // Crear sistemas de fondo y suelo
    crearFondoDinamico();
    crearSueloDual();

    qreal alturaSuelo = sceneH * 0.85;

    // Crear jugador centrado
    qreal anchoJugador = sceneH * 0.15;
    qreal altoJugador = sceneH * 0.15;

    jugador = new Jugador(anchoJugador, altoJugador, sceneW, sceneH, TipoMovimiento::CON_GRAVEDAD);
    jugador->setVida(100);
    jugador->setZValue(100);

    // *** AUMENTAR VELOCIDAD DEL JUGADOR ***
    jugador->setSpeed(5.5);  // Velocidad aumentada de 5.0 a 7.5

    if (escena) escena->addItem(jugador);

    Jugador* j = dynamic_cast<Jugador*>(jugador);
    if (j)
    {
        j->cargarSpritesnivel2();

        int altoReal = static_cast<int>(j->boundingRect().height());

        qreal posJugadorX = vistaAncho * 0.5;
        qreal posJugadorY = alturaSuelo - altoReal;

        jugador->setPos(posJugadorX, posJugadorY);

        qDebug() << "Jugador posicionado en" << posJugadorX << posJugadorY;
    }

    if (escena) {
        pantallaGameOver = new GameOverScreen(escena, this);
    }

    if (escena && !escena->views().isEmpty() && jugador) {
        QGraphicsView* vista = escena->views().first();

        vista->setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
        vista->setRenderHint(QPainter::Antialiasing, false);
        vista->setRenderHint(QPainter::SmoothPixmapTransform, true);

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

    qreal posEnemigoX = jugador->x() - vistaAncho * 0.3;
    qreal posEnemigoY = alturaSuelo - sizeEnemigo;

    enemigoAtras = new Enemigo(sizeEnemigo,
                               sizeEnemigo,
                               sceneW,
                               sceneH,
                               TipoMovimiento::CON_GRAVEDAD,
                               2);

    enemigoAtras->cargarSprites();

    int altoRealEnemigo = static_cast<int>(enemigoAtras->boundingRect().height());
    posEnemigoY = alturaSuelo - altoRealEnemigo;
    enemigoAtras->setPos(posEnemigoX, posEnemigoY);

    enemigoAtras->setVida(100);
    enemigoAtras->setZValue(100);
    enemigoAtras->setDireccionIzquierda(false);

    // *** ENEMIGO INICIA INACTIVO (velocidad 0) ***
    enemigoAtras->setSpeed(0);
    enemigoAtras->vy = 0.0;
    enemigoAtras->onGround = true;
    enemigoAtras->g = 0.4;

    enemigoAtras->leftPressed = false;
    enemigoAtras->rightPressed = false;
    enemigoAtras->upPressed = false;
    enemigoAtras->downPressed = false;

    enemigoAtras->setAnimacion(EstadoAnimacion::IDLE);

    if (escena) {
        escena->addItem(enemigoAtras);
    }
    enemigos.append(enemigoAtras);

    qDebug() << "Enemigo creado en posición" << posEnemigoX << posEnemigoY;
}

// ====================================================================
// GESTIÓN DE OBSTÁCULOS - CORREDOR INFINITO
// ====================================================================

void Nivel2::crearObstaculos()
{
    qDebug() << "=== INICIALIZANDO SISTEMA DE CORREDOR INFINITO ===";

    // El suelo ya fue creado en crearSueloDual()

    // Inicializar spawning
    ultimaPosicionSpawn = (jugador ? jugador->x() : 0) + vistaAncho * 0.3;

    // Generar obstáculos iniciales
    int obstaculosIniciales = 8;
    for (int i = 0; i < obstaculosIniciales; i++) {
        spawnearObstaculoAleatorio();
    }

    qDebug() << "Sistema infinito inicializado con" << obstaculosIniciales << "obstáculos";
}

void Nivel2::spawnearObstaculoAleatorio()
{
    qreal posYSuelo = sceneH * 0.85;
    qreal alturaJugador = sceneH * 0.15;

    // Variación aleatoria en distancia (más espaciado)
    qreal distanciaVariable = QRandomGenerator::global()->bounded(300, 600);

    qreal nuevaPosX = ultimaPosicionSpawn + distanciaVariable;

    // Generación aleatoria de tipo de obstáculo
    int tipoObstaculo = QRandomGenerator::global()->bounded(0, 100);

    int altoObs, anchoObs;

    if (tipoObstaculo < 50) {
        // 50% - Obstáculo MUY BAJO (fácil de saltar)
        altoObs = QRandomGenerator::global()->bounded(
            static_cast<int>(alturaJugador * 0.2),
            static_cast<int>(alturaJugador * 0.35)
            );
    }
    else if (tipoObstaculo < 80) {
        // 30% - Obstáculo BAJO-MEDIO
        altoObs = QRandomGenerator::global()->bounded(
            static_cast<int>(alturaJugador * 0.35),
            static_cast<int>(alturaJugador * 0.5)
            );
    }
    else if (tipoObstaculo < 95) {
        // 15% - Obstáculo MEDIO
        altoObs = QRandomGenerator::global()->bounded(
            static_cast<int>(alturaJugador * 0.5),
            static_cast<int>(alturaJugador * 0.65)
            );
    }
    else {
        // 5% - GAP (sin obstáculo)
        ultimaPosicionSpawn = nuevaPosX;
        contadorObstaculos++;
        return;
    }

    // Ancho proporcional (más delgados)
    int anchoMin = static_cast<int>(altoObs * 0.8);
    int anchoMax = static_cast<int>(altoObs * 1.3);
    anchoObs = QRandomGenerator::global()->bounded(anchoMin, anchoMax);

    qreal posY = posYSuelo - altoObs;

    Obstaculo* obs = new Obstaculo(nuevaPosX, posY, anchoObs, altoObs, Qt::darkGray);
    obs->setZValue(10);

    QStringList texturas = {
        ":/Recursos/Objects/Obstaculos2.png"
    };

    if (!texturas.isEmpty()) {
        QString textura = texturas[QRandomGenerator::global()->bounded(texturas.size())];
        obs->setTextura(textura, false);
    }

    if (escena) escena->addItem(obs);
    obstaculos.append(obs);

    ultimaPosicionSpawn = nuevaPosX;
    contadorObstaculos++;
}

void Nivel2::limpiarObstaculosLejanos()
{
    if (!jugador) return;

    qreal limiteEliminacion = jugador->x() - vistaAncho * 1.5;

    int eliminados = 0;
    for (int i = 0; i < obstaculos.size(); ) {
        Obstaculo* obs = obstaculos[i];

        // NO eliminar los suelos base
        if (obs == suelo || obs == suelo2) {
            ++i;
            continue;
        }

        if (obs->x() + obs->boundingRect().width() < limiteEliminacion) {
            escena->removeItem(obs);
            delete obs;
            obstaculos.removeAt(i);
            eliminados++;
        } else {
            ++i;
        }
    }

    if (eliminados > 0) {
        qDebug() << "Eliminados" << eliminados << "obstáculos fuera de vista";
    }
}

// ====================================================================
// REPOSICIONAMIENTO DE ESCENA
// ====================================================================

void Nivel2::reposicionarEscena()
{
    if (!jugador || !escena) return;

    qreal centroEscena = sceneW * 0.5;
    qreal distanciaDelCentro = std::abs(jugador->x() - centroEscena);

    // Reposicionar si está muy lejos del centro
    if (distanciaDelCentro > vistaAncho * 1.0) {

        qreal desplazamiento = jugador->x() - centroEscena;

        qDebug() << "=== REPOSICIONANDO ESCENA ==="
                 << "Desplazamiento:" << desplazamiento;

        QList<QGraphicsItem*> items = escena->items();

        for (QGraphicsItem* item : items) {
            // Excluir fondos (se manejan aparte)
            if (item == fondo1 || item == fondo2) continue;

            // Excluir suelos (se manejan aparte)
            if (item == suelo || item == suelo2) continue;

            item->setX(item->x() - desplazamiento);
        }

        // Ajustar suelos manualmente
        if (suelo) suelo->setX(suelo->x() - desplazamiento);
        if (suelo2) suelo2->setX(suelo2->x() - desplazamiento);

        // Ajustar variable de control
        ultimaPosicionSpawn -= desplazamiento;

        qDebug() << "Escena reposicionada";
    }
}

// ====================================================================
// LOOP PRINCIPAL DE ACTUALIZACIÓN
// ====================================================================

void Nivel2::actualizar()
{
    if (juegoEnPausa) {
        return;
    }

    NivelBase::actualizar();

    if (!jugador || !escena || escena->views().isEmpty()) {
        return;
    }

    // Forzar movimiento automático
    Jugador* j_cast = dynamic_cast<Jugador*>(jugador);
    if (j_cast && j_cast->estaVivo()) {
        j_cast->rightPressed = true;
        j_cast->leftPressed = false;
    }

    QGraphicsView* vista = escena->views().first();

    // *** ACTUALIZAR FONDO Y SUELO CON LA POSICIÓN DEL JUGADOR ***
    actualizarFondo(jugador->x());
    actualizarSuelo(jugador->x());

    // Reposicionar escena si es necesario
    reposicionarEscena();

    // Activar enemigo
    static qreal ultimaPosicionJugador = jugador->x();
    bool jugadorSeMovio = (jugador->x() != ultimaPosicionJugador);
    ultimaPosicionJugador = jugador->x();

    if (jugadorSeMovio && enemigoAtras && !enemigoAtras->estaActivo()) {
        enemigoAtras->activarPersecucion();
    }

    // Generación continua de obstáculos
    qreal limiteGeneracion = jugador->x() + vistaAncho * 1.2;

    while (ultimaPosicionSpawn < limiteGeneracion) {
        spawnearObstaculoAleatorio();
    }

    // Limpieza
    limpiarObstaculosLejanos();

    // Corrección enemigo
    if (enemigoAtras && jugador->estaVivo()) {
        qreal alturaSuelo = sceneH * 0.85;
        qreal alturaEnemigo = enemigoAtras->boundingRect().height();
        qreal posYCorrecta = alturaSuelo - alturaEnemigo;

        if (enemigoAtras->y() > posYCorrecta + 2) {
            enemigoAtras->setY(posYCorrecta);
            enemigoAtras->vy = 0;
            enemigoAtras->onGround = true;
        }
    }

    // Colisión jugador-enemigo
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

                qreal push = 30.0;

                if (jugador->x() < enemigoAtras->x()) {
                    jugador->setX(jugador->x() - push);
                    enemigoAtras->setX(enemigoAtras->x() + push * 0.5);
                } else {
                    jugador->setX(jugador->x() + push);
                    enemigoAtras->setX(enemigoAtras->x() - push * 0.5);
                }

                qreal alturaSuelo2 = sceneH * 0.85;
                jugador->setY(alturaSuelo2 - jugador->boundingRect().height());
                enemigoAtras->setY(alturaSuelo2 - enemigoAtras->boundingRect().height());
            }

            if (!jugador->estaVivo()) {
                Jugador* j = dynamic_cast<Jugador*>(jugador);
                if (j) j->activarAnimacionMuerte();
                if (enemigoAtras) enemigoAtras->activarAnimacionMuerte();

                juegoEnPausa = true;

                QTimer::singleShot(900, this, [this]() {
                    emit juegoTerminado();
                });
                return;
            }
        }
    }

    // Movimiento de cámara
    qreal camaraX = jugador->x();
    vista->centerOn(camaraX, sceneH * 0.5);
}

// ====================================================================
// GAME OVER Y REINICIO
// ====================================================================

void Nivel2::onJuegoTerminado()
{
    qDebug() << "onJuegoTerminado llamado";

    if (pantallaGameOver) {
        pantallaGameOver->mostrar();
    }
}

void Nivel2::manejarTecla(Qt::Key key)
{
    if (!juegoEnPausa) return;

    if (key == Qt::Key_R) {

        qreal alturaSuelo = sceneH * 0.85;

        // Limpiar obstáculos (excepto suelos)
        for (int i = 0; i < obstaculos.size(); ) {
            Obstaculo* obs = obstaculos[i];
            if (obs != suelo && obs != suelo2) {
                escena->removeItem(obs);
                delete obs;
                obstaculos.removeAt(i);
            } else {
                ++i;
            }
        }

        // Reiniciar estados
        if (juego) juego->iniciar();
        if (pantallaGameOver) pantallaGameOver->ocultar();
        juegoEnPausa = false;

        // Reiniciar Jugador
        if (jugador) {
            Jugador* j = dynamic_cast<Jugador*>(jugador);
            if (j) {
                j->reanudarAnimacion();
                j->setAnimacion(EstadoAnimacion::IDLE);
                j->setVida(100);
            }
            qreal posJugadorX = vistaAncho * 0.5;
            qreal posJugadorY = alturaSuelo - jugador->boundingRect().height();
            jugador->setPos(posJugadorX, posJugadorY);
        }

        // Reiniciar Enemigo
        if (enemigoAtras) {
            enemigoAtras->reanudarAnimacion();
            enemigoAtras->setVida(100);
            enemigoAtras->setSpeed(0);
            enemigoAtras->setAnimacion(EstadoAnimacion::IDLE);
            enemigoAtras->setDireccionIzquierda(false);

            qreal sizeEnemigo = enemigoAtras->boundingRect().height();
            qreal posEnemigoX = jugador->x() - vistaAncho * 0.3;
            qreal posEnemigoY = alturaSuelo - sizeEnemigo;

            enemigoAtras->setPos(posEnemigoX, posEnemigoY);
        }

        // Reiniciar suelos
        if (suelo && suelo2) {
            suelo->setX(0);
            suelo2->setX(anchoSuelo);
        }

        // Reiniciar fondos
        if (fondo1 && fondo2) {
            fondo1->setPos(0, 0);
            fondo2->setPos(anchoFondo, 0);
        }

        // Reiniciar spawning
        ultimaPosicionSpawn = (jugador ? jugador->x() : 0) + vistaAncho * 0.3;
        contadorObstaculos = 0;

        // Regenerar obstáculos
        for (int i = 0; i < 8; i++) {
            spawnearObstaculoAleatorio();
        }

        // Re-centrar vista
        if (escena && !escena->views().isEmpty() && jugador) {
            QGraphicsView* vista = escena->views().first();
            vista->centerOn(jugador);
        }

        qDebug() << "Nivel reiniciado completamente";
    }
    else if (key == Qt::Key_Escape) {
        qDebug() << "Volver al menú...";
    }
}
