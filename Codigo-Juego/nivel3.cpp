// ============ nivel3.cpp - COMPLETO Y CORREGIDO (Estética Urbana) ============
#include "nivel3.h"
#include "juego.h"
#include "jugador.h"
#include "enemigointeligente.h"
#include "coleccionable.h"
#include "panelinfo.h"
#include "victoriascreen.h"
#include <QDebug>
#include <QTimer>
#include <QGraphicsRectItem>
#include <QMap>
#include <QGraphicsDropShadowEffect>

Nivel3::Nivel3(Juego* juego, QObject* parent)
    : NivelBase(juego, 3, parent)
    , coleccionablesRecolectados(0)
    , totalColeccionables(7)
    , nivelGanado(false)
    , infoPanel(nullptr)
    , puntuacionActual(0)
    , victoriaScreen(nullptr)
{
    int ancho = juego->getVistaAncho();
    int alto = juego->getVistaAlto();

    crearEscena(ancho, alto);

    qDebug() << "\n╔════════════════════════════════════════════════════╗";
    qDebug() << "║  NIVEL 3: EL LABERINTO Y LA CAZA (ESTÉTICA URBANA)  ║";
    qDebug() << "╚════════════════════════════════════════════════════╝\n";
}

void Nivel3::configurarNivel()
{
    qDebug() << "⚙️  Configurando nivel...";

    // FONDO GRIS OSCURO (Asfalto/Calle)
    if (escena) {
        escena->setBackgroundBrush(QBrush(QColor(40, 40, 40)));
        qDebug() << "✓ Fondo cambiado a gris oscuro (Estética urbana).";
    }

    // Crear jugador
    qreal posJugadorX = sceneW * 0.05;
    qreal posJugadorY = sceneH * 0.5;  // Posición central vertical

    crearJugador(posJugadorX, posJugadorY, TipoMovimiento::RECTILINEO);

    if (jugador) {
        qreal escala = 0.5;
        jugador->setScale(escala);
        jugador->setSpeed(6.0);
        jugador->setVida(100);

        // Cargar sprites de Nivel 3
        jugador->cargarSpritesNivel3();

        qDebug() << "✓ Jugador creado (TAMAÑO REDUCIDO 50%) con sprites de Nivel 3";
    }

    // CREACIÓN DE HUD Y PANTALLA DE VICTORIA
    infoPanel = new PanelInfo(juego->getVistaAncho());
    if (escena) {
        escena->addItem(infoPanel);
        infoPanel->setZValue(100);
    }
    infoPanel->setBotonPausaVisible(false);

    if (escena) {
        victoriaScreen = new VictoriaScreen(escena, this);
        qDebug() << "✓ Pantalla de Victoria creada.";
    }

    // Primera actualización del HUD
    if (jugador && infoPanel) {
        QMap<QString, QString> hudData;
        hudData["VIDA"] = QString::number(jugador->getVida());
        hudData["PUNTAJE"] = QString::number(puntuacionActual);
        hudData["COLECCIONABLES"] = QString("%1/%2")
                                        .arg(coleccionablesRecolectados)
                                        .arg(totalColeccionables);
        infoPanel->actualizar(hudData);
    }
}

