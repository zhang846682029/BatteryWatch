#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "server.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

        QPalette pal(this->palette());
        pal.setColor(QPalette::Background, QColor(60,60,60,200));
        this->setAutoFillBackground(true);
        this->setPalette(pal);
    //    setStyleSheet("background-color:#282828;");

   Server *tcpServer = new Server(this,7869);
   // todo 处理网络数据
   // connect(tcpServer, &Server::updateserver,NULL, NULL);

}

MainWindow::~MainWindow()
{
    delete ui;
}
