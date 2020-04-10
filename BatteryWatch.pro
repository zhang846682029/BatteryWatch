#-------------------------------------------------
#
# Project created by QtCreator 2020-02-06T13:45:02
#
#-------------------------------------------------

QT += core gui
QT += network
QT += sql
QT += charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = BatteryWatch
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    batteryinfowidget.cpp \
    server.cpp \
    tcpclientsocket.cpp \
    datahandler.cpp \
    dialoggroupeditor.cpp \
    editordatahandler.cpp \
    dialogwarningsertting.cpp \
    warndatahandler.cpp \
    warninghandler.cpp \
    dialogwarningmore.cpp \
    dialogrecordchart.cpp \
    qchartviewbattery.cpp

HEADERS  += mainwindow.h \
    batteryinfowidget.h \
    server.h \
    tcpclientsocket.h \
    datahandler.h \
    dialoggroupeditor.h \
    editordatahandler.h \
    dialogwarningsertting.h \
    warndatahandler.h \
    warninghandler.h \
    dialogwarningmore.h \
    dialogrecordchart.h \
    qchartviewbattery.h

FORMS += \
    mainwindow.ui \
    dialoggroupeditor.ui \
    dialogwarningsertting.ui \
    dialogwarningmore.ui \
    dialogrecordchart.ui

RESOURCES += \
    ui-resource.qrc

RC_ICONS = battery.ico
TRANSLATIONS += battery_cn.ts

