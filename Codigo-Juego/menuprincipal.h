#ifndef MENUPRINCIPAL_H
#define MENUPRINCIPAL_H

#include <QGraphicsScene>
#include <QGraphicsProxyWidget>
#include <QGraphicsOpacityEffect>
#include <QTimer>
#include <QPushButton>
#include <QMediaPlayer>
#include <QAudioOutput>

class MenuPrincipal : public QGraphicsScene
{
    Q_OBJECT

public:
     QMediaPlayer* musicaMenu;
    MenuPrincipal(int ancho, int alto, QObject* parent = nullptr);

    void iniciarTransicionEntrada();
    void iniciarMusicaMenu();

    // 🔥 MÉTODO PÚBLICO para detener la música
    void detenerMusica();

    // ✅ CONSTANTES PÚBLICAS para la transición
    static const int INTERVALO_MS = 16;
    static const int DURACION_TRANSICION_MS = 1000;

signals:
    void iniciarNivel(int idNivel);
    void salirJuego();

private slots:
    void actualizarOpacidad();
    void onIniciarNivelClicked(int idNivel);
    void onSalirClicked();
    void onToggleMusicaClicked();

private:
    void configurarFondo();
    void crearBotonMusica(int anchoVista, int altoVista);
    QPushButton* crearBotonCaotico(const QString& texto, QWidget* contenedor,
                                   const char* slot, int idNivel, bool esBotonSalir = false);

    // Elementos de la interfaz
    QGraphicsProxyWidget* proxyPanel;
    QGraphicsOpacityEffect* opacityEffect;
    QTimer* transicionTimer;
    qreal opacidadActual;

    // Botón de música
    QPushButton* botonMusica;

    // 🔥 AHORA SON PRIVADOS pero con método público de acceso

    QAudioOutput* audioOutput;
};

#endif // MENUPRINCIPAL_H
