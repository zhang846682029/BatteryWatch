#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "server.h"
#include "datahandler.h"
#include "dialoggroupeditor.h"
#include "dialogwarningsertting.h"
#include "dialogwarningmore.h"
#include "warninghandler.h"
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
    connect(ui->actionDeviceEditor,&QAction::triggered,\
            this,&MainWindow::slotActionDeviceEditortriggered,\
            Qt::UniqueConnection);
    connect(ui->actionEventEditor,&QAction::triggered,\
            this,&MainWindow::slotActionWarnEditorTriggered,\
            Qt::UniqueConnection);
    connect(ui->actionEventMore,&QAction::triggered,\
            this,&MainWindow::slotActionWarnMoreTriggered,\
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
    connect(mDataHandler,&DataHandler::sigUpdatePowerInfo,\
            ui->widget,&BatteryInfoWidget::slotUpdatePowerInfo);

    // 设备树
    ui->treeViewDevice->setModel(mDataHandler->getModelDeviceTree());
    ui->treeViewDevice->expandAll();
    // 右键菜单
    ui->treeViewDevice->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->treeViewDevice, &QTreeView::customContextMenuRequested, \
            this, &MainWindow::slotTreeMenu);
    connect(ui->treeViewDevice,&QTreeView::clicked,\
            mDataHandler,&DataHandler::slotItemClicked);
    connect(mDataHandler,&DataHandler::sigTreeModelUpdate,\
            this,&MainWindow::slotTreeModeUpdate);

    ui->tableViewDeviceDetail->setModel(mDataHandler->getModelDeviceInfo());
    ui->tableViewDeviceDetail->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableViewDeviceDetail->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableViewDeviceDetail->setSelectionMode(QAbstractItemView::SingleSelection);

    // Event/Warn
    mWarningHandler = new WarningHandler(this);
    ui->tableViewWarnningInfo->setModel(mWarningHandler->getModelWarningInfo());
    ui->tableViewWarnningInfo->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tableViewWarnningInfo->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableViewWarnningInfo->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableViewWarnningInfo->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tableViewWarnningInfo, &QTreeView::customContextMenuRequested, \
            this, &MainWindow::slotWarnMenu);
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
    menu->addAction(ui->actionDeviceEditor);
    menu->exec(ui->treeViewDevice->mapToGlobal(pos));
    delete menu;
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

void MainWindow::slotActionDeviceEditortriggered(bool)
{
    DialogGroupEditor *dlg_editor=new DialogGroupEditor(2,this);
    dlg_editor->exec();
    delete dlg_editor;
    mDataHandler->getDeviceTree();
    mDataHandler->updateDeviceDetail();
}

void MainWindow::slotWarnMenu(const QPoint &pos)
{
    QMenu *menu = new QMenu(ui->tableViewWarnningInfo);
    menu->addAction(ui->actionEventEditor);
    menu->addAction(ui->actionEventMore);
    menu->exec(ui->tableViewWarnningInfo->mapToGlobal(pos));
    delete menu;
}

void MainWindow::slotActionWarnEditorTriggered(bool)
{
    DialogWarningSertting *dlg = new DialogWarningSertting(this);
    dlg->exec();
    delete dlg;
    mWarningHandler->initEventInfo();
}

void MainWindow::slotActionWarnMoreTriggered(bool)
{
    DialogWarningMore *dlg = new DialogWarningMore(this);
    dlg->exec();
    delete dlg;
}
