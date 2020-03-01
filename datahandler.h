#ifndef DATAHANDLER_H
#define DATAHANDLER_H

#include <QObject>
#include <QTime>
#include <QMap>
#include <QModelIndex>

class QStandardItem;
class QStandardItemModel;
class DataHandler : public QObject
{
    Q_OBJECT
public:
    explicit DataHandler(QObject *parent = 0);
    ~DataHandler();
    inline QStandardItemModel *getDeviceTreeModel(){return mModelTree;}


    void creatTablePowerInfo(void); // 电池电量信息
    void creatTableWarningInfo(void); // 报警信息
    void creatTablePropertyInfo(void); // 报警事件类型
    void creatTableEventInfo(void); // 报警事件类型
    void creatTableBatteryDetailInfo(void); // 电池详细信息 厂家，容量等
    void creatTableZoneInfo(void); // 电池所属区域
    void creatTableGroupInfo(void); // 区域中的分组信息
    void initPropertyInfo(void);
    void initZoneAndGroup(void);

    void getRealtimeDataFromDb(void); //
    void getDeviceTree(void);
    void updateOnlineStatus(void);

    void onGroupDelete(int id);
    void onZoneDelete(int id);
    void onGroupModify(int id, int zone_from, QString name_from, int zone_to, QString name_to);
    void onZoneModify(int id, QString name_from, QString name_to);
    void onGroupAppend(int id, QString name);
    void onZoneAppend(QString name);
    void onDeviceAppend(int id, QString ip, QString mac, int address);

signals:
    void sigTreeModelUpdate(void);

public slots:

    void slotItemClicked(const QModelIndex &index);
    void slotAppendPowerInfo(QString data, bool valid);
    void timerEvent(QTimerEvent *event);

private:
    bool mDatabaseValid;

    int mCurrentClient; // 当前的设备 电脑编号
    QString mCurrentMac; // 当前的设备 mac
    int mCurrentAddress; // 当前的设备 address

    QStandardItemModel *mModelTree;  // 设备列表数据模型
    QMap<int,QTime> mOnlineTime; // 记录设备最后一次上传数据的时间
                                     // 超过3分钟没有数据就显示为离线
                                     // 超过半小时没有数据就记录一次报警信息
    QMap<int,QStandardItem *> mDeviceWithoutGroupItem;
};

#endif // DATAHANDLER_H
