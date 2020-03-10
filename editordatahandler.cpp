#include "editordatahandler.h"
#include <QStringListModel>
#include <QStandardItemModel>
#include <QAbstractItemModel>
#include <QSqlQuery>
#include <QDebug>

EditorDataHandler::EditorDataHandler(QObject *parent) : QObject(parent)
{
    modelZone = new QStandardItemModel(this);
    modelGroupIn = new QStandardItemModel(this);
    modelGroupOut = new QStandardItemModel(this);

    modelGroup = new QStandardItemModel(this);
    modelDeviceIn = new QStandardItemModel(this);
    modelDeviceOut = new QStandardItemModel(this);
}

QAbstractItemModel *EditorDataHandler::getModelZone()
{
    return modelZone;
}

QAbstractItemModel *EditorDataHandler::getModelGroupIn()
{
    return modelGroupIn;
}

QAbstractItemModel *EditorDataHandler::getModelGroupOut()
{
    return modelGroupOut;
}

QAbstractItemModel *EditorDataHandler::getModelGroup()
{
    return modelGroup;
}

QAbstractItemModel *EditorDataHandler::getModelDeviceIn()
{
    return modelDeviceIn;
}

QAbstractItemModel *EditorDataHandler::getModelDeviceOut()
{
    return modelDeviceOut;
}

void EditorDataHandler::initModelZone()
{
    modelZone->clear();
    QStandardItem *parentItem = modelZone->invisibleRootItem();

    QSqlQuery query;
    QString sql = QString("SELECT id,name FROM zoneInfo");
    bool r1 = query.exec(sql);
    qDebug()<<__FUNCTION__<<r1<<sql;
    while(query.next()){
        QString zone_name=query.value(1).toString();
        int zone_id=query.value(0).toInt();
        if(zone_id==1){
            continue;
        }

        QStandardItem *item = new QStandardItem(zone_name);
        item->setData(zone_id);
        parentItem->appendRow(item);
    }

}

void EditorDataHandler::initGroupOut()
{
    modelGroupOut->clear();
    QStandardItem *parentItemOut = modelGroupOut->invisibleRootItem();

    QSqlQuery query;
    QString sql = QString("SELECT * FROM groupInfo WHERE zone=1 AND NOT id=1");
    bool r1 = query.exec(sql);
    qDebug()<<__FUNCTION__<<r1<<sql;
    while(query.next()){
        int group_id=query.value(0).toInt();
        QString group_name=query.value(1).toString();

        QStandardItem *item = new QStandardItem(group_name);
        item->setData(group_id);
        parentItemOut->appendRow(item);
    }
}

void EditorDataHandler::getGroupsInOut(int zone_id)
{
    modelGroupIn->clear();
    modelGroupOut->clear();
    QStandardItem *parentItemIn = modelGroupIn->invisibleRootItem();
    QStandardItem *parentItemOut = modelGroupOut->invisibleRootItem();

    QSqlQuery query;
    QString sql = QString("SELECT * FROM groupInfo WHERE NOT id=1");
    bool r1 = query.exec(sql);
    qDebug()<<__FUNCTION__<<r1<<sql;
    while(query.next()){
        int group_id=query.value(0).toInt();
        QString group_name=query.value(1).toString();
        int zone = query.value(2).toInt();

        QStandardItem *item = new QStandardItem(group_name);
        item->setData(group_id);
        if(zone==zone_id){
            parentItemIn->appendRow(item);
        }
        if(zone==1) {
            parentItemOut->appendRow(item);
        }
    }
}

void EditorDataHandler::onGroupOut(bool)
{
    QStandardItem *item_group=currentGroupInItem;
    currentGroupOutItem=nullptr;
    currentGroupInItem=nullptr;
    if(nullptr==item_group){
        return;
    }
    int group_id=item_group->data().toInt();

    QStandardItem *item_zone=currentZoneItem;
    if(nullptr==item_zone){
        return;
    }
    int zone_id=item_zone->data().toInt();

    QSqlQuery query;
    QString sql = QString("UPDATE groupInfo SET zone=1 WHERE id=%1").arg(group_id);
    bool r1 = query.exec(sql);
    qDebug()<<__FUNCTION__<<r1<<sql;

    getGroupsInOut(zone_id);
}