void Nivel3::crearEnemigos()
{
    qDebug() << "\n🤖 Creando Múltiples Enemigos Inteligentes...";

    qreal sizeEnemigo = sceneH * 0.04;
    qreal radioDeteccion = 180.0;  // Aumentado ligeramente para compensar
    int numEnemigos = 4;

    // *** POSICIONES ESTRATÉGICAS ALEJADAS DE MUROS ***
    QList<QPointF> posiciones = {
        {sceneW * 0.35, sceneH * 0.15},  // Superior centro-izquierda (alejado del muro superior)
        {sceneW * 0.65, sceneH * 0.2},   // Superior centro-derecha (alejado de muros)
        {sceneW * 0.5, sceneH * 0.6},    // Centro (zona abierta)
        {sceneW * 0.8, sceneH * 0.45}    // Derecha centro (alejado del muro derecho)
    };

    for (int i = 0; i < numEnemigos; ++i) {
        EnemigoInteligente* enemigo = new EnemigoInteligente(
            sizeEnemigo,
            sizeEnemigo,
            sceneW,
            sceneH,
            TipoMovimiento::RECTILINEO,
            3,
            radioDeteccion
            );

        enemigo->setPos(posiciones[i]);
        enemigo->setVida(30);
        enemigo->setSpeed(0);  // Mantener estáticos (torretas)
        enemigo->mostrarAreaDeteccion(true);  // *** MOSTRAR área de detección ***
        enemigo->setBrush(QBrush(QColor(255, 50, 50)));

        if (escena) escena->addItem(enemigo);
        enemigos.append(enemigo);

        qDebug() << "✓ Enemigo " << i + 1 << " creado (TORRETA):"
                 << " Posición:" << enemigo->pos()
                 << " Radio detección:" << radioDeteccion;
    }
    qDebug() << "";
}

void Nivel3::crearMuro(qreal x, qreal y, qreal w, qreal h)
{
    QGraphicsRectItem* muro = new QGraphicsRectItem(x, y, w, h);
    muro->setBrush(QBrush(QColor(80, 80, 80)));
    muro->setPen(QPen(QColor(100, 100, 100), 2));

    QGraphicsDropShadowEffect* shadow = new QGraphicsDropShadowEffect();
    shadow->setBlurRadius(10);
    shadow->setColor(QColor(0, 0, 0, 150));
    shadow->setOffset(5, 5);
    muro->setGraphicsEffect(shadow);

    if (escena) escena->addItem(muro);
}

void Nivel3::crearDecoraciones()
{
    qDebug() << "✓ Decoraciones de fondo preparadas.";
}

void Nivel3::crearObstaculos()
{
    qDebug() << "🧱 Creando laberinto y coleccionables...";

    // --- 1. Crear Laberinto (Muros) - DISEÑO OPTIMIZADO ---
    qreal wallW = sceneW * 0.02;
    qreal wallH = sceneH * 0.02;

    // Muro superior horizontal (completo)
    crearMuro(sceneW * 0.1, sceneH * 0.1, sceneW * 0.8, wallH);

    // Muro vertical izquierdo (altura moderada)
    crearMuro(sceneW * 0.25, sceneH * 0.1, wallW, sceneH * 0.5);

    // *** Muro vertical derecho MÁS CORTO para permitir acceso superior ***
    crearMuro(sceneW * 0.75, sceneH * 0.25, wallW, sceneH * 0.3);

    // Muro inferior horizontal (con gap en el centro)
    crearMuro(sceneW * 0.1, sceneH * 0.85, sceneW * 0.3, wallH);
    crearMuro(sceneW * 0.6, sceneH * 0.85, sceneW * 0.3, wallH);

    // Muro central horizontal (para crear pasillo)
    crearMuro(sceneW * 0.35, sceneH * 0.5, sceneW * 0.3, wallH);

    qDebug() << "✓ Laberinto creado con múltiples pasillos.";

    // --- 2. Crear Coleccionables (Total 7) - DISTRIBUIDOS ESTRATÉGICAMENTE ---
    qreal colSize = 25.0;
    QColor colorColeccionable = QColor(255, 200, 0);

    QList<QPointF> posicionesCol = {
        // Zona superior
        {sceneW * 0.15, sceneH * 0.15},   // Superior izquierda (arriba del muro)
        {sceneW * 0.85, sceneH * 0.15},   // Superior derecha (accesible por arriba)

        // Zona media
        {sceneW * 0.15, sceneH * 0.4},    // Medio izquierda (entre muros)
        {sceneW * 0.5, sceneH * 0.35},    // Centro superior (zona abierta)
        {sceneW * 0.85, sceneH * 0.4},    // Medio derecha (accesible por el gap)

        // Zona inferior
        {sceneW * 0.15, sceneH * 0.7},    // Inferior izquierda
        {sceneW * 0.85, sceneH * 0.7}     // Inferior derecha
    };

    int i = 0;
    for (const QPointF& pos : posicionesCol) {
        Coleccionable* col = new Coleccionable(pos.x(), pos.y(), colSize, i % 3);
        col->setFillColor(colorColeccionable);

        escena->addItem(col);
        listaColeccionables.append(col);
        connect(col, &Coleccionable::coleccionableRecolectado,
                this, &Nivel3::onColeccionableRecolectado);
        i++;
    }

    qDebug() << "✓ " << totalColeccionables << " coleccionables distribuidos estratégicamente.";
    qDebug() << "✓ Rutas de acceso:";
    qDebug() << "   - Pasillo superior completo";
    qDebug() << "   - Gap en muro derecho para acceso lateral";
    qDebug() << "   - Gap central en muro inferior";
    qDebug() << "   - Todos los coleccionables son 100% accesibles.\n";
}

