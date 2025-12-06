#ifndef PANELINFO_H
#define PANELINFO_H

#include <QGraphicsRectItem>
#include <QObject>
#include <QMap>
#include <QGraphicsProxyWidget>
#include <QPushButton>

// Declaraciones adelantadas
class QString;
class QGraphicsTextItem;
class QWidget;

/*
 * Clase PanelInfo (HUD)
 * Ahora hereda de QObject Y QGraphicsRectItem.
 */
class PanelInfo : public QObject, public QGraphicsRectItem
{
    Q_OBJECT

public:
    // Enumeración para las acciones del menú de pausa
    enum class AccionPausa {
        ReiniciarNivel,
        VolverMenu,
        Reanudar
    };
    Q_ENUM(AccionPausa)  // 🔥 CRÍTICO: Registrar el enum en el sistema de meta-objetos de Qt

    // Constructor que acepta el ancho de la escena
    PanelInfo(qreal sceneWidth, QGraphicsItem *parent = nullptr);
    ~PanelInfo() override;

    void actualizar(const QMap<QString, QString>& dataset);

    /*
     * Muestra/Oculta el menú de pausa (opciones).
     */
    void setMenuPausaVisible(bool visible);

    /*
     * Establece la visibilidad del botón de pausa del HUD.
     */
    void setBotonPausaVisible(bool visible);

signals:
    /*
     * SEÑAL: Emitida cuando se hace clic en el botón de pausa principal.
     */
    void pausaPresionada();

    /*
     * SEÑAL: Emitida cuando se selecciona una opción del menú de pausa.
     */
    void accionPausaSeleccionada(AccionPausa accion);

private:
    QGraphicsTextItem* textoHUD;
    qreal anchoEscena;

    // --- ELEMENTOS DE PAUSA ---
    QGraphicsProxyWidget* proxyBotonPausa;
    QPushButton* botonPausa;

    QGraphicsRectItem* fondoPausa;
    QGraphicsProxyWidget* contenedorMenu;

    void configurarEstilo();
    void inicializarMenuPausa();
    void inicializarBotonPausa();

    /*
     * Ranuras privadas para manejar los clics.
     */
    void manejarBotonPausaClic();
    void manejarBotonMenuClic();
};

#endif // PANELINFO_H
