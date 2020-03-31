#include "qchartviewbattery.h"
#include <QDateTime>
#include <QLineSeries>
#include <QSqlQuery>
#include <QDebug>

QChartViewBattery::QChartViewBattery(QWidget *parent)
    :QChartView(parent)
{
    setRenderHint(QPainter::Antialiasing);
}

void QChartViewBattery::getRecordAndShow(int battery, int property, QDate &begin, QDate &end)
{
    QMap<int,QString> map_property;
    map_property[1]=QString("rate");
    map_property[2]=QString("voltage");
    map_property[3]=QString("current");
    map_property[4]=QString("volume");
    map_property[5]=QString("temp");
    map_property[6]=QString("count");
//    map_property[7]=QString("Connect");

    QSqlQuery query;
    QString sql = QString("SELECT clientId,%1,recordTime FROM batteryPowerInfo WHERE clientId=%2 "
                          "AND recordTime BETWEEN strftime('%Y-%m-%d','%3','start of day') "
                          "AND strftime('%Y-%m-%d','%4','1 day','start of day')")
            .arg(map_property[property]).arg(battery)
            .arg(begin.toString("yyyy-MM-dd")).arg(end.toString("yyyy-MM-dd"));
    bool r1=query.exec(sql);
    qDebug()<<__FUNCTION__<<r1<<sql;

    QMap<float,float> record;
    while(query.next()){
        QDateTime datetime = query.value(2).toDateTime();
        float datetime_tt = datetime.toMSecsSinceEpoch();
        int value = query.value(1).toInt();
        float value_tt=value;
        switch (property) {
        case 1: // rate
            value_tt = value_tt/1.0;
            break;
        case 2: // voltage
            value_tt = value_tt/100.0;
            break;
        case 3: // current
            value_tt = value_tt/100.0;
            break;
        case 4: // volume
            value_tt = value_tt/10.0;
            break;
        case 5: // temp
            value_tt = value_tt/10.0;
            break;
        case 6: // temp
            value_tt = value_tt/10.0;
            break;
        default:
            break;
        }
        record[datetime_tt]=value_tt;
    }

    update_chart(property,record,begin,end);
}

void QChartViewBattery::update_chart(int type, QMap<float,float> &record, QDate &begin, QDate &end)
{
    QLineSeries *series_tt = new QLineSeries();
    QMapIterator<float,float> iter(record);
    while(iter.hasNext()){
        iter.next();
        series_tt->append(iter.key(),iter.value());
//        qDebug()<<__FUNCTION__<<iter.key()<<iter.value();
    }

    QChart *chart = new QChart();
    chart->addSeries(series_tt);
    chart->legend()->hide();
    if(type==1){
        chart->setTitle(tr("Rate"));
    } else if(type==2){
        chart->setTitle(tr("Voltage"));
    } else if(type==4){
        chart->setTitle(tr("Volume"));
    } else if(type==3){
        chart->setTitle(tr("Current"));
    } else if(type==5){
        chart->setTitle(tr("Temperature"));
    }

    QDateTimeAxis *axisX = new QDateTimeAxis;
    axisX->setTickCount(10);
    axisX->setFormat("hh:mm:ss");
    axisX->setTitleText(tr("Time"));
    QDateTime time_begin2;
    time_begin2.setDate(begin);
    time_begin2.setTime(QTime::fromString("00:00:00","hh:mm:ss"));
    QDateTime time_end2;
    time_end2.setDate(end);
    time_end2.setTime(QTime::fromString("23:59:59","hh:mm:ss"));
    qDebug()<<__FUNCTION__<<time_begin2.toString("yyyy-MM-dd hh:mm:ss")
              <<time_end2.toString("yyyy-MM-dd hh:mm:ss");
    axisX->setRange(time_begin2,time_end2);
    chart->addAxis(axisX, Qt::AlignBottom);
    series_tt->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis;
    axisY->setTitleText(tr("Value"));
    axisY->setLabelFormat("%i");
    if(type==1){ // rate
        axisY->setRange(0.0,100.0);
    } else if(type==2){ // voltage
        axisY->setRange(0.0,50.0);
    } else if(type==4){ // volume
        axisY->setRange(0.0,80.0);
    } else if(type==3){ // current
        axisY->setRange(0.0,30.0);
    } else if(type==5){ // volume
        axisY->setRange(-10.0,80.0);
    }
    chart->addAxis(axisY, Qt::AlignLeft);
    series_tt->attachAxis(axisY);

    QChart *chart_prev=this->chart();
    delete chart_prev;
    chart_prev=NULL;
    setChart(chart);
}
