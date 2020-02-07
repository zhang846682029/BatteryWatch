#include "mainwindow.h"
#include "batteryinfowidget.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    this->resize(640,480);
    QPalette pal(this->palette());
    pal.setColor(QPalette::Background, QColor(60,60,60,200));
    this->setAutoFillBackground(true);
    this->setPalette(pal);
//    setStyleSheet("background-color:#282828;");

    BatteryInfoWidget *infoWidget = new BatteryInfoWidget();
    this->setCentralWidget(infoWidget);
}

MainWindow::~MainWindow()
{

}
