#include "dialogrecordchart.h"
#include "ui_dialogrecordchart.h"
#include "qchartviewbattery.h"
#include <QSqlQuery>
#include <QDebug>

DialogRecordChart::DialogRecordChart(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogRecordChart)
{
    ui->setupUi(this);
    Qt::WindowFlags flag = Qt::Dialog;
    flag |= Qt::WindowMinimizeButtonHint;
    flag |= Qt::WindowMaximizeButtonHint;
    flag |= Qt::WindowCloseButtonHint;
    setWindowFlags(flag);

    QDate date_tt = QDate::currentDate();
    ui->dateEditBegin->setDate(date_tt);
    ui->dateEditEnd->setDate(date_tt.addDays(1));

    initProperty();
    initBattery();
    on_btnQuery_clicked(true);
}

DialogRecordChart::~DialogRecordChart()
{
    delete ui;
}

void DialogRecordChart::initProperty()
{
    ui->combxProperty->insertItem(0,tr("rate"));
    ui->combxProperty->insertItem(1,tr("voltage"));
    ui->combxProperty->insertItem(2,tr("current"));
    ui->combxProperty->insertItem(3,tr("volume"));
    ui->combxProperty->insertItem(4,tr("temperature"));
//    ui->combxProperty->insertItem(5,tr("life"));
    //    ui->combxProperty->insertItem(6,tr("connect"));
}

void DialogRecordChart::initBattery()
{
    QSqlQuery query;
    QString sql = QString("SELECT clientId FROM batteryDetailInfo;");
    bool r1=query.exec(sql);
    qDebug()<<r1<<sql;

    while(query.next())
    {
        int battery = query.value(0).toInt();
        ui->combxBattery->addItem(QString::number(battery));
    }
}

void DialogRecordChart::on_btnQuery_clicked(bool)
{
    int battery=ui->combxBattery->currentText().toInt();
    int property=ui->combxProperty->currentIndex()+1;
    QDate date_begin = ui->dateEditBegin->date();
    QDate date_end = ui->dateEditEnd->date();

    QChartViewBattery *chart_view=ui->chartView;
    chart_view->getRecordAndShow(battery,property,date_begin,date_end);
}
