#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "server.h"
#include "datahandler.h"
#include <QStandardItemModel>
#include <QHeaderView>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonParseError>

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

    mTcpServer = new Server(this,7869);
    mDataHandler = new DataHandler(this);
    connect(mTcpServer, &Server::sigPowerInfo, \
            mDataHandler, &DataHandler::slotAppendPowerInfo);

    // 设备树
    ui->treeViewDevice->setModel(mDataHandler->getDeviceTreeModel());
    ui->treeViewDevice->expandAll();
    // 右键菜单
    ui->treeViewDevice->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->treeViewDevice, &QTreeView::customContextMenuRequested, \
            this, &MainWindow::slotTreeMenu);
    connect(ui->treeViewDevice,&QTreeView::clicked,\
            mDataHandler,&DataHandler::slotItemClicked);
    connect(mDataHandler,&DataHandler::sigTreeModelUpdate,\
            this,&MainWindow::slotTreeModeUpdate);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::slotTreeModeUpdate()
{
    ui->treeViewDevice->expandAll();
}

void MainWindow::slotTreeMenu(const QPoint &pos)
{
    QMenu *menu = new QMenu(ui->treeViewDevice);
    QAction *a1=new QAction(tr("Group Editor"));
    menu->addAction(a1);
    QAction *a2=new QAction(tr("Device Editor"));
    menu->addAction(a2);
    menu->exec(ui->treeViewDevice->mapToGlobal(pos));
}
