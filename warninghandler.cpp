#include "warninghandler.h"
#include <QSqlQuery>
#include <QDateTime>
#include <QStandardItemModel>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

WarningHandler::WarningHandler(QObject *parent) : QObject(parent)
{

    mMaxSeconds = 0;
    initEventInfo();

   startTimer(5*60*1000);
//    startTimer(10*1000); // test

    mModelWarningInfo = new QStandardItemModel(this);
    update_warning_model();
    mModelWarningTaskInfo = new QStandardItemModel(this);
    updateWarningTaskModel();
}

void WarningHandler::timerEvent(QTimerEvent *event)
{
    get_recent_powerinfo();
//    updateWarningTaskModel();
}

void WarningHandler::initEventInfo()
{
    int time_count_max=0;

    QSqlQuery query;
    QString sql = QString("SELECT * FROM eventInfo");
    bool r1 = query.exec(sql);
    qDebug()<<r1<<sql;

    while(query.next()){
        EventInfo info;
        info.property = query.value(0).toInt();
        info.event_desc = query.value(1).toString();
        info.event_max = query.value(2).toFloat();
        info.event_max_enable = query.value(3).toBool();
        info.event_min = query.value(4).toFloat();
        info.event_min_enable = query.value(5).toBool();
        info.event_count = query.value(6).toInt(); // time count*60 seconds
        info.event_enable = query.value(7).toBool();

        mEventDesc[info.property]=info.event_desc;
        switch (info.property) {
        case 1: // rate
            memcpy(&mEventInfoRate,&info,sizeof(EventInfo));
            break;
        case 2: // voltage
            memcpy(&mEventInfoVoltage,&info,sizeof(EventInfo));
            break;
        case 3: // current
            memcpy(&mEventInfoCurrent,&info,sizeof(EventInfo));
            break;
        case 4: // volume
            memcpy(&mEventInfoVolume,&info,sizeof(EventInfo));
            break;
        case 5: // temp
            memcpy(&mEventInfoTemp,&info,sizeof(EventInfo));
            break;
        case 6: // count
            memcpy(&mEventInfoCount,&info,sizeof(EventInfo));
            break;
        case 7: // count
            memcpy(&mEventInfoConnect,&info,sizeof(EventInfo));
            break;
        default: break;
        }

        if(info.event_count>time_count_max&&info.event_enable==true){
            time_count_max=info.event_count;
            mMaxSeconds = 60*time_count_max;
        }
    }
}

void WarningHandler::get_recent_powerinfo()
{
    QSqlQuery query;
    QString sql = QString("SELECT * FROM batteryPowerInfo "
                          "WHERE recordTime > datetime('now','localtime','-%1 seconds') "
                          "ORDER BY clientId, recordTime DESC;").arg(mMaxSeconds);
    bool r1 = query.exec(sql);
    qDebug()<<r1<<sql;

    int client_id_last=-1;
    int flags1[6]={0,0,0,0,0};// 记录上一次的数据情况
    int flags2[6]={0,0,0,0,0};// 1-报警 2-解除 0-保持
    QMap<int,bool> battery_alive_map;
    while(query.next()){
        int client_id = query.value(1).toInt();
        if(client_id!=client_id_last){
            update_warning_info(client_id_last,&flags2[0]);
            memset(&flags1,0,sizeof(flags1));
            memset(&flags2,0,sizeof(flags2));
            client_id_last=client_id;
        }

        int rate = query.value(5).toInt();
        int voltage = query.value(6).toInt();
        int current = query.value(7).toInt();
        int volume = query.value(8).toInt();
        int temp = query.value(9).toInt();
        int count = query.value(11).toInt(); // 循环次数 寿命
        QDateTime record_time=query.value(14).toDateTime();

        int property_index=-1;
        float property_value=-1.0;
        QDateTime current_time = QDateTime::currentDateTime();
        if(record_time.secsTo(current_time)<mEventInfoRate.event_count*60){
            property_index=mEventInfoRate.property-1;
            float rate_tt = rate;
            property_value = rate_tt;
        }
        if(record_time.secsTo(current_time)<mEventInfoVoltage.event_count*60){
            property_index=mEventInfoVoltage.property-1;
            float voltage_tt = voltage;
            voltage_tt = voltage_tt/100.0;
            property_value = voltage_tt;
        }
        if(record_time.secsTo(current_time)<mEventInfoCurrent.event_count*60){
            property_index=mEventInfoCurrent.property-1;
            float current_tt = current;
            current_tt = current_tt/100.0;
            property_value = current_tt;
        }
        if(record_time.secsTo(current_time)<mEventInfoVolume.event_count*60){
            property_index=mEventInfoVolume.property-1;
            float volume_tt = volume;
            volume_tt = volume_tt/100.0;
            property_value = volume_tt;
        }
        if(record_time.secsTo(current_time)<mEventInfoTemp.event_count*60){
            property_index=mEventInfoTemp.property-1;
            float temp_tt = temp;
            temp_tt = temp_tt/10.0;
            property_value = temp_tt;
        }
        if(record_time.secsTo(current_time)<mEventInfoCount.event_count*60){
            property_index=mEventInfoCount.property-1;
            float count_tt = count;
            count_tt = count_tt/10.0;
            property_value = count_tt;
        }
        if(record_time.secsTo(current_time)<mEventInfoConnect.event_count*60){
            battery_alive_map[client_id]=true;
        }

        // 全部错误就报警 全部正确就解除报警
        bool r1 = is_property_warning(property_index+1,property_value);
        if(flags1[property_index]==0){
            if(r1) {
                flags1[property_index]=1;
                flags2[property_index]=1;
            } else {
                flags1[property_index]=2;
                flags2[property_index]=2;
            }
        } else if(flags1[property_index]==1){
            if(r1==false){
                flags2[property_index]=0;
            }
        } else if(flags1[property_index]==2){
            if(r1==true){
                flags2[property_index]=0;
            }
        }
    }

    update_warning_info(client_id_last,&flags2[0]);
    update_warning_offline_info(battery_alive_map);
}


