#-------------------------------------------------
#
# Project created by QtCreator 2016-06-10T08:22:20
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = GoGoGo
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    goboard.cpp \
    gtpengineprocess.cpp \
    settings.cpp

HEADERS  += mainwindow.h \
    goboard.h \
    gtpengineprocess.h \
    settings.h

FORMS    += mainwindow.ui \
    settings.ui

RESOURCES += \
    resources.qrc

DISTFILES += \
    stylesheet.css
