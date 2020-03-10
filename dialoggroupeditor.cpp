#include "dialoggroupeditor.h"
#include "ui_dialoggroupeditor.h"
#include "editordatahandler.h"
#include <QPushButton>
#include <QDebug>

DialogGroupEditor::DialogGroupEditor(int tab, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogGroupEditor)
{
    dataHandler = new EditorDataHandler(this);

    ui->setupUi(this);
    connect(ui->tabWidget,&QTabWidget::currentChanged,\
            dataHandler,&EditorDataHandler::slotTabChanged);

    ui->listViewZone->setModel((QAbstractItemModel*)(dataHandler->getModelZone()));
    ui->listViewGroupIn->setModel((QAbstractItemModel*)(dataHandler->getModelGroupIn()));
    ui->listViewGroupOut->setModel((QAbstractItemModel*)(dataHandler->getModelGroupOut()));
    connect(ui->listViewZone,&QListView::clicked,\
            dataHandler,&EditorDataHandler::slotModelZoneItemClicked);
    connect(ui->listViewGroupIn,&QListView::clicked,\
            dataHandler,&EditorDataHandler::slotModelGroupInItemClicked);
    connect(ui->listViewGroupOut,&QListView::clicked,\
            dataHandler,&EditorDataHandler::slotModelGroupOutItemClicked);
    connect(ui->btnGroupIn,&QPushButton::clicked,\
            dataHandler,&EditorDataHandler::onGroupIn);
    connect(ui->btnGroupOut,&QPushButton::clicked,\
            dataHandler,&EditorDataHandler::onGroupOut);

    ui->listViewGroup->setModel((QAbstractItemModel*)(dataHandler->getModelGroup()));
    ui->listViewDeviceIn->setModel((QAbstractItemModel*)(dataHandler->getModelDeviceIn()));
    ui->listViewDeviceOut->setModel((QAbstractItemModel*)(dataHandler->getModelDeviceOut()));
    connect(ui->listViewGroup,&QListView::clicked,\
            dataHandler,&EditorDataHandler::slotModelGroupItemClicked);
    connect(ui->listViewDeviceIn,&QListView::clicked,\
            dataHandler,&EditorDataHandler::slotModelDeviceInItemClicked);
    connect(ui->listViewDeviceOut,&QListView::clicked,\
            dataHandler,&EditorDataHandler::slotModelDeviceOutItemClicked);
    connect(ui->btnDeviceIn,&QPushButton::clicked,\
            dataHandler,&EditorDataHandler::onDeviceIn);
    connect(ui->btnDeviceOut,&QPushButton::clicked,\
            dataHandler,&EditorDataHandler::onDeviceOut);

    ui->tabWidget->setCurrentIndex(tab);
    dataHandler->slotTabChanged(tab);
}

DialogGroupEditor::~DialogGroupEditor()
{
    delete ui;
}

void DialogGroupEditor::on_btnZoneAdd_clicked(bool)
{
    dataHandler->onZoneAppend(ui->editZoneName->text());
}

void DialogGroupEditor::on_btnZoneModify_clicked(bool)
{
    dataHandler->onZoneModify(ui->editZoneNameOld->text(),\
                              ui->editZoneNameNew->text());
}

void DialogGroupEditor::on_btnZoneDelete_clicked(bool)
{
    dataHandler->onZoneDelete();
}

void DialogGroupEditor::on_btnGroupAdd_clicked(bool)
{
    dataHandler->onGroupAppend(ui->editGroupName->text());
}

void DialogGroupEditor::on_btnGroupModify_clicked(bool)
{
    dataHandler->onGroupModify(ui->editGroupNameOld->text(),\
                               ui->editGroupNameNew->text());
}

void DialogGroupEditor::on_btnGroupDelete_clicked(bool)
{
    dataHandler->onGroupDelete();
}
