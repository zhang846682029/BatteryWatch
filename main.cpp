#include "mainwindow.h"
#include "datahandler.h"
#include <QApplication>
#include <QSqlDatabase>
#include <QTranslator>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator translate;
    translate.load(":/battery_cn.qm");
    a.installTranslator(&translate);

    MainWindow w;
    w.show();

    return a.exec();
}
