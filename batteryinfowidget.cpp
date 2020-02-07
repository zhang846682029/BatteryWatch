#include "batteryinfowidget.h"
#include <QPainter>
#include <QtMath>
#include <QDebug>

BatteryInfoWidget::BatteryInfoWidget(QWidget *parent) : QWidget(parent)
{
    update();
}

void BatteryInfoWidget::draw(QPainter *painter)
{
    painter->setPen(Qt::white);
    painter->drawLine(0,40,640,40);
    const QRect rectangle = QRect(0, 0, 640, 40);
    painter->drawText(rectangle, Qt::AlignCenter, tr("Battery Info"));

    painter->drawLine(0,440,640,440);
    const QRect rectangle1 = QRect(0, 440, 640, 40);
    painter->drawText(rectangle1, Qt::AlignCenter, \
                      tr("ADDRESS: %1    MAC: %2    STATUS: %3")
                      .arg(1).arg("41:50:F0:55:47:8E").arg("discharge"));

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
    double powerPercent = 177.5;
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
    double powerLeft = 35.0;
    powerLeft = qBound(0.0,powerLeft,50.0);
    painter->setBrush(Qt::red);
    painter->setPen(Qt::red);
    painter->rotate(powerLeft/5*24-120);
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
                      20,12,Qt::AlignCenter,QString::number(50));
    painter->drawText(-10,-35,\
                      20,12,Qt::AlignCenter,QString::number(25));
    painter->translate(-230,-160);

    // 当前电压
    QRadialGradient gradient2(410,160,50,410,185);
    gradient2.setColorAt(0.0,Qt::darkGray);
    gradient2.setColorAt(1.0,QColor(40,40,40));
    painter->setBrush(gradient2);
    painter->drawEllipse(360,110,100,100);

    // 电池温度
    QRadialGradient gradient3(530,290,100,530,340);
    gradient3.setColorAt(0.0,Qt::darkGray);
    gradient3.setColorAt(1.0,QColor(40,40,40));
    painter->setBrush(gradient3);
    painter->drawEllipse(430,190,200,200);
}

void BatteryInfoWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing,true);
    painter.setWindow(0,0,640,480);
    draw(&painter);
}
