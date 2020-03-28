#include "dialoggroupeditor.h"
#include "ui_dialoggroupeditor.h"
#include "editordatahandler.h"
#include <QPushButton>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDateEdit>
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

    //ui->combxDevice
    connect(ui->combxDevice,&QComboBox::currentTextChanged,\
            dataHandler,&EditorDataHandler::slotDeviceDetailSelected);
    connect(dataHandler,&EditorDataHandler::sigDetailSelected,\
            this,&DialogGroupEditor::slotDetailSelected);
    connect(this,&DialogGroupEditor::sigDetailModified,\
            dataHandler,&EditorDataHandler::slotDetailModified);

    ui->tabWidget->setCurrentIndex(tab);
    dataHandler->slotTabChanged(tab);
    if(tab==2){
        ui->combxDevice->addItems(dataHandler->getDeviceList());
    }
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

void DialogGroupEditor::on_btnDetailInfo_clicked(bool)
{
    QJsonObject device_info_map;
    device_info_map["clientId"]=ui->combxDevice->currentText();
    device_info_map["manufacturer"]=ui->editManufacturer->text();
    device_info_map["serialNumber"]=ui->editSerialNumber->text();
    device_info_map["installationDate"]=ui->dateTimeEdit->dateTime()\
            .toString("yyyy-MM-dd hh:mm:ss");
    device_info_map["batteryVolume"]=ui->editBatteryVolume->text();
    QJsonDocument doc(device_info_map);
    QByteArray array=doc.toJson();
    emit sigDetailModified(array);
}

void DialogGroupEditor::slotDetailSelected(QByteArray array)
{
    QJsonParseError errorLoad;
    QJsonDocument doc = QJsonDocument::fromJson(array,&errorLoad);
    if(QJsonParseError::NoError!=errorLoad.error){
        return;
    }

    QJsonObject obj=doc.object();
    QString device_manufacturer = obj["manufacturer"].toString();
    ui->editManufacturer->setText(device_manufacturer);

    QString device_sn = obj["serialNumber"].toString();
    ui->editSerialNumber->setText(device_sn);

    QString device_install_string = obj["installationDate"].toString();
    if(device_install_string.isEmpty()){
        QDateTime install_time=QDateTime::fromString(QString("1970-01-01 08:00:00"),\
                                                     QString("yyyy-MM-dd hh:mm:ss"));
        ui->dateTimeEdit->setDateTime(QDateTime::currentDateTime());
    } else {
        QDateTime install_time=QDateTime::fromString(device_install_string,\
                                                     QString("yyyy-MM-dd hh:mm:ss"));
        ui->dateTimeEdit->setDateTime(install_time);
    }

    QString device_volume_string = obj["batteryVolume"].toString();
    ui->editBatteryVolume->setText(device_volume_string);

}
