#ifndef BATTERYINFOWIDGET_H
#define BATTERYINFOWIDGET_H

#include <QWidget>

class BatteryInfoWidget : public QWidget
{
    Q_OBJECT
public:
    explicit BatteryInfoWidget(QWidget *parent = 0);
    void draw(QPainter *painter);

signals:

public slots:
    void slotUpdatePowerInfo(QMap<QString,int> &info);

protected:
    void paintEvent(QPaintEvent *event);

private:
    int mDeviceId;
    int mDeviceRate;
    float mDeviceVoltage;
    float mDeviceCurrent;
    float mDeviceVolume;
    float mDeviceTemp;
    int mDeviceDirection;
    int mDeviceCount;
    int mDeviceAlarm;
};

#endif // BATTERYINFOWIDGET_H
