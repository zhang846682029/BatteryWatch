#include "datahandler.h"
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QStandardPaths>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDateTime>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QMap>
#include <QMapIterator>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QDebug>

DataHandler::DataHandler(QObject *parent) : QObject(parent)
{
    mModelTree = new QStandardItemModel(this);

    QSqlDatabase dbconn = QSqlDatabase::addDatabase("SQLITECIPHER");
    // Set database name, which is the database file name in SQLite
    QString dbPathName = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir(dbPathName);
    dir.mkpath(dbPathName);
    dbPathName.append("/battery.db");
    qDebug()<<dbPathName;
    dbconn.setDatabaseName(dbPathName);
    // Set password if you like.
    // Leave it empty if you don't want to use password.
//    dbconn.setPassword("1qazxsw23edc");
//    dbconn.setConnectOptions("QSQLITE_CREATE_KEY"); // 用一个单独的软件给数据库加密
//    dbconn.setConnectOptions("QSQLITE_UPDATE_KEY=newtest"); // set new password
    dbconn.setConnectOptions("QSQLITE_REMOVE_KEY");
    if (!dbconn.open()) {
        mDatabaseValid = false;
        qDebug() << "Can not open connection: " << dbconn.lastError().driverText();
        return;
    } else {
        mDatabaseValid = true;
    }

    if(dbconn.tables().count()<6){
        creatTablePowerInfo();
        creatTableWarningInfo();
        creatTablePropertyInfo();
        creatTableEventInfo();
        creatTableBatteryDetailInfo();
        creatTableZoneInfo();
        creatTableGroupInfo();

        initPropertyInfo();
        initZoneAndGroup();
    }
    qDebug()<<dbconn.tables();

    startTimer(1*1000); // 10秒刷新一次界面数据
    // 库伦计60秒上传一次数据

    getDeviceTree();

}

DataHandler::~DataHandler()
{
    QSqlDatabase dbconn = QSqlDatabase::database();
    dbconn.close();
    qDebug()<<"db close";
}

void DataHandler::creatTablePowerInfo()
{
    QString sql = QString("CREATE TABLE [batteryPowerInfo]("
                          "[ip] TEXT NOT NULL,"
                          "[mac] TEXT NOT NULL,"
                          "[address] INTEGER NOT NULL, "
                          "[rate] INTEGER NOT NULL, "
                          "[voltage] INTEGER NOT NULL, "
                          "[current] INTEGER NOT NULL, "
                          "[volume] INTEGER NOT NULL, "
                          "[temp] INTEGER NOT NULL, "
                          "[direction] INTEGER NOT NULL, "
                          "[count] INTEGER NOT NULL, "
                          "[alarm] INTEGER NOT NULL, "
                          "[interval] INTEGER,"
                          "[time] TIME);");
    QSqlQuery query;
    query.exec(sql);
}

void DataHandler::creatTableWarningInfo()
{
    // id 事件编号 工单表中根据这个id找到具体的事件信息
    QString sql = QString("CREATE TABLE [warningInfo]("
                          "[id] INTEGER NOT NULL UNIQUE, "
                          "[ip] TEXT, "
                          "[mac] TEXT NOT NULL, "
                          "[address] INTEGER NOT NULL, "
                          "[event] INTEGER NOT NULL REFERENCES [eventInfo]([id]), "
                          "[time] TIME NOT NULL);");
    QSqlQuery query;
    query.exec(sql);
}

void DataHandler::creatTablePropertyInfo()
{
    QString sql = QString("CREATE TABLE [batteryPropertyInfo]("
                          "[id] INTEGER NOT NULL UNIQUE, "
                          "[name] TEXT NOT NULL);");
    QSqlQuery query;
    query.exec(sql);
}

void DataHandler::creatTableEventInfo()
{
    // desc 异常事件说明
    // property 监控的参数
    // max 该参数的最大值
    // min 该参数的最小值
    // count 连续异常超过count次就生成一个报警记录
    QString sql = QString("CREATE TABLE [eventInfo]("
                          "[id] INTEGER NOT NULL UNIQUE, "
                          "[desc] TEXT NOT NULL, "
                          "[property] INTEGER NOT NULL, "
                          "[max] FLOAT DEFAULT (-100000.00), "
                          "[min] FLOAT DEFAULT (100000.00), "
                          "[count] INTEGER NOT NULL DEFAULT 0);");
    QSqlQuery query;
    query.exec(sql);
}

