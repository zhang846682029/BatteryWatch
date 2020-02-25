#ifndef DATAHANDLER_H
#define DATAHANDLER_H

#include <QObject>

class DataHandler : public QObject
{
    Q_OBJECT
public:
    explicit DataHandler(QObject *parent = 0);
    ~DataHandler();

    void creatTablePowerInfo(void); // 电池电量信息
    void creatTableWarningInfo(void); // 报警信息
    void creatTablePropertyInfo(void); // 报警事件类型
    void creatTableEventInfo(void); // 报警事件类型
    void creatTableBatteryDetailInfo(void); // 电池详细信息 厂家，容量等
    void creatTableZoneInfo(void); // 电池所属区域
    void creatTableGroupInfo(void); // 区域中的分组信息
    void initPropertyInfo(void);

    void getRealtimeDataFromDb(void); //

signals:

public slots:
    void slotAppendPowerInfo(QString data, int len);
    void timerEvent(QTimerEvent *event);

private:
    bool mDatabaseValid;

    QString mCurrentMac;
    int mCurrentAddress;
};

#endif // DATAHANDLER_H