void EditorDataHandler::onGroupIn(bool)
{
    QStandardItem *item_group=currentGroupOutItem;
    currentGroupOutItem=nullptr;
    currentGroupInItem=nullptr;
    if(nullptr==item_group){
        return;
    }
    int group_id=item_group->data().toInt();

    QStandardItem *item_zone=currentZoneItem;
    if(nullptr==item_zone){
        return;
    }
    int zone_id=item_zone->data().toInt();

    QSqlQuery query;
    QString sql = QString("UPDATE groupInfo SET zone=%1 WHERE id=%2").arg(zone_id).arg(group_id);
    bool r1 = query.exec(sql);
    qDebug()<<__FUNCTION__<<r1<<sql;

    getGroupsInOut(zone_id);
}

void EditorDataHandler::initModelGroup()
{
    modelGroup->clear();
    QStandardItem *parentItem = modelGroup->invisibleRootItem();

    QSqlQuery query;
    QString sql = QString("SELECT * FROM groupInfo WHERE NOT id=1");
    bool r1 = query.exec(sql);
    qDebug()<<__FUNCTION__<<r1<<sql;
    while(query.next()){
        int group_id=query.value(0).toInt();
        QString group_name=query.value(1).toString();
        int zone_id=query.value(2).toInt();

        QStandardItem *item = new QStandardItem(group_name);
        item->setData(group_id);
        parentItem->appendRow(item);
    }

}

void EditorDataHandler::initDeviceOut()
{
    modelDeviceOut->clear();
    QStandardItem *parentItemOut = modelDeviceOut->invisibleRootItem();

    QSqlQuery query;
    QString sql = QString("SELECT clientId FROM batteryDetailInfo WHERE groupId=1");
    bool r1 = query.exec(sql);
    qDebug()<<__FUNCTION__<<r1<<sql;
    while(query.next()){
        int device_id=query.value(0).toInt();

        QStandardItem *item = new QStandardItem(QString("%1").arg(device_id,3,10,QChar('0')));
        item->setData(device_id);
        parentItemOut->appendRow(item);
    }
}

void EditorDataHandler::getDevicesInOut(int group_id)
{
    modelDeviceIn->clear();
    modelDeviceOut->clear();
    QStandardItem *parentItemIn = modelDeviceIn->invisibleRootItem();
    QStandardItem *parentItemOut = modelDeviceOut->invisibleRootItem();

    QSqlQuery query;
    QString sql = QString("SELECT clientId,groupId FROM batteryDetailInfo");
    bool r1 = query.exec(sql);
    qDebug()<<__FUNCTION__<<r1<<sql;
    while(query.next()){
        int device_id=query.value(0).toInt();
//        QString group_name=query.value(1).toString();
        int group = query.value(1).toInt();

        QStandardItem *item = new QStandardItem(QString("%1").arg(device_id,3,10,QChar('0')));
        item->setData(device_id);
        if(group==group_id){
            parentItemIn->appendRow(item);
        }
        if(group==1) {
            parentItemOut->appendRow(item);
        }
    }
}

void EditorDataHandler::onDeviceOut(bool)
{
    QStandardItem *item_group=currentDeviceInItem;
    currentDeviceOutItem=nullptr;
    currentDeviceInItem=nullptr;
    if(nullptr==item_group){
        return;
    }
    int device_id=item_group->data().toInt();

    QStandardItem *item_zone=currentGroupItem;
    if(nullptr==item_zone){
        return;
    }
    int group_id=item_zone->data().toInt();

    QSqlQuery query;
    QString sql = QString("UPDATE batteryDetailInfo SET groupId=1 WHERE clientId=%1").arg(device_id);
    bool r1 = query.exec(sql);
    qDebug()<<__FUNCTION__<<r1<<sql;

    getDevicesInOut(group_id);
}

