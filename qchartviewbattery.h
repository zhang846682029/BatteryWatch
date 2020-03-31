#ifndef QCHARTVIEWBATTERY_H
#define QCHARTVIEWBATTERY_H

#include <QtCharts>
#include <QChartView>

using namespace QtCharts;

class QChartViewBattery : public QChartView
{
public:
    QChartViewBattery(QWidget *parent = Q_NULLPTR);
    void getRecordAndShow(int battery, int property, QDate &begin, QDate &end);

private:
    void update_chart(int type, QMap<float, float> &record, QDate &begin, QDate &end);

};

#endif // QCHARTVIEWBATTERY_H