void DataHandler::creatTableBatteryDetailInfo()
{
    QString sql = QString("CREATE TABLE [batteryDetailInfo]("
                          "[ip] TEXT, "
                          "[mac] TEXT NOT NULL UNIQUE, "
                          "[address] INTEGER NOT NULL, "
                          "[manufacturer] TEXT, "
                          "[serialNumber] TEXT, "
                          "[installationDate] TIME, "
                          "[batteryVolume] FLOAT, "
                          "[zoneId] INTEGER DEFAULT 0 REFERENCES [zoneInfo]([id]) ON DELETE SET DEFAULT, "
                          "[groupId] INTEGER DEFAULT 0 REFERENCES [groupInfo]([id]) ON DELETE SET DEFAULT);");
    QSqlQuery query;
    query.exec(sql);
}

void DataHandler::creatTableZoneInfo()
{
    QString sql = QString("CREATE TABLE [zoneInfo]("
                          "[id] INTEGER PRIMARY KEY AUTOINCREMENT, "
                          "[name] TEXT NOT NULL);");
    QSqlQuery query;
    query.exec(sql);
}

void DataHandler::creatTableGroupInfo()
{
    QString sql = QString("CREATE TABLE [groupInfo]("
                          "[id] INTEGER PRIMARY KEY AUTOINCREMENT, "
                          "[name] TEXT NOT NULL, "
                          "[zone] INTEGER REFERENCES [zoneInfo]([id]));");
    QSqlQuery query;
    query.exec(sql);
}

void DataHandler::initPropertyInfo()
{
    QStringList propertyList;
    propertyList<<"rate"<<"voltage"<<"current"<<"volume"<<"temp";
    QSqlDatabase::database().transaction();
    QSqlQuery query;
    query.prepare("INSERT INTO batteryPropertyInfo "
                  "(id,name) VALUES (?,?)");
    for(int n=0;n<propertyList.size();n++){
        query.addBindValue(n+1);
        query.addBindValue(propertyList.at(n));
        query.exec();
    }
    QSqlDatabase::database().commit();
}

void DataHandler::initZoneAndGroup()
{
    QSqlQuery query;
    QString sql = QString("INSERT INTO zoneInfo (name) VALUES ('z0')");
    bool r1 = query.exec(sql);
    qDebug()<<r1<<sql;

    sql = QString("INSERT INTO groupInfo (name,zone) VALUES ('g0',1)");
    r1 = query.exec(sql);
    qDebug()<<r1<<sql;
}

void DataHandler::getRealtimeDataFromDb()
{
    // 设备在线信息
    getDeviceTree();
    // 设备详细信息
    // 报警信息
}

