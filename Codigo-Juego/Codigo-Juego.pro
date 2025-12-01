QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17
CONFIG += qt

SOURCES += \
    Obstaculo.cpp \
    enemigo.cpp \
    explosion.cpp \
    fisica.cpp \
    gameoverscreen.cpp \
    hudnivel1.cpp \
    juego.cpp \
    jugador.cpp \
    main.cpp \
    nivel1.cpp \
    nivel2.cpp \
    nivel3.cpp \
    nivelbase.cpp \
    persona.cpp \
    proyectil.cpp \
    proyectiltanque.cpp \
    tanque.cpp \
    victoriascreen.cpp

HEADERS += \
    Obstaculo.h \
    enemigo.h \
    explosion.h \
    fisica.h \
    gameoverscreen.h \
    hudnivel1.h \
    juego.h \
    jugador.h \
    nivel1.h \
    nivel2.h \
    nivel3.h \
    nivelbase.h \
    persona.h \
    proyectil.h \
    proyectiltanque.h \
    tanque.h \
    victoriascreen.h

FORMS +=

qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    Recursos.qrc

DISTFILES +=
