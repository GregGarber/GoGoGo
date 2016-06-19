#-------------------------------------------------
#
# Project created by QtCreator 2016-06-10T08:22:20
#
#-------------------------------------------------
# g++ -c -pipe -std=c++14 -g -std=gnu++0x -Wall -W -D_REENTRANT -fPIC -DQT_QML_DEBUG -DQT_WIDGETS_LIB -DQT_GUI_LIB -DQT_CORE_LIB -I../GoGoGo -I. -I/usr/local/Qt-5.6.0/include -I/usr/local/Qt-5.6.0/include/QtWidgets -I/usr/local/Qt-5.6.0/include/QtGui -I/usr/local/Qt-5.6.0/include/QtCore -I. -I. -I/usr/local/Qt-5.6.0/mkspecs/linux-g++ -o goboard.o ../GoGoGo/goboard.cpp
QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = GoGoGo
TEMPLATE = app
#QMAKE_CXXFLAGS = -std=gnu++14
CONFIG += c++14
QMAKE_CXXFLAGS += -std=c++1y
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