void DataHandler::getDeviceTree()
{
    QSqlQuery query;
    QMap<int,QStandardItem *> map_zone_item;
    query.exec("SELECT id,name FROM zoneInfo WHERE NOT id=1");
    while(query.next()){
        int zone_id=query.value(0).toInt();
        QString zone_name=query.value(1).toString();
        QStandardItem *item = new QStandardItem(zone_name);
        item->setData(zone_name);
        map_zone_item[zone_id]=item;
        qDebug()<<"zone"<<zone_id<<zone_name;
    }

    QMap<int,QStandardItem *> map_group_item;
    QMapIterator<int,QStandardItem *> it1(map_zone_item);
    while (it1.hasNext()) {
        it1.next();
        QString sql = QString("SELECT id,name FROM groupInfo WHERE zone=%1 AND NOT id=1")
                .arg(it1.key());
        query.exec(sql);
        while(query.next()){
            int group_id=query.value(0).toInt();
            QString group_name=query.value(1).toString();
            QStandardItem *item = new QStandardItem(group_name);
            item->setData(group_name);
            map_group_item[group_id]=item;
            it1.value()->appendRow(item);
            qDebug()<<"group"<<group_id<<group_name;
        }
//        it1++;
    }

    QMap<QString,QStandardItem *> map_device_item;
    QMapIterator<int,QStandardItem *> it2(map_group_item);
    while (it2.hasNext()) {
        it2.next();
        QString sql = QString("SELECT * FROM batteryDetailInfo "
                              "WHERE groupId=%1").arg(it2.key());
        bool r1=query.exec(sql);
        qDebug()<<r1<<sql;
        while(query.next()){
            QString device_mac=query.value(1).toString();
            int device_address=query.value(2).toInt();
            QString device_address_str = QString::number(device_address);
            QString device_ip=query.value(0).toString();
            QString device_manufacturer=query.value(3).toString();
            QString device_sn=query.value(4).toString();
            QString device_install=query.value(5).toDateTime().toString("yyyy-MM-dd hh:mm:ss");
//            float device_volume=query.value(1).toFloat();
            QString device_volume=QString::number(query.value(6).toFloat());
            int zone_id = query.value(7).toInt();
            QStandardItem *item_z = map_zone_item.value(zone_id,NULL);
            QString device_zone = item_z!=NULL?item_z->data().toString():QString();
            int group_id = query.value(8).toInt();
            QStandardItem *item_g = map_group_item.value(group_id,NULL);
            QString device_group = item_g!=NULL?item_g->data().toString():QString();
            QJsonObject device_info_map;
            device_info_map["ip"]=device_ip;
            device_info_map["mac"]=device_mac;
            device_info_map["address"]=device_address_str;
            device_info_map["manufacturer"]=device_manufacturer;
            device_info_map["sn"]=device_sn;
            device_info_map["install"]=device_install;
            device_info_map["volume"]=device_volume;
            device_info_map["zone"]=device_zone;
            device_info_map["group"]=device_group;
            QJsonDocument doc(device_info_map);
            QByteArray array=doc.toJson();

            QStandardItem *item = new QStandardItem(QString("%1 [%2]")
                                                    .arg(device_mac)
                                                    .arg(device_address,3,10,QChar('0')));
            item->setData(array);
            map_device_item[device_mac]=item;
            it2.value()->appendRow(item);
        }
    }

    QMap<QString,QStandardItem *> map_device_without_group_item;
    if(true){
        QString sql = QString("SELECT * FROM batteryDetailInfo");
        bool r1=query.exec(sql);
        qDebug()<<r1<<sql;
        while(query.next()){
            QString device_mac=query.value(1).toString();
            if(map_device_item.contains(device_mac)) continue;

            int device_address=query.value(2).toInt();
            QString device_address_str = QString::number(device_address);
            QString device_ip=query.value(0).toString();
            QString device_manufacturer=query.value(3).toString();
            QString device_sn=query.value(4).toString();
            QString device_install=query.value(5).toDateTime().toString("yyyy-MM-dd hh:mm:ss");
            //            float device_volume=query.value(1).toFloat();
            QString device_volume=QString::number(query.value(6).toFloat());
            int zone_id = query.value(7).toInt();
            QStandardItem *item_z = map_zone_item.value(zone_id,NULL);
            QString device_zone = item_z!=NULL?item_z->data().toString():QString();
            int group_id = query.value(8).toInt();
            QStandardItem *item_g = map_group_item.value(group_id,NULL);
            QString device_group = item_g!=NULL?item_g->data().toString():QString();
            QJsonObject device_info_map;
            device_info_map["ip"]=device_ip;
            device_info_map["mac"]=device_mac;
            device_info_map["address"]=device_address_str;
            device_info_map["manufacturer"]=device_manufacturer;
            device_info_map["sn"]=device_sn;
            device_info_map["install"]=device_install;
            device_info_map["volume"]=device_volume;
            device_info_map["zone"]=device_zone;
            device_info_map["group"]=device_group;
            QJsonDocument doc(device_info_map);
            QByteArray array=doc.toJson();

            QStandardItem *item = new QStandardItem(QString("%1 [%2]")
                                                    .arg(device_mac)
                                                    .arg(device_address,3,10,QChar('0')));
            item->setData(array);
            map_device_without_group_item[device_mac]=item;
        }
    }

    mModelTree->clear();
    QStandardItem *parentItem = mModelTree->invisibleRootItem();
    QMapIterator<int,QStandardItem *> it3(map_zone_item);
    while (it3.hasNext()) {
        it3.next();
        parentItem->appendRow(it3.value());
    }

    QMapIterator<QString,QStandardItem *> it6(map_device_without_group_item);
    while (it6.hasNext()) {
        it6.next();
        parentItem->appendRow(it6.value());
    }

    // 用字体颜色区别在线离线状态
    // 掉线(没上线) 在线
    map_device_item.unite(map_device_without_group_item);
    mDeviceWithoutGroupItem.swap(map_device_item);
    QMapIterator<QString,QStandardItem *> it4(mDeviceWithoutGroupItem);
    while (it4.hasNext()) {
        it4.next();
        auto it5 = mOnlineTime.find(it4.key());
        if(it5==mOnlineTime.end()){
            it4.value()->setForeground(QBrush(Qt::gray));
        } else {
            QTime time_tt = QTime::currentTime();
            int interval=time_tt.secsTo(it5.value());
            if(interval>3*60){
                it4.value()->setForeground(QBrush(Qt::gray));
            } else {
                it4.value()->setForeground(QBrush(Qt::green));
            }
        }
    }

    emit sigTreeModelUpdate();
}

