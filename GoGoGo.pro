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
    gtpengineprocess.cpp

HEADERS  += mainwindow.h \
    goboard.h \
    gtpengineprocess.h

FORMS    += mainwindow.ui
