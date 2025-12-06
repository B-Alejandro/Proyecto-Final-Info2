// nivel2.cpp - VERSION CORREDOR INFINITO CON SISTEMA TRIPLE

#include "nivel2.h"
#include "juego.h"
#include "jugador.h"
#include "enemigo.h"
#include "Obstaculo.h"
#include "persona.h"
#include "fisica.h"
#include "GameOverScreen.h"
#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QFontDatabase>
#include <QGraphicsDropShadowEffect>

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
    suelo1(nullptr),
    suelo2(nullptr),
    suelo3(nullptr),
    pantallaGameOver(nullptr),
    juegoEnPausa(false),
    fondo1(nullptr),
    fondo2(nullptr),
    fondo3(nullptr),
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
    int anchoEscena = vistaAncho * 4;
    int altoEscena = vistaAlto;

    crearEscena(anchoEscena, altoEscena);

    // Conecta la señal (declarada en nivel2.h) con el slot
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
// IMPLEMENTACIÓN NUEVA
// ====================================================================

// Implementación del método público para que juego.cpp pueda verificar el estado
bool Nivel2::estaEnGameOver() const
{
    return juegoEnPausa;
}

// ====================================================================
// GESTIÓN DE FONDO PARALLAX - SISTEMA TRIPLE
// ====================================================================
void Nivel2::crearFondoDinamico()
{
    // ... (resto de crearFondoDinamico sin cambios)
    QPixmap fondoPixmap(":/Recursos/Backgrounds/unnamed (1).jpg");
    int sceneH = this->sceneH;

    if (fondoPixmap.isNull()) {
        qDebug() << "No se pudo cargar la imagen del fondo, creando fondo de respaldo";
        fondoPixmap = QPixmap(vistaAncho * 2, sceneH);
        fondoPixmap.fill(QColor(135, 206, 235));
    } else {
        fondoPixmap = fondoPixmap.scaledToHeight(sceneH, Qt::SmoothTransformation);

        // *** AUMENTAR ANCHO DEL FONDO PARA MAYOR COBERTURA ***
        // Usar 2.5 vistas en lugar de 2 para mejor overlap
        int anchoDeseado = static_cast<int>(vistaAncho * 2.5);

        if (fondoPixmap.width() < anchoDeseado) {
            QPixmap fondoExtendido(anchoDeseado, fondoPixmap.height());
            QPainter painter(&fondoExtendido);
            int veces = (anchoDeseado / fondoPixmap.width()) + 1;
            for (int i = 0; i < veces; i++) {
                painter.drawPixmap(i * fondoPixmap.width(), 0, fondoPixmap);
            }
            painter.end();
            fondoPixmap = fondoExtendido;
        }

        fondoPixmap = fondoPixmap.scaledToHeight(sceneH, Qt::SmoothTransformation);
    }

    anchoFondo = fondoPixmap.width();

    // Limpiar fondos previos
    if (fondo1) {
        escena->removeItem(fondo1);
        delete fondo1;
    }
    if (fondo2) {
        escena->removeItem(fondo2);
        delete fondo2;
    }
    if (fondo3) {
        escena->removeItem(fondo3);
        delete fondo3;
    }

    // Crear tres fondos
    fondo1 = new QGraphicsPixmapItem(fondoPixmap);
    fondo2 = new QGraphicsPixmapItem(fondoPixmap);
    fondo3 = new QGraphicsPixmapItem(fondoPixmap);

    // Posicionar en cascada perfecta
    fondo1->setPos(0, 0);
    fondo2->setPos(anchoFondo, 0);
    fondo3->setPos(anchoFondo * 2, 0);

    fondo1->setZValue(-1000);
    fondo2->setZValue(-1000);
    fondo3->setZValue(-1000);

    if (escena) {
        escena->addItem(fondo1);
        escena->addItem(fondo2);
        escena->addItem(fondo3);
    }

    qDebug() << "Sistema de fondo TRIPLE creado. Ancho de cada fondo:" << anchoFondo;
}