bool WarningHandler::is_property_warning(int type, float value)
{
    EventInfo *p_event_info = NULL;
    switch (type){
    case 1:
        p_event_info=&mEventInfoRate;
        break;
    case 2:
        p_event_info=&mEventInfoVoltage;
        break;
    case 3:
        p_event_info=&mEventInfoCurrent;
        break;
    case 4:
        p_event_info=&mEventInfoVolume;
        break;
    case 5:
        p_event_info=&mEventInfoTemp;
        break;
    case 6:
        p_event_info=&mEventInfoCount;
        break;
    default:
        p_event_info=NULL;
        break;
    }
    if(p_event_info==NULL){
        return false;
    }


    if(p_event_info->event_enable==false){
        return false;
    }

    if(p_event_info->event_max_enable){
        if(value>p_event_info->event_max){
            return true;
        }
    }

    if(p_event_info->event_min_enable){
        if(value<p_event_info->event_min){
            return true;
        }
    }

    return false;
}

void WarningHandler::update_warning_info(int client, int *flags)
{
    if(client<0||client>60000){
        return;
    }

    QSqlQuery query;
    for (int index=0;index<6;index++){
        bool update=false;
        QString sql = QString("SELECT * FROM warningInfo "
                              "WHERE clientId=%1 AND event=%2 "
                              "ORDER BY id DESC LIMIT 0,1")
                .arg(client).arg(index+1);
        bool r1 = query.exec(sql);
        qDebug()<<r1<<sql;
        if(query.next()){
            bool state = query.value(8).toBool(); // true-报警 false-已解除
            int sn = query.value(0).toInt();
            if(flags[index]==1){ // 要报警
                if(state==false){
                    QString sql = QString("INSERT INTO warningInfo (clientId,event,time_start,state) "
                                          "VALUES (%1,%2,datetime('now','localtime'),%3)")
                            .arg(client).arg(index+1).arg(true);
                    bool r1 = query.exec(sql);
                    qDebug()<<r1<<sql;
                    update=true;
                }
            }
            if(flags[index]==2){
                if(state==true){
                    QString sql = QString("UPDATE warningInfo "
                                          "SET time_end=datetime('now','localtime'), "
                                          "state=%1 WHERE id=%2").arg(false).arg(sn);
                    bool r1 = query.exec(sql);
                    qDebug()<<r1<<sql;
                    update=true;
                }
            }
        } else {
            if(flags[index]==1){ // 要报警
                QString sql = QString("INSERT INTO warningInfo (clientId,event,time_start,state) "
                                      "VALUES (%1,%2,datetime('now','localtime'),%3)")
                        .arg(client).arg(index+1).arg(true);
                bool r1 = query.exec(sql);
                qDebug()<<r1<<sql;
                update=true;
            }
        }

        if(update){
            update_warning_model();
        }
    }
}

