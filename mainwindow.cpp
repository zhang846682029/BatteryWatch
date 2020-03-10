#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "server.h"
#include "datahandler.h"
#include "dialoggroupeditor.h"
#include <QStandardItemModel>
#include <QHeaderView>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QAction>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->actionZoneEditor,&QAction::triggered,\
            this,&MainWindow::slotActionZoneEditortriggered,\
            Qt::UniqueConnection);
    connect(ui->actionGroupEditor,&QAction::triggered,\
            this,&MainWindow::slotActionGroupEditortriggered,\
            Qt::UniqueConnection);

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
    menu->addAction(ui->actionZoneEditor);
    menu->addAction(ui->actionGroupEditor);
    menu->exec(ui->treeViewDevice->mapToGlobal(pos));
}

void MainWindow::slotActionZoneEditortriggered(bool)
{
    DialogGroupEditor *dlg_editor=new DialogGroupEditor(0,this);
    dlg_editor->exec();
    delete dlg_editor;
    mDataHandler->getDeviceTree();
}

void MainWindow::slotActionGroupEditortriggered(bool)
{
    DialogGroupEditor *dlg_editor=new DialogGroupEditor(1,this);
    dlg_editor->exec();
    delete dlg_editor;
    mDataHandler->getDeviceTree();
}