void Nivel2::actualizarFondo(qreal posicionJugador)
{
    Q_UNUSED(posicionJugador); // <-- CORRECCIÓN: Evita el warning de parámetro no utilizado.

    if (!fondo1 || !fondo2 || !fondo3 || anchoFondo <= 0) return;

    qreal factorParallax = 0.3; // 30% de velocidad (el fondo se mueve más lento)
    qreal posicionCamara = jugador->x();

    // 1. Calcular el desplazamiento parallax basado en la posición de la cámara
    qreal desplazamientoCamara = posicionCamara - vistaAncho * 0.5;
    qreal desplazamientoParallax = desplazamientoCamara * (1.0 - factorParallax);

    // 2. Aplicar el desplazamiento parallax a cada fondo
    fondo1->setX(0 - desplazamientoParallax);
    fondo2->setX(anchoFondo - desplazamientoParallax);
    fondo3->setX(anchoFondo * 2 - desplazamientoParallax);

    // 3. Sistema de bucle infinito con reposicionamiento suave
    qreal limiteIzquierdo = posicionCamara - vistaAncho * 1.5;

    // Obtener las posiciones actuales
    qreal pos1 = fondo1->x();
    qreal pos2 = fondo2->x();
    qreal pos3 = fondo3->x();

    // Determinar cuál es el fondo más a la derecha (sin usar qMax encadenado)
    qreal maxPosActual = pos1;
    if (pos2 > maxPosActual) maxPosActual = pos2;
    if (pos3 > maxPosActual) maxPosActual = pos3;

    // Reposicionar cada fondo que sale del límite izquierdo
    if (pos1 + anchoFondo < limiteIzquierdo) {
        qreal nuevaPos = maxPosActual + anchoFondo;
        fondo1->setX(nuevaPos);

        // Recalcular máximo después de mover fondo1
        maxPosActual = nuevaPos;
        if (pos2 > maxPosActual) maxPosActual = pos2;
        if (pos3 > maxPosActual) maxPosActual = pos3;
    }

    if (pos2 + anchoFondo < limiteIzquierdo) {
        qreal nuevaPos = maxPosActual + anchoFondo;
        fondo2->setX(nuevaPos);

        // Recalcular máximo después de mover fondo2
        maxPosActual = nuevaPos;
        if (pos1 > maxPosActual) maxPosActual = pos1;
        if (pos3 > maxPosActual) maxPosActual = pos3;
    }

    if (pos3 + anchoFondo < limiteIzquierdo) {
        qreal nuevaPos = maxPosActual + anchoFondo;
        fondo3->setX(nuevaPos);
    }
}

// ====================================================================
// GESTIÓN DE SUELO - SISTEMA TRIPLE
// ====================================================================

