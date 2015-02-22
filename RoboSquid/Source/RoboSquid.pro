#-------------------------------------------------
#
# Project created by QtCreator 2014-04-10T13:19:06
#
#-------------------------------------------------

QT       += core gui sql


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport serialport webkitwidgets

TARGET = RoboSquid
TEMPLATE = app

INCLUDEPATH += $$PWD/includes
INCLUDEPATH += $$PWD/glm-0.9.5.4/glm/

SOURCES += main.cpp\
        mainwindow.cpp \
        parser.cpp \
        settings.cpp \
    masterthread.cpp \
    qcustomplot.cpp

HEADERS  += mainwindow.h \
         parser.h \
         settings.h \
    masterthread.h \
    ui_mainwindow.h \
    qcustomplot.h \
    rotation.h

RESOURCES +=