void DataHandler::updateOnlineStatus()
{
    QMapIterator<QString,QStandardItem *> it4(mDeviceWithoutGroupItem);
    while (it4.hasNext()) {
        it4.next();
        auto it5 = mOnlineTime.find(it4.key());
        if(it5==mOnlineTime.end()){
            it4.value()->setForeground(QBrush(Qt::gray));
        } else {
            QTime time_tt = QTime::currentTime();
            int interval=it5.value().secsTo(time_tt);
            if(interval>3*60){
                it4.value()->setForeground(QBrush(Qt::gray));
            } else {
                it4.value()->setForeground(QBrush(Qt::green));
            }
        }
    }
}

/**
 * @brief 1.原来属于这个组的 调整到默认分组
 *      2.删除组信息
 * @param id
 */
void DataHandler::onGroupDelete(int id)
{
    if(id==1){
        return;
    }

    QSqlQuery query;
    QString sql = QString("SELECT mac FROM batteryDetailInfo WHERE groupId=%1").arg(id);
    bool r1 = query.exec(sql);
    qDebug()<<r1<<sql;
    QStringList macList;
    while(query.next()){
        macList.append(query.value(0).toString());
    }

    QStringList::Iterator it = macList.begin();
    while(it!=macList.end()){
        QString sql = QString("UPDATE batteryDetailInfo SET groupId=1,zoneId=1 WHERE mac='%1'").arg(*it);
        bool r1 = query.exec(sql);
        qDebug()<<r1<<sql;
        it++;
    }

    sql = QString("DELETE FROM groupInfo WHERE id=%1").arg(id);
    r1 = query.exec(sql);
    qDebug()<<r1<<sql;

    getDeviceTree();
}

/**
 * @brief 1.删除区域里的分组
 *      2.区域里的设备移动到默认分组
 *      3.删除区域信息
 * @param id
 */
void DataHandler::onZoneDelete(int id)
{
    if(id==1){
        return;
    }

    QSqlQuery query;
    QString sql = QString("SELECT id FROM groupInfo WHERE zone=%1").arg(id);
    bool r1 = query.exec(sql);
    QVector<int> group_id_vec;
    while(query.next()){
        group_id_vec.append(query.value(0).toInt());
    }

    QStringList macList;
    QVector<int>::Iterator it1 = group_id_vec.begin();
    while(it1!=group_id_vec.end()){
        QString sql = QString("SELECT mac FROM batteryDetailInfo "
                              "WHERE groupId=%1 AND zoneId=%2")
                .arg(*it1).arg(id);
        bool r1 = query.exec(sql);
        qDebug()<<r1<<sql;
        while(query.next()){
            macList.append(query.value(0).toString());
        }
        it1++;
    }
    QStringList::Iterator it = macList.begin();
    while(it!=macList.end()){
        QString sql = QString("UPDATE batteryDetailInfo SET groupId=1,zoneId=1 WHERE mac='%1'").arg(*it);
        bool r1 = query.exec(sql);
        qDebug()<<r1<<sql;
        it++;
    }

    sql = QString("DELETE FROM groupInfo WHERE zone=%1").arg(id);
    r1 = query.exec(sql);
    qDebug()<<r1<<sql;

    sql = QString("DELETE FROM zoneInfo WHERE id=%1").arg(id);
    r1 = query.exec(sql);
    qDebug()<<r1<<sql;

    getDeviceTree();
}

/**
 * @brief 1.更新设备信息 2.更新组信息
 * @param zone_from
 * @param name_from
 * @param zone_to
 * @param name_to
 */
void DataHandler::onGroupModify(int id, int zone_from, QString name_from, int zone_to, QString name_to)
{
    QSqlQuery query;
    if(zone_from!=zone_to){
        QString sql = QString("UPDATE batteryDetailInfo SET zoneId=%1 WHERE mac IN "
                              "(SELECT mac FROM batteryDetailInfo groupId=%2 AND zoneId=%3)")
                .arg(zone_to).arg(id).arg(zone_from);
        bool r1 = query.exec(sql);
        qDebug()<<r1<<sql;
    }

    QString sql = QString("UPDATE groupInfo SET name='%1',zone=%2 WHERE id = %3")
            .arg(name_to).arg(zone_to).arg(id);
    bool r1 = query.exec(sql);
    qDebug()<<r1<<sql;

    getDeviceTree();
}

void DataHandler::onZoneModify(int id, QString name_from, QString name_to)
{
    QSqlQuery query;
    QString sql = QString("UPDATE zoneInfo SET name='%1' WHERE id = %2")
            .arg(name_to).arg(id);
    bool r1 = query.exec(sql);
    qDebug()<<r1<<sql;

    getDeviceTree();
}

