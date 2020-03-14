#include "batteryinfowidget.h"
#include <QPainter>
#include <QtMath>
#include <QDebug>

const double MAXVOLUME = 50.0;
const double MAXVOLTAGE = 50.0;

BatteryInfoWidget::BatteryInfoWidget(QWidget *parent) : QWidget(parent)
{
    mDeviceId=0;
    mDeviceRate=0;
    mDeviceVoltage=0.0;
    mDeviceCurrent=0.0;
    mDeviceVolume=0.0;
    mDeviceTemp=0.0;
    mDeviceDirection=2;
    mDeviceCount=0;
    mDeviceAlarm=0;

    update();
}

void BatteryInfoWidget::draw(QPainter *painter)
{
    painter->setPen(Qt::white);
    painter->drawRect(0,0,640,480);

    painter->drawLine(0,40,640,40);
    const QRect rectangle = QRect(0, 0, 640, 40);
    painter->drawText(rectangle, Qt::AlignCenter, tr("Battery Info"));

    painter->drawLine(0,440,640,440);
    const QRect rectangle1 = QRect(0, 440, 640, 40);
    QStringList power_status;
    power_status<<tr("Discharge")<<tr("Charge")<<tr("Idle");
    painter->drawText(rectangle1, Qt::AlignCenter, \
                      tr("ADDRESS: %1    MAC: %2    STATUS: %3")
                      .arg(mDeviceId,3,10,QChar('0')).arg("41:50:F0:55:47:8E")
                      .arg(power_status.at(mDeviceDirection)));

    // 电池容量百分比
    painter->translate(110,290);
    QRadialGradient gradient(0,0,100,0,50);
    gradient.setColorAt(0.0,Qt::darkGray);
    gradient.setColorAt(1.0,QColor(40,40,40));
    painter->setBrush(gradient);
    painter->drawEllipse(-100,-100,200,200);
    painter->setBrush(Qt::black);
    painter->drawEllipse(-12,-12,24,24);
    for(int n=0;n<=20;n++)
    {
        painter->save();
        painter->rotate(n*12-120);
        if(n%2==0)
        {
            painter->drawLine(0,-95,0,-91);
            painter->drawText(-12,-90,24,24,Qt::AlignCenter,QString::number(n*5));
        }
        else
        {
            painter->drawLine(0,-95,0,-91);
        }
        painter->restore();
    }

    const QPointF points[3] = {
          QPointF(0.0, -95.0),
          QPointF(-2.0, 15.0),
          QPointF(2.0, 15.0),
      };
    painter->save();
    double powerPercent = mDeviceRate;
    powerPercent = qBound(0.0,powerPercent,100.0);
    painter->setBrush(Qt::red);
    painter->setPen(Qt::red);
    painter->rotate(powerPercent/5*12-120);
    painter->drawPolygon(points,3);
    painter->restore();
    painter->drawText(-30,30,60,40,Qt::AlignCenter,tr("Percent\n%1\%").arg(powerPercent));
    painter->translate(-110,-290);

    // 电池剩余容量
    painter->translate(230,160);
    QRadialGradient gradient1(0,0,50,0,25);
    gradient1.setColorAt(0.0,Qt::darkGray);
    gradient1.setColorAt(1.0,QColor(40,40,40));
    painter->setBrush(gradient1);
    painter->drawEllipse(-50,-50,100,100);
    painter->setBrush(Qt::black);
    painter->drawEllipse(-5,-5,10,10);
    for(int n=0;n<=10;n++)
    {
        painter->save();
        painter->rotate(n*24-120);
            painter->drawLine(0,-45,0,-41);
//            painter->drawText(-12,-40,24,24,Qt::AlignCenter,QString::number(n*5));
        painter->restore();
    }
    const QPointF points1[3] = {
          QPointF(0.0, -45.0),
          QPointF(-2.0, 7.0),
          QPointF(2.0, 7.0),
      };
    painter->save();
    double powerLeft = mDeviceVolume;
    powerLeft = qBound(0.0,powerLeft,MAXVOLUME);
    painter->setBrush(Qt::red);
    painter->setPen(Qt::red);
    painter->rotate(powerLeft/(MAXVOLUME/10)*24-120);
    painter->drawPolygon(points1,3);
    painter->restore();
    QFont font1;
    font1.setPointSize(8);
    painter->setFont(font1);
    painter->drawText(-15,15,30,20,Qt::AlignCenter,tr("AH"));
    font1.setPointSize(6);
    painter->setFont(font1);
    painter->drawText(45*qSin(qDegreesToRadians(-120.0))+5,-45*qCos(qDegreesToRadians(-120.0))-5,\
                      10,12,Qt::AlignCenter,QString::number(0));
    painter->drawText(45*qSin(qDegreesToRadians(120.0))-20,-45*qCos(qDegreesToRadians(120.0))-5,\
                      20,12,Qt::AlignCenter,QString::number(MAXVOLUME));
    painter->drawText(-10,-35,\
                      20,12,Qt::AlignCenter,QString::number(MAXVOLUME/2));
    painter->translate(-230,-160);

    // 当前电压
//    QRadialGradient gradient2(410,160,50,410,185);
//    gradient2.setColorAt(0.0,Qt::darkGray);
//    gradient2.setColorAt(1.0,QColor(40,40,40));
//    painter->setBrush(gradient2);
//    painter->drawEllipse(360,110,100,100);
    painter->translate(410,160);
    QRadialGradient gradient2(0,0,50,0,25);
    gradient2.setColorAt(0.0,Qt::darkGray);
    gradient2.setColorAt(1.0,QColor(40,40,40));
    painter->setBrush(gradient2);
    painter->drawEllipse(-50,-50,100,100);
    painter->setBrush(Qt::black);
    painter->drawEllipse(-5,-5,10,10);
    for(int n=0;n<=10;n++)
    {
        painter->save();
        painter->rotate(n*24-120);
            painter->drawLine(0,-45,0,-41);
//            painter->drawText(-12,-40,24,24,Qt::AlignCenter,QString::number(n*5));
        painter->restore();
    }
    const QPointF points2[3] = {
          QPointF(0.0, -45.0),
          QPointF(-2.0, 7.0),
          QPointF(2.0, 7.0),
      };
    painter->save();
    double voltageLeft = mDeviceVoltage;
    voltageLeft = qBound(0.0,voltageLeft,MAXVOLTAGE);
    painter->setBrush(Qt::red);
    painter->setPen(Qt::red);
    painter->rotate(voltageLeft/(MAXVOLTAGE/10)*24-120);
    painter->drawPolygon(points2,3);
    painter->restore();
    QFont font2;
    font2.setPointSize(8);
    painter->setFont(font2);
    painter->drawText(-15,15,30,20,Qt::AlignCenter,tr("V"));
    font2.setPointSize(6);
    painter->setFont(font2);
    painter->drawText(45*qSin(qDegreesToRadians(-120.0))+5,-45*qCos(qDegreesToRadians(-120.0))-5,\
                      10,12,Qt::AlignCenter,QString::number(0));
    painter->drawText(45*qSin(qDegreesToRadians(120.0))-20,-45*qCos(qDegreesToRadians(120.0))-5,\
                      20,12,Qt::AlignCenter,QString::number(MAXVOLTAGE));
    painter->drawText(-10,-35,\
                      20,12,Qt::AlignCenter,QString::number(MAXVOLTAGE/2));
    painter->translate(-410,-160);

    // 电池温度
//    QRadialGradient gradient3(530,290,100,530,340);
//    gradient3.setColorAt(0.0,Qt::darkGray);
//    gradient3.setColorAt(1.0,QColor(40,40,40));
//    painter->setBrush(gradient3);
//    painter->drawEllipse(430,190,200,200);
    painter->translate(530,290);
    QRadialGradient gradient3(0,0,100,0,50);
    gradient3.setColorAt(0.0,Qt::darkGray);
    gradient3.setColorAt(1.0,QColor(40,40,40));
    painter->setBrush(gradient3);
    painter->drawEllipse(-100,-100,200,200);
    painter->setBrush(Qt::black);
    painter->drawEllipse(-12,-12,24,24);
    for(int n=0;n<=30;n++)
    {
        painter->save();
        painter->rotate(n*8-120);
        if(n%2==0)
        {
            painter->drawLine(0,-95,0,-91);
            painter->drawText(-12,-90,24,24,Qt::AlignCenter,QString::number(n*5-50));
        }
        else
        {
            painter->drawLine(0,-95,0,-91);
        }
        painter->restore();
    }

    const QPointF points3[3] = {
          QPointF(0.0, -95.0),
          QPointF(-2.0, 15.0),
          QPointF(2.0, 15.0),
      };
    painter->save();
    double temperature = mDeviceTemp;
    temperature = qBound(-50.0,temperature,100.0);
    painter->setBrush(Qt::red);
    painter->setPen(Qt::red);
    painter->rotate((temperature+50)/5*8-120);
    painter->drawPolygon(points3,3);
    painter->restore();
    QFont font3;
    font3.setPointSize(8);
    painter->setFont(font3);
    painter->drawText(-30,30,60,40,Qt::AlignCenter,tr("Temp\n%1°C").arg(temperature));
    painter->translate(-530,-290);

    // 文字显示区
//    QLinearGradient gradent4(240,260,240,380);
//    gradent4.setColorAt(0.0,QColor(40,40,40));
//    gradent4.setColorAt(0.5,Qt::darkGray);
//    gradent4.setColorAt(1.0,QColor(40,40,40));
//    painter->setBrush(gradent4);
//    painter->setPen(Qt::NoPen);
//    painter->drawRect(240,260,160,120);
    double current = mDeviceCurrent;
    double cycleCount = mDeviceCount;
    painter->translate(240,240);
    painter->drawText(0,0,160,20,Qt::AlignCenter,tr("Percent %1 \%").arg(powerPercent));
    painter->drawText(0,20,160,20,Qt::AlignCenter,tr("Voltage %1 V").arg(voltageLeft));
    painter->drawText(0,40,160,20,Qt::AlignCenter,tr("Current %1 A").arg(current));
    painter->drawText(0,60,160,20,Qt::AlignCenter,tr("Volume %1 AH").arg(powerLeft));
    painter->drawText(0,80,160,20,Qt::AlignCenter,tr("Temp %1 °C").arg(temperature));
    painter->drawText(0,100,160,20,Qt::AlignCenter,tr("Count %1").arg(cycleCount));
    painter->drawLine(10,20,150,20);
    painter->drawLine(10,40,150,40);
    painter->drawLine(10,60,150,60);
    painter->drawLine(10,80,150,80);
    painter->drawLine(10,100,150,100);
    painter->drawLine(10,120,150,120);
    painter->translate(-240,-240);
}

void BatteryInfoWidget::slotUpdatePowerInfo(QMap<QString, int> &info)
{
    mDeviceId=info["clientId"];
    mDeviceRate=info["rate"];
    int mDeviceVoltage_tt=info["voltage"];
    int mDeviceCurrent_tt=info["current"];
    int mDeviceVolume_tt=info["volume"];
    int mDeviceTemp_tt=info["temp"];
    mDeviceDirection=info["direction"];
    mDeviceCount=info["count"];
    mDeviceAlarm=info["alarm"];

    mDeviceVoltage=mDeviceVoltage_tt;mDeviceVoltage=mDeviceVoltage/100.0;
    mDeviceCurrent=mDeviceCurrent_tt;mDeviceCurrent=mDeviceCurrent/100.0;
    mDeviceVolume=mDeviceVolume_tt;mDeviceVolume=mDeviceVolume/10.0;
    mDeviceTemp=mDeviceTemp_tt;mDeviceTemp=mDeviceTemp/10.0;

    update();
}

void BatteryInfoWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing,true);
    painter.setWindow(0,0,640,480);
    draw(&painter);
}
