#-------------------------------------------------
#
# Project created by QtCreator 2019-04-12T08:37:03
#
#-------------------------------------------------

QT       += core gui
QT += network
QT += serialport

RC_ICONS = myico.ico

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ESS-Robot
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    EssCom.cpp \
    DataCenter.cpp \
    TcpServer.cpp \
    SuperDog.cpp

HEADERS += \
        mainwindow.h \
    EssCom.h \
    DataCenter.h \
    Defines.h \
    TcpServer.h \
    dog_api.h \
    SuperDog.h

FORMS += \
        mainwindow.ui


win32: LIBS += -L$$PWD/./ -ldog_windows_3149894

INCLUDEPATH += $$PWD/.
DEPENDPATH += $$PWD/.

RESOURCES += \
    ess-robot.qrc

RC_ICONS =myico.ico