void Nivel2::crearSueloTriple()
{
    // ... (resto de crearSueloTriple sin cambios)
    qreal posYSuelo = sceneH * 0.85;
    qreal alturaSuelo = sceneH * 0.15;

    // *** AUMENTAR ANCHO DEL SUELO PARA MAYOR COBERTURA ***
    anchoSuelo = static_cast<int>(vistaAncho * 2.5);

    // Limpiar suelos previos
    if (suelo1) {
        escena->removeItem(suelo1);
        delete suelo1;
    }
    if (suelo2) {
        escena->removeItem(suelo2);
        delete suelo2;
    }
    if (suelo3) {
        escena->removeItem(suelo3);
        delete suelo3;
    }

    // Crear tres suelos
    suelo1 = new Obstaculo(0, posYSuelo, anchoSuelo, alturaSuelo, Qt::darkGreen);
    suelo1->setTextura(":/Recursos/Backgrounds/tierra.jpg", true);
    suelo1->setZValue(-10);

    suelo2 = new Obstaculo(anchoSuelo, posYSuelo, anchoSuelo, alturaSuelo, Qt::darkGreen);
    suelo2->setTextura(":/Recursos/Backgrounds/tierra.jpg", true);
    suelo2->setZValue(-10);

    suelo3 = new Obstaculo(anchoSuelo * 2, posYSuelo, anchoSuelo, alturaSuelo, Qt::darkGreen);
    suelo3->setTextura(":/Recursos/Backgrounds/tierra.jpg", true);
    suelo3->setZValue(-10);

    if (escena) {
        escena->addItem(suelo1);
        escena->addItem(suelo2);
        escena->addItem(suelo3);
    }

    obstaculos.append(suelo1);
    obstaculos.append(suelo2);
    obstaculos.append(suelo3);

    qDebug() << "Sistema de suelo TRIPLE creado. Ancho de cada suelo:" << anchoSuelo;
}
void Nivel2::actualizarSuelo(qreal posicionJugador)
{
    // ... (resto de actualizarSuelo sin cambios)
    if (!suelo1 || !suelo2 || !suelo3 || anchoSuelo <= 0) return;

    qreal posicionCamara = posicionJugador;

    // 1. El suelo se mueve 1:1 con la cámara (sin parallax)
    qreal desplazamientoCamara = posicionCamara - vistaAncho * 0.5;

    // 2. Aplicar desplazamiento a cada suelo
    suelo1->setX(0 - desplazamientoCamara);
    suelo2->setX(anchoSuelo - desplazamientoCamara);
    suelo3->setX(anchoSuelo * 2 - desplazamientoCamara);

    // 3. Sistema de bucle infinito (mismo principio que el fondo)
    qreal limiteIzquierdo = posicionCamara - vistaAncho * 1.5;

    // Obtener posiciones actuales
    qreal pos1 = suelo1->x();
    qreal pos2 = suelo2->x();
    qreal pos3 = suelo3->x();

    // Determinar posición máxima
    qreal maxPosActual = pos1;
    if (pos2 > maxPosActual) maxPosActual = pos2;
    if (pos3 > maxPosActual) maxPosActual = pos3;

    // Reposicionar suelos que salieron del límite
    if (pos1 + anchoSuelo < limiteIzquierdo) {
        qreal nuevaPos = maxPosActual + anchoSuelo;
        suelo1->setX(nuevaPos);

        maxPosActual = nuevaPos;
        if (pos2 > maxPosActual) maxPosActual = pos2;
        if (pos3 > maxPosActual) maxPosActual = pos3;
    }

    if (pos2 + anchoSuelo < limiteIzquierdo) {
        qreal nuevaPos = maxPosActual + anchoSuelo;
        suelo2->setX(nuevaPos);

        maxPosActual = nuevaPos;
        if (pos1 > maxPosActual) maxPosActual = pos1;
        if (pos3 > maxPosActual) maxPosActual = pos3;
    }

    if (pos3 + anchoSuelo < limiteIzquierdo) {
        qreal nuevaPos = maxPosActual + anchoSuelo;
        suelo3->setX(nuevaPos);
    }
}

// ====================================================================
// CONFIGURACIÓN INICIAL
// ====================================================================

void Nivel2::configurarNivel()
{
    // ... (resto de configurarNivel sin cambios)
    // Inicializar timer de daño
    if (!danoObstaculoTimer.isValid()) {
        danoObstaculoTimer.start();
    }

    // Crear sistemas de fondo y suelo TRIPLE
    crearFondoDinamico();
    crearSueloTriple();

    qreal alturaSuelo = sceneH * 0.85;

    // Crear jugador centrado
    qreal anchoJugador = sceneH * 0.15;
    qreal altoJugador = sceneH * 0.15;

    int sceneW = this->sceneW;

    jugador = new Jugador(anchoJugador, altoJugador, sceneW, sceneH, TipoMovimiento::CON_GRAVEDAD);
    jugador->setVida(100);
    jugador->setZValue(100);
    jugador->setSpeed(4.5);

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
        // Crear HUD
        if (escena && !escena->views().isEmpty()) {

            QGraphicsView* vista = escena->views().first();

            // cargar fuente arcade
            QFontDatabase::addApplicationFont(":/Recursos/Fuentes/arcade.ttf");
            QFont fuenteArcade("arcade", 18);

            hudWidget = new QWidget(vista);
            hudWidget->setGeometry(20, 20, 260, 120);

            // Estilo del panel arcade
            hudWidget->setStyleSheet(
                "background-color: rgba(0, 0, 0, 180);"
                "border: 3px solid rgb(0, 180, 255);"
                "border-radius: 10px;"
                );

            QVBoxLayout* layout = new QVBoxLayout(hudWidget);

            // Labels
            vidaLabel = new QLabel("Vida: 100", hudWidget);
            tiempoLabel = new QLabel("Tiempo: 0", hudWidget);
            scoreLabel = new QLabel("Score: 0", hudWidget);

            // Estilos arcade
            vidaLabel->setFont(fuenteArcade);
            tiempoLabel->setFont(fuenteArcade);
            scoreLabel->setFont(fuenteArcade);

            vidaLabel->setStyleSheet("color: rgb(0,255,255);");
            tiempoLabel->setStyleSheet("color: rgb(0,255,100);");
            scoreLabel->setStyleSheet("color: rgb(255,255,0);");

            // Efecto glow para cada label
            QGraphicsDropShadowEffect* glowVida = new QGraphicsDropShadowEffect();
            glowVida->setBlurRadius(25);
            glowVida->setColor(QColor(0,255,255));
            glowVida->setOffset(0,0);

            QGraphicsDropShadowEffect* glowTiempo = new QGraphicsDropShadowEffect();
            glowTiempo->setBlurRadius(25);
            glowTiempo->setColor(QColor(0,255,100));
            glowTiempo->setOffset(0,0);

            QGraphicsDropShadowEffect* glowScore = new QGraphicsDropShadowEffect();
            glowScore->setBlurRadius(25);
            glowScore->setColor(QColor(255,255,0));
            glowScore->setOffset(0,0);

            vidaLabel->setGraphicsEffect(glowVida);
            tiempoLabel->setGraphicsEffect(glowTiempo);
            scoreLabel->setGraphicsEffect(glowScore);

            layout->addWidget(vidaLabel);
            layout->addWidget(tiempoLabel);
            layout->addWidget(scoreLabel);

            hudWidget->setLayout(layout);
            hudWidget->show();

            tiempoJugado.start();
        }

    }
}