void EditorDataHandler::onDeviceIn(bool)
{
    QStandardItem *item_group=currentDeviceOutItem;
    currentDeviceOutItem=nullptr;
    currentDeviceInItem=nullptr;
    if(nullptr==item_group){
        return;
    }
    int device_id=item_group->data().toInt();

    QStandardItem *item_zone=currentGroupItem;
    if(nullptr==item_zone){
        return;
    }
    int group_id=item_zone->data().toInt();

    QSqlQuery query;
    QString sql = QString("UPDATE batteryDetailInfo SET groupId=%1 WHERE clientId=%2").arg(group_id).arg(device_id);
    bool r1 = query.exec(sql);
    qDebug()<<__FUNCTION__<<r1<<sql;

    getDevicesInOut(group_id);
}

void EditorDataHandler::slotModelZoneItemClicked(const QModelIndex &index)
{
    QStandardItemModel* model=(QStandardItemModel*)index.model();
    currentZoneItem=model->itemFromIndex(index);
    int zone_id=currentZoneItem->data().toInt();
    qDebug()<<__FUNCTION__<<index.row()<<zone_id;

    getGroupsInOut(zone_id);
}

void EditorDataHandler::slotModelGroupInItemClicked(const QModelIndex &index)
{
    QStandardItemModel* model=(QStandardItemModel*)index.model();
    currentGroupInItem=model->itemFromIndex(index);
    qDebug()<<__FUNCTION__;
}

void EditorDataHandler::slotModelGroupOutItemClicked(const QModelIndex &index)
{
    QStandardItemModel* model=(QStandardItemModel*)index.model();
    currentGroupOutItem=model->itemFromIndex(index);
    qDebug()<<__FUNCTION__;
}

void EditorDataHandler::slotModelGroupItemClicked(const QModelIndex &index)
{
    QStandardItemModel* model=(QStandardItemModel*)index.model();
    currentGroupItem=model->itemFromIndex(index);
    int zone_id=currentGroupItem->data().toInt();
    qDebug()<<__FUNCTION__<<index.row()<<zone_id;

    getDevicesInOut(zone_id);
}

void EditorDataHandler::slotModelDeviceInItemClicked(const QModelIndex &index)
{
    QStandardItemModel* model=(QStandardItemModel*)index.model();
    currentDeviceInItem=model->itemFromIndex(index);
    qDebug()<<__FUNCTION__;
}

void EditorDataHandler::slotModelDeviceOutItemClicked(const QModelIndex &index)
{
    QStandardItemModel* model=(QStandardItemModel*)index.model();
    currentDeviceOutItem=model->itemFromIndex(index);
    qDebug()<<__FUNCTION__;
}

void EditorDataHandler::slotTabChanged(int tab)
{
    if(tab==0){
        initModelZone();
        initGroupOut();
    } else {
        initModelGroup();
        initDeviceOut();
    }
}

/**
 * @brief 1.原来属于这个组的 调整到默认分组
 *      2.删除组信息
 * @param id
 */
void EditorDataHandler::onGroupDelete(void)
{
    if(currentGroupItem==nullptr){
        return;
    }
    int id = currentGroupItem->data().toInt();

    QSqlQuery query;
    QString sql = QString("SELECT clientId FROM batteryDetailInfo WHERE groupId=%1").arg(id);
    bool r1 = query.exec(sql);
    qDebug()<<r1<<sql;
    QVector<int> client_id_vec;
    while(query.next()){
        client_id_vec.append(query.value(0).toInt());
    }

    QVector<int>::Iterator it = client_id_vec.begin();
    while(it!=client_id_vec.end()){
        QString sql = QString("UPDATE batteryDetailInfo SET groupId=1,zoneId=1 WHERE clientId=%1").arg(*it);
        bool r1 = query.exec(sql);
        qDebug()<<r1<<sql;
        it++;
    }

    sql = QString("DELETE FROM groupInfo WHERE id=%1").arg(id);
    r1 = query.exec(sql);
    qDebug()<<r1<<sql;

    initModelGroup();
}

