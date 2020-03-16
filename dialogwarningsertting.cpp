#include "dialogwarningsertting.h"
#include "ui_dialogwarningsertting.h"
#include "warndatahandler.h"
#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStringListIterator>


DialogWarningSertting::DialogWarningSertting(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogWarningSertting)
{
    ui->setupUi(this);
    dataHandler=new WarnDataHandler(this);
    QStringList properties=dataHandler->getPropertyList();
    QStringList properties_translate;
    QStringListIterator iter(properties);
    while (iter.hasNext()){
        QString from=iter.next();
        QString to=from;
        if(from==QString("rate")){
            to=tr("rate");
        } else if(from==QString("voltage")){
            to=tr("voltage");
        } else if(from==QString("current")){
            to=tr("current");
        } else if(from==QString("volume")){
            to=tr("volume");
        } else if(from==QString("temp")){
            to=tr("temp");
        } else if(from==QString("count")){
            to=tr("count");
        } else {}
        properties_translate<<to;
    }
    ui->combxWarnType->addItems(properties_translate);

    connect(ui->combxWarnType,&QComboBox::currentTextChanged,\
            dataHandler,&WarnDataHandler::slotPropertySelected);
    connect(dataHandler,&WarnDataHandler::sigPropertySelected,
            this,&DialogWarningSertting::slotPropertySelected);
    connect(this,&DialogWarningSertting::sigPropertyModified,\
            dataHandler,&WarnDataHandler::slotPropertyModified);
    connect(ui->btnWarnConfirm,&QPushButton::clicked,\
            this,&DialogWarningSertting::on_btnWarnConfirm_clicked);
    connect(ui->checkWarnMinEnable,&QCheckBox::toggled,\
            this,&DialogWarningSertting::on_checkWarnMinEnable_toggled);
    connect(ui->checkWarnMaxEnable,&QCheckBox::toggled,\
            this,&DialogWarningSertting::on_checkWarnMaxEnable_toggled);
}

DialogWarningSertting::~DialogWarningSertting()
{
    delete ui;
}

void DialogWarningSertting::on_btnWarnConfirm_clicked(bool)
{
    QString event_property = ui->combxWarnType->currentText();
    QString event_desc = ui->editWarnDesc->text();
    float event_min = ui->editWarnMin->text().toFloat();
    bool event_min_enable = !ui->checkWarnMinEnable->isChecked();
    float event_max = ui->editWarnMax->text().toFloat();
    bool event_max_enable = !ui->checkWarnMaxEnable->isChecked();
    float event_count_tt = ui->editWarnTime->text().toFloat();
    int event_count = event_count_tt*60;
    bool event_enable = ui->checkWarnEneble->isChecked();

    if(event_enable==false) return;

    QJsonObject device_info_map;
    device_info_map["propertyName"]=event_property;
    device_info_map["desc"]=event_desc;
    device_info_map["max"]=event_max;
    device_info_map["maxEnable"]=event_max_enable;
    device_info_map["min"]=event_min;
    device_info_map["minEnable"]=event_min_enable;
    device_info_map["count"]=event_count;
    device_info_map["enable"]=event_enable;
    QJsonDocument doc(device_info_map);
    QByteArray array=doc.toJson();
    emit sigPropertyModified(array);
}

void DialogWarningSertting::on_checkWarnMinEnable_toggled(bool enable)
{
    ui->editWarnMin->setEnabled(!enable);
}

void DialogWarningSertting::on_checkWarnMaxEnable_toggled(bool enable)
{
    ui->editWarnMax->setEnabled(!enable);
}

void DialogWarningSertting::slotPropertySelected(QByteArray array)
{
    QJsonParseError errorLoad;
    QJsonDocument doc = QJsonDocument::fromJson(array,&errorLoad);
    if(QJsonParseError::NoError!=errorLoad.error){
        return;
    }

    QJsonObject obj=doc.object();
    int event_property = obj["property"].toInt();
    QString event_desc = obj["desc"].toString();
    float event_max = obj["max"].toDouble();
    bool event_max_enable = obj["maxEnable"].toBool();
    float event_min = obj["min"].toDouble();
    bool event_min_enable = obj["minEnable"].toBool();
    int event_count_tt = obj["count"].toInt();
    float event_count = event_count_tt;event_count=event_count/60.0;
    bool event_enable = obj["enable"].toBool();

    ui->editWarnDesc->setText(event_desc);
    ui->editWarnMax->setText(QString::number(event_max));
    ui->checkWarnMaxEnable->setChecked(!event_max_enable);
    ui->editWarnMin->setText(QString::number(event_min));
    ui->checkWarnMinEnable->setChecked(!event_min_enable);
    ui->editWarnTime->setText(QString::number(event_count));
    ui->checkWarnEneble->setChecked(event_enable);
}
