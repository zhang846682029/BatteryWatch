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

protected:
    void paintEvent(QPaintEvent *event);
};

#endif // BATTERYINFOWIDGET_H