/**
 * @brief 1.删除区域里的分组
 *      2.区域里的设备移动到默认分组
 *      3.删除区域信息
 * @param id
 */
void EditorDataHandler::onZoneDelete(void)
{
    if(currentZoneItem==nullptr){
        return;
    }
    int id = currentZoneItem->data().toInt();

    QSqlQuery query;
    QString sql = QString("SELECT id FROM groupInfo WHERE zone=%1").arg(id);
    bool r1 = query.exec(sql);
    QVector<int> group_id_vec;
    while(query.next()){
        group_id_vec.append(query.value(0).toInt());
    }

    QVector<int> client_id_vec;
    QVector<int>::Iterator it1 = group_id_vec.begin();
    while(it1!=group_id_vec.end()){
        QString sql = QString("SELECT clientId FROM batteryDetailInfo "
                              "WHERE groupId=%1 AND zoneId=%2")
                .arg(*it1).arg(id);
        bool r1 = query.exec(sql);
        qDebug()<<r1<<sql;
        while(query.next()){
            client_id_vec.append(query.value(0).toInt());
        }
        it1++;
    }
    QVector<int>::Iterator it = client_id_vec.begin();
    while(it!=client_id_vec.end()){
        QString sql = QString("UPDATE batteryDetailInfo SET groupId=1,zoneId=1 WHERE clientId=%1").arg(*it);
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

    initModelZone();
}

/**
 * @brief 1.更新设备信息 2.更新组信息
 * @param zone_from
 * @param name_from
 * @param zone_to
 * @param name_to
 */
void EditorDataHandler::onGroupModify(QString name_from, QString name_to)
{
    if(currentGroupItem==nullptr){
        return;
    }
    int id = currentGroupItem->data().toInt();

    QSqlQuery query;
    QString sql = QString("UPDATE groupInfo SET name='%1' WHERE id = %2")
            .arg(name_to).arg(id);
    bool r1 = query.exec(sql);
    qDebug()<<r1<<sql;

    initModelGroup();
}

void EditorDataHandler::onZoneModify(QString name_from, QString name_to)
{
    if(currentZoneItem==nullptr){
        return;
    }
    int id = currentZoneItem->data().toInt();

    QSqlQuery query;
    QString sql = QString("UPDATE zoneInfo SET name='%1' WHERE id = %2")
            .arg(name_to).arg(id);
    bool r1 = query.exec(sql);
    qDebug()<<r1<<sql;

    initModelZone();
}

void EditorDataHandler::onGroupAppend(QString name)
{
    QSqlQuery query;
    QString sql = QString("INSERT INTO groupInfo (name,zone) VALUES ('%1',%2)").arg(name).arg(1);
    bool r1 = query.exec(sql);
    qDebug()<<r1<<sql;

    initModelGroup();
}

void EditorDataHandler::onZoneAppend(QString name)
{
    QSqlQuery query;
    QString sql = QString("INSERT INTO zoneInfo (name) VALUES ('%1')").arg(name);
    bool r1 = query.exec(sql);
    qDebug()<<r1<<sql;

    initModelZone();
}

void EditorDataHandler::onDeviceAppend(int id, QString ip, QString mac, int address)
{
    QSqlQuery query;
    QString sql=QString("INSERT INTO batteryDetailInfo "
                        "(clientId,clientIp,clientMac,clientAddress,zoneId,groupId) "
                        "VALUES (%1,'%2','%3',%4,1,1)")
            .arg(id).arg(ip).arg(mac).arg(address);
    bool r1 = query.exec(sql);
    qDebug()<<__FUNCTION__<<r1<<sql;
}
