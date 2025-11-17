QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17
CONFIG += qt

SOURCES += \
    Obstaculo.cpp \
    enemigo.cpp \
    fisica.cpp \
    juego.cpp \
    jugador.cpp \
    main.cpp \
    nivel.cpp \
    nivel1.cpp \
    nivel2.cpp \
    nivel3.cpp \
    persona.cpp

HEADERS += \
    Obstaculo.h \
    enemigo.h \
    fisica.h \
    juego.h \
    jugador.h \
    nivel.h \
    nivel1.h \
    nivel2.h \
    nivel3.h \
    persona.h

FORMS +=

qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    Recursos.qrc