void WarningHandler::update_warning_offline_info(QMap<int, bool> &online)
{
    QSqlQuery query;
    bool update=false;

    // 处于离线状态的电池
    QMap<int,int> battery_sn_map;
    QString sql = QString("SELECT * FROM warningInfo "
                  "WHERE state=%1 AND event=7;").arg(true);
    bool r1=query.exec(sql);
    qDebug()<<r1<<sql;
    while(query.next()){
        int sn = query.value(0).toInt();
        int id = query.value(1).toInt();
        battery_sn_map[id]=sn;
        qDebug()<<__FUNCTION__<<"warning"<<id<<sn;
    }

    QMapIterator<int, bool> iter1(online);
    while(iter1.hasNext()){
        iter1.next();

        if(battery_sn_map.contains(iter1.key())){
            QString sql = QString("UPDATE warningInfo "
                                  "SET time_end=datetime('now','localtime'), "
                                  "state=%1 WHERE id=%2")
                    .arg(false).arg(battery_sn_map[iter1.key()]);
            bool r1 = query.exec(sql);
            qDebug()<<r1<<sql;
            update=true;
        }
    }

    QMap<int, bool> battery_all_map;
    sql = QString("SELECT clientId FROM batteryDetailInfo;");
    r1=query.exec(sql);
    qDebug()<<r1<<sql;

    while(query.next()){
        int client = query.value(0).toInt();
        battery_all_map[client]=true;
    }

    QMapIterator<int, bool> iter2(battery_all_map);
    while(iter2.hasNext()){
        iter2.next();

        if(false==online.contains(iter2.key())){
            if(false==battery_sn_map.contains(iter2.key())){
                QString sql = QString("INSERT INTO warningInfo (clientId,event,time_start,state) "
                                      "VALUES (%1,%2,datetime('now','localtime'),%3)")
                        .arg(iter2.key()).arg(7).arg(true);
                bool r1 = query.exec(sql);
                qDebug()<<r1<<sql;
                update=true;
            }
        }
    }

    if(update){
        update_warning_model();
    }
}

void WarningHandler::update_warning_model()
{
//    mModelWarningInfo->clear();
    mModelWarningInfo->rowCount();
    mModelWarningInfo->removeRows(0,mModelWarningInfo->rowCount());

    QSqlQuery query;
    QString sql = QString("SELECT * FROM warningInfo ORDER BY id DESC LIMIT 0,10;");
    bool r1 = query.exec(sql);
    qDebug()<<r1<<sql;

    int row=0;
    while(query.next()){
        int client = query.value(1).toInt();
        QDateTime event_time = query.value(6).toDateTime();
        int event = query.value(5).toInt();
        bool state = query.value(8).toBool();
        qDebug()<<__FUNCTION__<<event_time.toString("yyyy-MM-dd hh:mm:ss");

        QStandardItem *item_client = new QStandardItem(QString::number(client));
        QStandardItem *item_time = new QStandardItem(event_time.toString("yyyy-MM-dd hh:mm:ss"));
        QStandardItem *item_desc = new QStandardItem(mEventDesc.value(event));
        QStandardItem *item_state = new QStandardItem(state?tr("Warning On"):tr("Warning Off"));
        item_client->setEditable(false);
        item_time->setEditable(false);
        item_desc->setEditable(false);
        item_state->setEditable(false);
        mModelWarningInfo->setItem(row,0,item_client);
        mModelWarningInfo->setItem(row,1,item_time);
        mModelWarningInfo->setItem(row,2,item_state);
        mModelWarningInfo->setItem(row,3,item_desc);
        row++;
    }
}

void WarningHandler::updateWarningTaskModel()
{
//    mModelWarningTaskInfo->clear();
    mModelWarningTaskInfo->rowCount();
    mModelWarningTaskInfo->removeRows(0,mModelWarningTaskInfo->rowCount());

    QSqlQuery query;
    QString sql = QString("SELECT * FROM taskInfo WHERE NOT task_state=2 "
                          "ORDER BY num DESC LIMIT 0,10;");
    bool r1 = query.exec(sql);
    qDebug()<<r1<<sql;

    int row=0;
    while(query.next()){
        int num = query.value(0).toInt();
        int client = query.value(1).toInt();
        QDateTime task_time = query.value(2).toDateTime();
        QString task_desc = query.value(3).toString();
        int state = query.value(7).toInt();
        qDebug()<<__FUNCTION__<<task_time.toString("yyyy-MM-dd hh:mm:ss");

        QStandardItem *item_num = new QStandardItem(QString::number(num));
        QStandardItem *item_client = new QStandardItem(QString::number(client));
        QStandardItem *item_time = new QStandardItem(task_time.toString("yyyy-MM-dd hh:mm:ss"));
        QStandardItem *item_desc = new QStandardItem(task_desc);
        QString state_str = tr("task open");
        if(state==1) state_str = tr("task begin");
        if(state==2) state_str = tr("task finish");
        QStandardItem *item_state = new QStandardItem(state_str);
        item_num->setEditable(false);
        item_client->setEditable(false);
        item_time->setEditable(false);
        item_desc->setEditable(false);
        item_state->setEditable(false);
        mModelWarningTaskInfo->setItem(row,0,item_num);
        mModelWarningTaskInfo->setItem(row,1,item_client);
        mModelWarningTaskInfo->setItem(row,2,item_time);
        mModelWarningTaskInfo->setItem(row,3,item_state);
        mModelWarningTaskInfo->setItem(row,4,item_desc);
        row++;
    }
}

void WarningHandler::onTaskAppend()
{
    updateWarningTaskModel();
}
