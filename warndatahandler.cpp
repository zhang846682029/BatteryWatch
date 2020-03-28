#include "warndatahandler.h"
#include <QSqlQuery>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QDebug>

WarnDataHandler::WarnDataHandler(QObject *parent) : QObject(parent)
{

}

void WarnDataHandler::slotPropertySelected(int index)
{
//    int id = mPropertys[name];
    int id = index+1;

    QString sql = QString("SELECT * FROM eventInfo WHERE property=%1").arg(id);
    QSqlQuery query;
    bool r1=query.exec(sql);
    qDebug()<<r1<<sql;

    if(query.next()){
        int event_property = query.value(0).toInt();
        QString event_desc = query.value(1).toString();
        float event_max = query.value(2).toFloat();
        bool event_max_enable = query.value(3).toBool();
        float event_min = query.value(4).toFloat();
        bool event_min_enable = query.value(5).toBool();
        int event_count = query.value(6).toInt();
        bool event_enable = query.value(7).toBool();

        QJsonObject device_info_map;
        device_info_map["property"]=event_property;
        device_info_map["desc"]=event_desc;
        device_info_map["max"]=event_max;
        device_info_map["maxEnable"]=event_max_enable;
        device_info_map["min"]=event_min;
        device_info_map["minEnable"]=event_min_enable;
        device_info_map["count"]=event_count;
        device_info_map["enable"]=event_enable;
        QJsonDocument doc(device_info_map);
        QByteArray array=doc.toJson();
        emit sigPropertySelected(array);
    } else {
        QJsonObject device_info_map;
        device_info_map["property"]=id;
        device_info_map["desc"]=QString();
        device_info_map["max"]=100.0;
        device_info_map["maxEnable"]=false;
        device_info_map["min"]=0.0;
        device_info_map["minEnable"]=false;
        device_info_map["count"]=0;
        device_info_map["enable"]=false;
        QJsonDocument doc(device_info_map);
        QByteArray array=doc.toJson();
        emit sigPropertySelected(array);
    }
}

void WarnDataHandler::slotPropertyModified(QByteArray array)
{
    QJsonParseError errorLoad;
    QJsonDocument doc = QJsonDocument::fromJson(array,&errorLoad);
    if(QJsonParseError::NoError!=errorLoad.error){
        return;
    }

    QJsonObject obj=doc.object();
    int event_property = obj["property"].toInt();
//    QString event_property_string = obj["propertyName"].toString();
//    int event_property = mPropertys[event_property_string];
    QString event_desc = obj["desc"].toString();
    float event_max = obj["max"].toDouble();
    bool event_max_enable = obj["maxEnable"].toBool();
    float event_min = obj["min"].toDouble();
    bool event_min_enable = obj["minEnable"].toBool();
    int event_count = obj["count"].toInt();
    bool event_enable = obj["enable"].toBool();

    QSqlQuery query;
    QString sql = QString("SELECT * FROM eventInfo WHERE property=%1;").arg(event_property);
    bool r1=query.exec(sql);
    qDebug()<<r1<<sql;
    if(!query.next()){
        QString sql = QString("INSERT INTO eventInfo "
                              "(property,event_desc,event_max,event_max_enable,"
                              "event_min,event_min_enable,event_count,event_enable) "
                              "VALUES (%1,'%2',%3,%4,%5,%6,%7,%8);")
                .arg(event_property).arg(event_desc).arg(event_max)
                .arg(event_max_enable).arg(event_min).arg(event_min_enable)
                .arg(event_count).arg(event_enable);
        QSqlQuery query;
        bool r1=query.exec(sql);
        qDebug()<<r1<<sql;
    } else {

        QString sql = QString("UPDATE eventInfo SET event_desc='%1',event_max=%2,event_max_enable=%3,"
                              "event_min=%4,event_min_enable=%5,event_count=%6,event_enable=%7 "
                              "WHERE property=%8;")
                .arg(event_desc).arg(event_max).arg(event_max_enable)
                .arg(event_min).arg(event_min_enable).arg(event_count)
                .arg(event_enable).arg(event_property);
        QSqlQuery query;
        bool r1=query.exec(sql);
        qDebug()<<r1<<sql;
    }
}