void Nivel2::crearEnemigos()
{
    // ... (resto de crearEnemigos sin cambios)
    if (!jugador) return;

    qreal alturaSuelo = sceneH * 0.85;
    qreal sizeEnemigo = sceneH * 0.15;

    qreal posEnemigoX = jugador->x() - vistaAncho * 0.3;
    qreal posEnemigoY = alturaSuelo - sizeEnemigo;

    int sceneW = this->sceneW;

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

    enemigoAtras->setSpeed(0);
    enemigoAtras->vy = 0.0;
    enemigoAtras->onGround = true;
    enemigoAtras->g = 0.5;

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
    // ... (resto de crearObstaculos sin cambios)
    qDebug() << "=== INICIALIZANDO SISTEMA DE CORREDOR INFINITO ===";

    // El suelo ya fue creado en crearSueloTriple()

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
    // ... (resto de spawnearObstaculoAleatorio sin cambios)
    qreal posYSuelo = sceneH * 0.85;
    qreal alturaJugador = sceneH * 0.15;

    // Variación aleatoria en distancia
    qreal distanciaVariable = QRandomGenerator::global()->bounded(300, 600);
    qreal nuevaPosX = ultimaPosicionSpawn + distanciaVariable;

    // Generación aleatoria de tipo de obstáculo (0-99)
    int tipoObstaculo = QRandomGenerator::global()->bounded(0, 100);

    // 5% chance de GAP (no crear obstáculo)
    if (tipoObstaculo >= 95) {
        ultimaPosicionSpawn = nuevaPosX;
        contadorObstaculos++;
        qDebug() << "GAP generado en posición:" << nuevaPosX;
        return;
    }

    // Determinar altura del obstáculo
    int altoObs;

    if (tipoObstaculo < 40) {
        // 40% - Obstáculo BAJO (35% a 50% de altura del jugador)
        altoObs = QRandomGenerator::global()->bounded(
            static_cast<int>(alturaJugador * 0.35),
            static_cast<int>(alturaJugador * 0.5)
            );
    }
    else if (tipoObstaculo < 80) {
        // 40% - Obstáculo MEDIO (50% a 65% de altura del jugador)
        altoObs = QRandomGenerator::global()->bounded(
            static_cast<int>(alturaJugador * 0.5),
            static_cast<int>(alturaJugador * 0.65)
            );
    }
    else {
        // 15% - Obstáculo ALTO (65% a 80% de altura del jugador)
        altoObs = QRandomGenerator::global()->bounded(
            static_cast<int>(alturaJugador * 0.65),
            static_cast<int>(alturaJugador * 0.80)
            );
    }

    // Ancho proporcional al alto
    int anchoMin = static_cast<int>(altoObs * 0.8);
    int anchoMax = static_cast<int>(altoObs * 1.3);
    int anchoObs = QRandomGenerator::global()->bounded(anchoMin, anchoMax);

    // Posición Y (sobre el suelo)
    qreal posY = posYSuelo - altoObs;

    // Crear obstáculo
    Obstaculo* obs = new Obstaculo(nuevaPosX, posY, anchoObs, altoObs, Qt::transparent);
    obs->setZValue(10);

    // ===============================================================
    // DECISIÓN DE DAÑO Y ASIGNACIÓN DE SPRITE
    // ===============================================================

    // 30% de probabilidad de ser dañino
    bool esDanino = (QRandomGenerator::global()->bounded(0, 100) < 30);

    QString texturaSeleccionada;

    if (esDanino) {
        // ===== OBSTÁCULO DAÑINO =====
        obs->setDanioColision(20);

        // Lista de sprites dañinos
        QStringList spritesDaninos = {
            ":/Recursos/Objects/D_Obstaculo1.png",
            ":/Recursos/Objects/D_Obstaculo2.png",
            ":/Recursos/Objects/D_Obstaculo3.png"
        };

        // Seleccionar sprite aleatorio
        int indice = QRandomGenerator::global()->bounded(spritesDaninos.size());
        texturaSeleccionada = spritesDaninos[indice];

        qDebug() << "Obstáculo DAÑINO creado en" << nuevaPosX
                 << "Daño:" << obs->getDanioColision()
                 << "Sprite:" << texturaSeleccionada;

    } else {
        // ===== OBSTÁCULO SEGURO =====
        obs->setDanioColision(0);

        // Lista de sprites seguros
        QStringList spritesNoDestructivos = {
            ":/Recursos/Objects/Obstaculo1.png",
            ":/Recursos/Objects/Obstaculo2.png",
            ":/Recursos/Objects/ND_Obstaculo3.png"
        };

        // Seleccionar sprite aleatorio
        int indice = QRandomGenerator::global()->bounded(spritesNoDestructivos.size());
        texturaSeleccionada = spritesNoDestructivos[indice];

        qDebug() << "Obstáculo SEGURO creado en" << nuevaPosX
                 << "Sprite:" << texturaSeleccionada;
    }

    // ===============================================================
    // CARGAR Y APLICAR SPRITE
    // ===============================================================

    QPixmap spritePixmap(texturaSeleccionada);

    if (spritePixmap.isNull()) {
        qDebug() << " ERROR: No se pudo cargar sprite:" << texturaSeleccionada;
        qDebug() << "   Usando color de respaldo...";

        // Color de respaldo según tipo
        if (esDanino) {
            obs->setColor(QColor(180, 0, 0)); // Rojo oscuro
        } else {
            obs->setColor(QColor(100, 100, 100)); // Gris
        }
    } else {
        // Aplicar sprite (false = escalar para ajustar, no repetir)
        obs->setTextura(spritePixmap, false);
        qDebug() << "✓ Sprite cargado correctamente:" << texturaSeleccionada;
    }

    // Agregar a la escena y lista
    if (escena) escena->addItem(obs);
    obstaculos.append(obs);

    ultimaPosicionSpawn = nuevaPosX;
    contadorObstaculos++;
}