// ════════════════════════════════════════════════════════════
// ACTUALIZACIÓN POR FRAME
// ════════════════════════════════════════════════════════════
void Nivel3::actualizar()
{
    // Si el nivel está ganado o la pantalla de victoria está visible, detener la lógica del juego
    if (nivelGanado || (victoriaScreen && victoriaScreen->estaVisible())) return;

    NivelBase::actualizar();

    if (escena) {
        escena->advance();
    }

    // Verificar colisiones con coleccionables
    for (Coleccionable* col : listaColeccionables) {
        if (col && !col->fueRecolectado()) {
            col->verificarColision();
        }
    }

    // Actualizar el panel en cada frame
    if (jugador && infoPanel) {
        QMap<QString, QString> hudData;
        hudData["VIDA"] = QString::number(jugador->getVida());
        hudData["PUNTAJE"] = QString::number(puntuacionActual);
        hudData["COLECCIONABLES"] = QString("%1/%2")
                                        .arg(coleccionablesRecolectados)
                                        .arg(totalColeccionables);
        infoPanel->actualizar(hudData);
    }
}

// ════════════════════════════════════════════════════════════
// 🎯 SISTEMA DE COLECCIONABLES Y VICTORIA
// ════════════════════════════════════════════════════════════

void Nivel3::onColeccionableRecolectado(Coleccionable* col)
{
    if (!col) return;

    coleccionablesRecolectados++;
    puntuacionActual += 100;

    qDebug() << "╔════════════════════════════════════════════════════╗";
    qDebug() << "║             ✨ Coleccionable Recolectado!          ║";
    qDebug() << "╚════════════════════════════════════════════════════╝";
    qDebug() << "   📊 Progreso:" << coleccionablesRecolectados << "/" << totalColeccionables;

    listaColeccionables.removeOne(col);

    if (escena) {
        escena->removeItem(col);
    }

    col->deleteLater();

    if (coleccionablesRecolectados >= totalColeccionables) {
        nivelGanado = true;
        mostrarVictoria();
    } else {
        qDebug() << "   💡 Faltan:" << (totalColeccionables - coleccionablesRecolectados);
        qDebug() << "";
    }
}

void Nivel3::mostrarVictoria()
{
    if (victoriaScreen) {
        victoriaScreen->mostrar(puntuacionActual);
    }

    qDebug() << "\n";
    qDebug() << "╔════════════════════════════════════════════════════╗";
    qDebug() << "║                                                    ║";
    qDebug() << "║          ★ ★ ★  ¡VICTORIA!  ★ ★ ★                ║";
    qDebug() << "║                                                    ║";
    qDebug() << "║  ¡Has recolectado todos los objetos!               ║";
    qDebug() << "║                                                    ║";
    qDebug() << "║  🎉 ¡NIVEL COMPLETADO! 🎉                          ║";
    qDebug() << "║                                                    ║";
    qDebug() << "╚════════════════════════════════════════════════════╝";
}
