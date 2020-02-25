#include "datahandler.h"
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QStandardPaths>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDateTime>
#include <QDebug>

DataHandler::DataHandler(QObject *parent) : QObject(parent)
{
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
    } else {
        mDatabaseValid = true;
    }

    if(mDatabaseValid){
        if(dbconn.tables().count()<6){
            creatTablePowerInfo();
            creatTableWarningInfo();
            creatTablePropertyInfo();
            creatTableEventInfo();
            creatTableBatteryDetailInfo();
            creatTableZoneInfo();
            creatTableGroupInfo();

            initPropertyInfo();
        }
        qDebug()<<dbconn.tables();

        startTimer(10*1000); // 10秒刷新一次界面数据
        // 库伦计60秒上传一次数据

    }
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
                          "[zone] INTEGER NOT NULL DEFAULT 0 REFERENCES [zoneInfo]([id]) ON DELETE SET DEFAULT, "
                          "[group] INTEGER NOT NULL DEFAULT 0 REFERENCES [groupInfo]([id]) ON DELETE SET DEFAULT);");
    QSqlQuery query;
    query.exec(sql);
}

void DataHandler::creatTableZoneInfo()
{
    QString sql = QString("CREATE TABLE [zoneInfo]("
                          "[id] INTEGER NOT NULL UNIQUE, "
                          "[name] TEXT NOT NULL);");
    QSqlQuery query;
    query.exec(sql);
}

void DataHandler::creatTableGroupInfo()
{
    QString sql = QString("CREATE TABLE [groupInfo]("
                          "[id] INTEGER NOT NULL UNIQUE, "
                          "[name] TEXT NOT NULL, "
                          "[groupId] INTEGER NOT NULL DEFAULT 0);");
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

void DataHandler::getRealtimeDataFromDb()
{
    // 设备在线信息
    // 设备详细信息
    // 报警信息
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
        // 更新仪表盘
    }
}

void DataHandler::timerEvent(QTimerEvent *event)
{
    getRealtimeDataFromDb();
}
