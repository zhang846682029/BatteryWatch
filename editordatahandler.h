#ifndef EDITORDATAHANDLER_H
#define EDITORDATAHANDLER_H

#include <QObject>

class QAbstractItemModel;
class QStandardItemModel;
class QStringListModel;
class QStandardItem;
class EditorDataHandler : public QObject
{
    Q_OBJECT
public:
    explicit EditorDataHandler(QObject *parent = 0);
    QAbstractItemModel *getModelZone(void);
    QAbstractItemModel *getModelGroupIn(void);
    QAbstractItemModel *getModelGroupOut(void);

    QAbstractItemModel *getModelGroup(void);
    QAbstractItemModel *getModelDeviceIn(void);
    QAbstractItemModel *getModelDeviceOut(void);

    void initModelZone(void);
    void initGroupOut(void);
    void getGroupsInOut(int zone_id);
    void onGroupOut(bool);
    void onGroupIn(bool);
    void onZoneDelete(void);
    void onZoneModify(QString name_from, QString name_to);
    void onZoneAppend(QString name);

    void initModelGroup(void);
    void initDeviceOut(void);
    void getDevicesInOut(int group_id);
    void onDeviceOut(bool);
    void onDeviceIn(bool);
    void onGroupDelete(void);
    void onGroupModify(QString name_from, QString name_to);
    void onGroupAppend(QString name);

    void onDeviceAppend(int id, QString ip, QString mac, int address);

signals:

public slots:
    void slotModelZoneItemClicked(const QModelIndex &index);
    void slotModelGroupInItemClicked(const QModelIndex &index);
    void slotModelGroupOutItemClicked(const QModelIndex &index);

    void slotModelGroupItemClicked(const QModelIndex &index);
    void slotModelDeviceInItemClicked(const QModelIndex &index);
    void slotModelDeviceOutItemClicked(const QModelIndex &index);

    void slotTabChanged(int);

private:
    QStandardItemModel *modelZone;
    QStandardItemModel *modelGroupIn;
    QStandardItemModel *modelGroupOut;
    QStandardItem *currentZoneItem;
    QStandardItem *currentGroupInItem;
    QStandardItem *currentGroupOutItem;

    QStandardItemModel *modelGroup;
    QStandardItemModel *modelDeviceIn;
    QStandardItemModel *modelDeviceOut;
    QStandardItem *currentGroupItem;
    QStandardItem *currentDeviceInItem;
    QStandardItem *currentDeviceOutItem;
};

#endif // EDITORDATAHANDLER_H
