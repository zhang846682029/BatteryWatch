#ifndef WARNINGHANDLER_H
#define WARNINGHANDLER_H

#include <QObject>
#include <QVector>
#include <QMap>

class QStandardItemModel;

/**
 * @brief 监控异常数据
 */
class WarningHandler : public QObject
{
    Q_OBJECT
public:
    explicit WarningHandler(QObject *parent = 0);
    void initEventInfo(void);
    inline QStandardItemModel *getModelWarningInfo(){return mModelWarningInfo;}

protected:
    void timerEvent(QTimerEvent *event);

private:
    typedef struct {
        int property;
        QString event_desc;
        float event_max;
        bool event_max_enable;
        float event_min;
        bool event_min_enable;
        int event_count; // time count*60 seconds
        int event_enable;
    } EventInfo;

    void get_recent_powerinfo(void);
    bool is_property_warning(int type, float value);
    void update_warning_info(int client, int *flags);
    void update_warning_offline_info(QMap<int,bool> &online);

    void update_warning_model(void);

    int mMaxSeconds; // 每个属性有个时间  取最大值 根据这个最大值取查询记录
    EventInfo mEventInfoRate;
    EventInfo mEventInfoVoltage;
    EventInfo mEventInfoCurrent;
    EventInfo mEventInfoVolume;
    EventInfo mEventInfoTemp;
    EventInfo mEventInfoCount;// 寿命
    EventInfo mEventInfoConnect;// 离线报警
    QMap<int,QString> mEventDesc;

    QStandardItemModel *mModelWarningInfo;
};

#endif // WARNINGHANDLER_H
