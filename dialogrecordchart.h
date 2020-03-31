#ifndef DIALOGRECORDCHART_H
#define DIALOGRECORDCHART_H

#include <QDialog>

namespace Ui {
class DialogRecordChart;
}

class DialogRecordChart : public QDialog
{
    Q_OBJECT

public:
    explicit DialogRecordChart(QWidget *parent = 0);
    ~DialogRecordChart();
    void initProperty(void);
    void initBattery(void);

public slots:
    void on_btnQuery_clicked(bool);

private:
    Ui::DialogRecordChart *ui;
};

#endif // DIALOGRECORDCHART_H