void Nivel2::limpiarObstaculosLejanos()
{
    // ... (resto de limpiarObstaculosLejanos sin cambios)
    if (!jugador) return;

    qreal limiteEliminacion = jugador->x() - vistaAncho * 1.5;

    int eliminados = 0;
    for (int i = 0; i < obstaculos.size(); ) {
        Obstaculo* obs = obstaculos[i];

        // NO eliminar los suelos base
        if (obs == suelo1 || obs == suelo2 || obs == suelo3) {
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
    // ... (resto de reposicionarEscena sin cambios)
    if (!jugador || !escena) return;

    qreal centroEscena = sceneW * 0.5;
    qreal distanciaDelCentro = std::abs(jugador->x() - centroEscena);

    if (distanciaDelCentro > vistaAncho * 1.2) {

        qreal desplazamiento = jugador->x() - centroEscena;

        qDebug() << "=== REPOSICIONANDO ESCENA ==="
                 << "Desplazamiento:" << desplazamiento;

        QList<QGraphicsItem*> items = escena->items();

        for (QGraphicsItem* item : items) {
            // Excluir fondos y suelos (se manejan con sus propios sistemas)
            if (item == fondo1 || item == fondo2 || item == fondo3) continue;
            if (item == suelo1 || item == suelo2 || item == suelo3) continue;

            item->setX(item->x() - desplazamiento);
        }

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
    // Si el juego está en pausa (ej. después de morir), no hacer nada
    if (juegoEnPausa) {
        return;
    }

    // 1. Ejecutar física base (movimiento del jugador, gravedad, si aplica)
    NivelBase::actualizar();

    if (!jugador || !escena || escena->views().isEmpty()) {
        return;
    }

    // 2. Forzar movimiento automático (Corredor Infinito)
    Jugador* j_cast = dynamic_cast<Jugador*>(jugador);
    if (j_cast && j_cast->estaVivo()) {
        j_cast->rightPressed = true;
        j_cast->leftPressed = false;
    }

    QGraphicsView* vista = escena->views().first();
    qreal alturaSuelo = sceneH * 0.85;


    // 3. Sistemas de bucle infinito (Parallax y Suelo)
    actualizarFondo(jugador->x());
    actualizarSuelo(jugador->x());

    // 4. Mantenimiento de la Escena (reposicionamiento)
    reposicionarEscena();

    // 5. Lógica del Enemigo (activación y movimiento)
    if (enemigoAtras && jugador->estaVivo()) {
        // Corrección de altura del enemigo (asegurar que esté en el suelo)
        qreal alturaEnemigo = enemigoAtras->boundingRect().height();
        qreal posYCorrecta = alturaSuelo - alturaEnemigo;

        if (enemigoAtras->y() > posYCorrecta + 2) {
            enemigoAtras->setY(posYCorrecta);
            enemigoAtras->vy = 0;
            enemigoAtras->onGround = true;
        }

        // Activación del enemigo si el jugador se movió lo suficiente
        static qreal ultimaPosicionJugador = jugador->x();
        bool jugadorSeMovio = (jugador->x() != ultimaPosicionJugador);
        ultimaPosicionJugador = jugador->x();

        if (jugadorSeMovio && !enemigoAtras->estaActivo()) {
            enemigoAtras->activarPersecucion();
        }
    }


    // 6. Generación continua de obstáculos y Limpieza
    qreal limiteGeneracion = jugador->x() + vistaAncho * 1.2;

    while (ultimaPosicionSpawn < limiteGeneracion) {
        spawnearObstaculoAleatorio();
    }

    limpiarObstaculosLejanos();

    // --------------------------------------------------------------------
    // 7. DETECCIÓN DE COLISIONES CON OBSTÁCULOS DAÑINOS
    // --------------------------------------------------------------------
    if (jugador && jugador->estaVivo()) {
        QRectF rectJugador = jugador->sceneBoundingRect();

        for (Obstaculo* obs : obstaculos) {
            // Saltar suelos (no hacen daño)
            if (obs == suelo1 || obs == suelo2 || obs == suelo3) {
                continue;
            }

            QRectF rectObstaculo = obs->sceneBoundingRect();
            int dano = obs->getDanioColision();

            // ⚠️ DEPURACIÓN: Chequeo de colisión
            if (rectJugador.intersects(rectObstaculo)) {

                if (dano > 0) { // Solo procesar obstáculos con daño

                    qDebug() << "🚨 INTERSECCIÓN DETECTADA con obstáculo! Daño:" << dano
                             << "Cooldown transcurrido (ms):" << danoObstaculoTimer.elapsed();

                    // Aplicar daño con cooldown (cada 400ms)
                    if (danoObstaculoTimer.elapsed() > 400) {

                        jugador->recibirDanio(dano); // <-- APLICACIÓN DEL DAÑO

                        qDebug() << "✅ DAÑO APLICADO. Vida restante:" << jugador->getVida();

                        // Efecto de retroceso (knockback)
                        qreal pushBack = -40.0;
                        jugador->setX(jugador->x() + pushBack);

                        // Asegurar que el jugador no caiga del suelo
                        jugador->setY(alturaSuelo - jugador->boundingRect().height());

                        // Reiniciar el timer solo si el jugador sobrevive
                        if (jugador->estaVivo()) {
                            danoObstaculoTimer.restart();
                        }
                    }
                }

                // --------------------------------------------------------------------
                // VERIFICACIÓN DE MUERTE DEL JUGADOR
                // --------------------------------------------------------------------
                if (!jugador->estaVivo()) {
                    qDebug() << "💀 Jugador murió por obstáculo dañino. Iniciando Game Over.";
                    Jugador* j = dynamic_cast<Jugador*>(jugador);
                    if (j) j->activarAnimacionMuerte();
                    if (enemigoAtras) enemigoAtras->activarAnimacionMuerte();

                    juegoEnPausa = true;

                    // Esperar 900ms para la animación de muerte antes de terminar el juego
                    QTimer::singleShot(900, this, [this]() {
                        emit juegoTerminado(); // <-- CORRECCIÓN: Emitir la señal
                    });
                    return; // Salir de actualizar()
                }

                break; // Solo procesar una colisión de daño por frame
            }
        }
    }

    // --------------------------------------------------------------------
    // 8. Colisión Jugador-Enemigo
    // --------------------------------------------------------------------
    if (enemigoAtras && jugador && jugador->estaVivo()) {
        QRectF rectJugador = jugador->sceneBoundingRect();
        QRectF rectEnemigo = enemigoAtras->sceneBoundingRect();

        static QElapsedTimer danoTimerEnemigo; // Timer para colisión de enemigo
        if (!danoTimerEnemigo.isValid()) {
            danoTimerEnemigo.start();
        }

        if (rectJugador.intersects(rectEnemigo)) {
            qDebug() << "🚨 INTERSECCIÓN DETECTADA con ENEMIGO. Cooldown (ms):" << danoTimerEnemigo.elapsed();

            if (danoTimerEnemigo.elapsed() > 300) {
                jugador->recibirDanio(50);
                danoTimerEnemigo.restart();
                qDebug() << "✅ DAÑO ENEMIGO APLICADO. Vida restante:" << jugador->getVida();

                // Knockback y ajuste de altura... (lógica de retroceso)
                qreal push = 30.0;
                if (jugador->x() < enemigoAtras->x()) {
                    jugador->setX(jugador->x() - push);
                    enemigoAtras->setX(enemigoAtras->x() + push * 0.5);
                } else {
                    jugador->setX(jugador->x() + push);
                    enemigoAtras->setX(enemigoAtras->x() - push * 0.5);
                }
                jugador->setY(alturaSuelo - jugador->boundingRect().height());
                enemigoAtras->setY(alturaSuelo - enemigoAtras->boundingRect().height());
            }

            // Verificación de muerte (por enemigo)
            if (!jugador->estaVivo()) {
                qDebug() << "💀 Jugador murió por ENEMIGO. Iniciando Game Over.";
                Jugador* j = dynamic_cast<Jugador*>(jugador);
                if (j) j->activarAnimacionMuerte();
                if (enemigoAtras) enemigoAtras->activarAnimacionMuerte();

                juegoEnPausa = true;

                QTimer::singleShot(900, this, [this]() {
                    emit juegoTerminado(); // <-- CORRECCIÓN: Emitir la señal
                });
                return;
            }
        }
    }


    // 9. Movimiento de cámara (centrar en el jugador)
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
    // ... (resto de manejarTecla sin cambios)
    if (!juegoEnPausa) return;

    if (key == Qt::Key_R) {

        qreal alturaSuelo = sceneH * 0.85;

        // Limpiar obstáculos (excepto suelos)
        for (int i = 0; i < obstaculos.size(); ) {
            Obstaculo* obs = obstaculos[i];
            if (obs != suelo1 && obs != suelo2 && obs != suelo3) {
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

        // Reiniciar suelos TRIPLE
        if (suelo1 && suelo2 && suelo3) {
            suelo1->setX(0);
            suelo2->setX(anchoSuelo);
            suelo3->setX(anchoSuelo * 2);
        }

        // Reiniciar fondos TRIPLE
        if (fondo1 && fondo2 && fondo3) {
            fondo1->setPos(0, 0);
            fondo2->setPos(anchoFondo, 0);
            fondo3->setPos(anchoFondo * 2, 0);
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

        qDebug() << "Nivel reiniciado con sistema TRIPLE";
    }
    else if (key == Qt::Key_Escape) {
        qDebug() << "Volver al menú...";
    }
}
void Jugador::saltar()
{
    // CRÍTICO: Solo permite saltar si el personaje está en el suelo (onGround == true).
    if (onGround) {
        vy = -10;             // Setea la velocidad vertical para subir (valor negativo)
        onGround = false;     // El jugador ya no está en el suelo
        setAnimacion(EstadoAnimacion::SALTANDO);
        qDebug() << "SALTO DISPARADO por llamada externa!";
    } else {
        // opcional: qDebug() << "Intento de salto, pero no está en el suelo.";
    }
}
