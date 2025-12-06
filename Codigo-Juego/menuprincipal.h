#ifndef MENUPRINCIPAL_H
#define MENUPRINCIPAL_H

#include <QGraphicsScene>
#include <QPushButton>
#include <QGraphicsProxyWidget>
#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QGraphicsOpacityEffect>
#include <QTimer>
// QComboBox eliminado

// Incluimos QMediaPlayer para la música
#include <QMediaPlayer>
#include <QAudioOutput>

class MenuPrincipal : public QGraphicsScene
{
    Q_OBJECT

public:
    MenuPrincipal(int ancho, int alto, QObject* parent = nullptr);
    void iniciarTransicionEntrada();
    void iniciarMusicaMenu();

signals:
    void iniciarNivel(int idNivel);
    void salirJuego();

private slots:
    void onIniciarNivelClicked(int idNivel);
    void onSalirClicked()  { emit salirJuego(); }
    void onToggleMusicaClicked();
    void actualizarOpacidad();

private:
    void configurarFondo();
    // FIX 1: DECLARACIÓN FALTANTE
    void crearBotonMusica(int anchoVista, int altoVista);
    QPushButton* crearBotonCaotico(const QString& texto, QWidget* contenedor, const char* slot, int idNivel, bool esBotonSalir = false);

private:
    QGraphicsProxyWidget* proxyPanel;
    QGraphicsOpacityEffect* opacityEffect;
    QTimer* transicionTimer;
    qreal opacidadActual;

    QPushButton* botonMusica;
    QMediaPlayer* musicaMenu;
    QAudioOutput* audioOutput;

    const int INTERVALO_MS = 16;
    const int DURACION_TRANSICION_MS = 800;
};

#endif // MENUPRINCIPAL_H
