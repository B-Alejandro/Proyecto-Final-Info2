#include <QApplication>
#include "Juego.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    Juego juego;
    juego.iniciar();

    return app.exec();
}
