#-------------------------------------------------
#
# Project created by QtCreator 2020-02-06T13:45:02
#
#-------------------------------------------------

QT       += core gui
QT += network
QT += sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = BatteryWatch
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    batteryinfowidget.cpp \
    server.cpp \
    tcpclientsocket.cpp \
    datahandler.cpp

HEADERS  += mainwindow.h \
    batteryinfowidget.h \
    server.h \
    tcpclientsocket.h \
    datahandler.h

FORMS += \
    mainwindow.ui
