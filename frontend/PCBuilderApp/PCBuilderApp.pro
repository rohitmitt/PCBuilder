QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# Add the appropriate include path based on the PCBuilder folder location

SOURCES += \
    ../../graph.cpp \
    initdialog.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    ../../graph.h \
    initdialog.h \
    mainwindow.h

FORMS += \
    initdialog.ui \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc

QT += concurrent
QT += core gui network

DISTFILES += \
    assets/placeholder.jpg \
    assets/switch.png \
    assets/switchBFS.svg \
    assets/switchDFS.svg

LIBS += -L"C:/Program Files/OpenSSL-Win64/lib" \
         "C:/Program Files/OpenSSL-Win64/lib/libssl.lib" \
         "C:/Program Files/OpenSSL-Win64/lib/libcrypto.lib"
INCLUDEPATH += "C:/Program Files/OpenSSL-Win64/include"



