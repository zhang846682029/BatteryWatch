#include "mainwindow.h"
#include "datahandler.h"
#include <QApplication>
#include <QSqlDatabase>
#include <QTranslator>
#include <QDebug>


int main(int argc, char *argv[])
{
    QDate date_t;
    date_t.setDate(2020,6,6);
    QDate date_f=QDate::currentDate();
    int day_valid=date_f.daysTo(date_t);
//    qDebug()<<date_t.toString()<<"day_valid"<<day_valid;
    if(day_valid<=0){
        qDebug()<<"out of date";
//        return 0;
    }

    QApplication a(argc, argv);

    QTranslator translate;
    translate.load(":/battery_cn.qm");
    a.installTranslator(&translate);

    MainWindow w;
    w.show();

    return a.exec();
}
