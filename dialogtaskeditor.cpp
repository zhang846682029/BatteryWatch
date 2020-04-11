#include "dialogtaskeditor.h"
#include "ui_dialogtaskeditor.h"
#include <QSqlQuery>
#include <QDateTime>
#include <QDebug>

DialogTaskEditor::DialogTaskEditor(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogTaskEditor)
{
    ui->setupUi(this);
    connect(ui->combxTaskNum,&QComboBox::currentTextChanged,\
            this,&DialogTaskEditor::on_combxTask_changed);
    init_task_list();
}

DialogTaskEditor::~DialogTaskEditor()
{
    delete ui;
}

void DialogTaskEditor::on_btnTaskModify_clicked(bool)
{
    int num = ui->combxTaskNum->currentText().toInt();
    QString task_reason = ui->textEditTask->toPlainText();
    QString task_result = ui->textEditResult->toPlainText();
    QString engineer_name = ui->lineEditName->text();
    QString engineer_phone = ui->lineEditPhone->text();
    int task_state=0;
    if(ui->radioBtnBegin->isChecked()) task_state=1;
    if(ui->radioBtnFinish->isChecked()) task_state=2;

    QSqlQuery query;
    QString sql = QString("UPDATE taskInfo SET task_reason='%1',"
                          "task_result='%2',engineer_name='%3',engineer_phone='%4',"
                          "task_state=%5 WHERE num=%6;").arg(task_reason).arg(task_result)
            .arg(engineer_name).arg(engineer_phone).arg(task_state).arg(num);
    bool r1=query.exec(sql);
    qDebug()<<__FUNCTION__<<r1<<sql;
}

void DialogTaskEditor::on_combxTask_changed(QString task)
{
    int num = task.toInt();
    QSqlQuery query;
    QString sql = QString("SELECT * FROM taskInfo WHERE NOT task_state=2 AND num=%1").arg(num);
    bool r1=query.exec(sql);
    qDebug()<<__FUNCTION__<<r1<<sql;
    if(false==r1){
        return;
    }

    query.next();
    QDateTime task_time=query.value(2).toDateTime();
    ui->lineEditTime->setText(task_time.toString("yyyy-MM-dd hh:mm:ss"));
    ui->textEditTask->setText(query.value(3).toString());
    ui->lineEditName->setText(query.value(4).toString());
    ui->lineEditPhone->setText(query.value(5).toString());
    ui->textEditResult->setText(query.value(6).toString());
    int task_state = query.value(7).toInt();
    if(0==task_state) ui->radioBtnOpen->setChecked(true);
    if(1==task_state) ui->radioBtnBegin->setChecked(true);
    if(2==task_state) ui->radioBtnFinish->setChecked(true);

    int battery = query.value(1).toInt();
    QString battery_info = QString::number(battery);
    ui->lineEditBattery->setText(battery_info);

    sql = QString("SELECT groupId FROM batteryDetailInfo WHERE clientId=%1;").arg(battery);
    r1=query.exec(sql);
    qDebug()<<__FUNCTION__<<r1<<sql;
    if(r1==false){
        return;
    }
    query.next();
    int group=query.value(0).toInt();

    sql = QString("SELECT * FROM groupInfo WHERE id=%1;").arg(group);
    r1=query.exec(sql);
    qDebug()<<__FUNCTION__<<r1<<sql;
    if(r1==false){
        return;
    }
    query.next();
    int zone=query.value(2).toInt();
    QString group_name = query.value(1).toString();
    battery_info.append(QString("-")+group_name);
    ui->lineEditBattery->setText(battery_info);

    sql = QString("SELECT name FROM zoneInfo WHERE id=%1;").arg(zone);
    r1=query.exec(sql);
    qDebug()<<__FUNCTION__<<r1<<sql;
    if(r1==false){
        return;
    }
    query.next();
    QString zone_name = query.value(0).toString();
    battery_info.append(QString("-")+zone_name);
    ui->lineEditBattery->setText(battery_info);
}

void DialogTaskEditor::init_task_list()
{
    QSqlQuery query;
    QString sql = QString("SELECT num FROM taskInfo WHERE NOT task_state=2;");
    bool r1 = query.exec(sql);
    qDebug()<<r1<<sql;

    QStringList task_num_list;
    while(query.next()){
        int num=query.value(0).toInt();
        task_num_list<<QString::number(num);
    }

    ui->combxTaskNum->clear();
    ui->combxTaskNum->addItems(task_num_list);
}