void DataHandler::onGroupAppend(int id, QString name)
{
    QSqlQuery query;
    QString sql = QString("INSERT INTO groupInfo (name,zone) VALUES ('%1',%2)").arg(name).arg(id);
    bool r1 = query.exec(sql);
    qDebug()<<r1<<sql;
}

void DataHandler::onZoneAppend(QString name)
{
    QSqlQuery query;
    QString sql = QString("INSERT INTO zoneInfo (name) VALUES ('%1')").arg(name);
    bool r1 = query.exec(sql);
    qDebug()<<r1<<sql;
}

void DataHandler::onDeviceAppend(QString ip, QString mac, int address)
{
    QSqlQuery query;
    QString sql=QString("INSERT INTO batteryDetailInfo "
                        "(ip,mac,address,zoneId,groupId) "
                        "VALUES ('%1','%2',%3,1,1)")
            .arg(ip).arg(mac).arg(address);
    bool r1 = query.exec(sql);
    qDebug()<<__FUNCTION__<<r1<<sql;

    getDeviceTree();
}

void DataHandler::slotItemClicked(const QModelIndex &index)
{
    QStandardItemModel* model=(QStandardItemModel*)index.model();
    QStandardItem *item=model->itemFromIndex(index);
    QByteArray array=item->data().toByteArray();
//    qDebug()<<__FUNCTION__<<array;

    QJsonParseError errorLoad;
    QJsonDocument doc = QJsonDocument::fromJson(array,&errorLoad);
    if(QJsonParseError::NoError!=errorLoad.error){
        return;
    }

    QJsonObject obj=doc.object();
    QString device_ip = obj["ip"].toString();
    QString device_mac = obj["mac"].toString();
    QString device_address = obj["address"].toString();
    QString device_manufacturer = obj["manufacturer"].toString();
    QString device_sn = obj["sn"].toString();
    QString device_install = obj["install"].toString();
    QString device_volume = obj["volume"].toString();
    QString device_zone = obj["zone"].toString();
    QString device_group = obj["group"].toString();
    qDebug()<<device_ip<<device_mac<<device_address<<device_zone<<device_group;

    mCurrentMac = device_mac;
    mCurrentAddress = device_address.toInt();
}

void DataHandler::slotAppendPowerInfo(QString data, int len)
{
    QJsonDocument json_doc=QJsonDocument::fromJson(data.toLatin1());
    QJsonObject json_obj=json_doc.object();
    QJsonObject json_obj_identify = json_obj.value("identify").toObject();
    QString identify_ip = json_obj_identify.value("ip").toString();
    QString identify_mac = json_obj_identify.value("mac").toString();
    int identify_address = json_obj_identify.value("address").toInt();
    QJsonObject json_obj_property = json_obj.value("property").toObject();
    int property_rate = json_obj_property.value("rate").toInt();
    int property_voltage = json_obj_property.value("voltage").toInt();
    int property_current = json_obj_property.value("current").toInt();
    int property_volume = json_obj_property.value("volume").toInt();
    int property_temp = json_obj_property.value("temp").toInt();
    int property_direction = json_obj_property.value("direction").toInt();
    int property_count = json_obj_property.value("count").toInt();
    int property_alarm = json_obj_property.value("alarm").toInt();
    int property_interval = json_obj_property.value("interval").toInt();
    qDebug()<<__FUNCTION__<<identify_address<<identify_mac<<identify_ip<<property_rate;

    QString sql2=QString("INSERT INTO batteryPowerInfo "
            "(ip,mac,address,rate,voltage,current,volume,"
            "temp,direction,count,alarm,interval,time) "
            "VALUES ('%1','%2',%3,%4,%5,%6,%7,%8,%9,%10,%11,%12,"
            "datetime('now','localtime'))")
            .arg(identify_ip)
            .arg(identify_mac)
            .arg(identify_address)
            .arg(property_rate)
            .arg(property_voltage)
            .arg(property_current)
            .arg(property_volume)
            .arg(property_temp)
            .arg(property_direction)
            .arg(property_count)
            .arg(property_alarm)
            .arg(property_interval);
    QSqlQuery query;
    bool r2=query.exec(sql2);
    qDebug()<<r2<<sql2;

    if(mCurrentMac==identify_mac&&mCurrentAddress==identify_address){
        // todo
        // 更新仪表盘
    }

    mOnlineTime[identify_mac]=QTime::currentTime();
    if(mDeviceWithoutGroupItem.find(identify_mac)==mDeviceWithoutGroupItem.end()){
        onDeviceAppend(identify_ip,identify_mac,identify_address);
    }
}

void DataHandler::timerEvent(QTimerEvent *event)
{
    updateOnlineStatus();
}