#-------------------------------------------------
#
# Project created by QtCreator 2017-05-30T12:29:58
#
#-------------------------------------------------

QT       += core gui sql axcontainer

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = DanfossGraphGenerator
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += main.cpp\
        mainwindow.cpp \
    graph.cpp \
    qcustomplot.cpp \
    fileloader.cpp \
    graphwidget.cpp \
    datacalculator.cpp \
    infomessage.cpp \
    pptxwidget.cpp

HEADERS  += mainwindow.h \
    graph.h \
    qcustomplot.h \
    fileloader.h \
    graphwidget.h \
    datacalculator.h \
    infomessage.h \
    pptxwidget.h

FORMS    += mainwindow.ui \
    graphwidget.ui \
    infomessage.ui \
    pptxwidget.ui

RESOURCES += \
    resources.qrc

TRANSLATIONS += dgg_ru.ts \
                dgg_en.ts

LIBS    += -lole32

CONFIG += c++14

#RC_FILE = danfoss.rc

#CODECFORSRC     = UTF-8

RC_FILE+= \
    danfossgg.rc

DISTFILES +=
