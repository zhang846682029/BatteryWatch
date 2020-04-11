#include "dialogtaskmore.h"
#include "ui_dialogtaskmore.h"
#include <QDate>
#include <QStandardItemModel>
#include <QSqlQuery>
#include <QDebug>

DialogTaskMore::DialogTaskMore(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogTaskMore)
{
    ui->setupUi(this);

    ui->dateEditBegin->setDate(QDate::currentDate());
    ui->dateEditEnd->setDate(QDate::currentDate());

    mTaskModel=new QStandardItemModel();
    QStringList headers;
    headers<<tr("num")<<tr("battery")<<tr("time")<<tr("state")<<tr("desc")
             <<tr("result")<<tr("name")<<tr("phone");
    mTaskModel->setHorizontalHeaderLabels(headers);
    ui->tableView->setModel(mTaskModel);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);

//    connect(ui->comboBox,&QComboBox::currentIndexChanged,this,&DialogTaskMore::slot_state_changed);
    connect(ui->comboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(slot_state_changed(int)));
}

DialogTaskMore::~DialogTaskMore()
{
    delete ui;
}

void DialogTaskMore::on_btnQuery_clicked(bool)
{
    mTaskModel->rowCount();
    mTaskModel->removeRows(0,mTaskModel->rowCount());

    QSqlQuery query;
    QString sql = QString("SELECT * FROM taskInfo WHERE task_time "
                          "BETWEEN strftime('%Y-%m-%d','%2','start of day') "
                          "AND strftime('%Y-%m-%d','%3','1 day','start of day')")
            .arg(ui->dateEditBegin->date().toString("yyyy-MM-dd"))
            .arg(ui->dateEditEnd->date().toString("yyyy-MM-dd"));
    if(ui->comboBox->currentIndex()<3){
        sql = QString("SELECT * FROM taskInfo WHERE task_state=%1 AND task_time "
                      "BETWEEN strftime('%Y-%m-%d','%2','start of day') "
                      "AND strftime('%Y-%m-%d','%3','1 day','start of day')")
                .arg(ui->comboBox->currentIndex())
                .arg(ui->dateEditBegin->date().toString("yyyy-MM-dd"))
                .arg(ui->dateEditEnd->date().toString("yyyy-MM-dd"));
    }
    bool r1 = query.exec(sql);
    qDebug()<<__FUNCTION__<<r1<<sql;

    int row=0;
    while(query.next()){
        int num = query.value(0).toInt();
        int client = query.value(1).toInt();
        QDateTime task_time = query.value(2).toDateTime();
        QString task_desc = query.value(3).toString();
        QString engineer_name = query.value(4).toString();
        QString engineer_phone = query.value(5).toString();
        QString task_result = query.value(6).toString();
        int state = query.value(7).toInt();
        qDebug()<<__FUNCTION__<<task_time.toString("yyyy-MM-dd hh:mm:ss");

        QStandardItem *item_num = new QStandardItem(QString::number(num));
        QStandardItem *item_client = new QStandardItem(QString::number(client));
        QStandardItem *item_time = new QStandardItem(task_time.toString("yyyy-MM-dd hh:mm:ss"));
        QStandardItem *item_desc = new QStandardItem(task_desc);
        QStandardItem *item_name = new QStandardItem(engineer_name);
        QStandardItem *item_phone = new QStandardItem(engineer_phone);
        QStandardItem *item_result = new QStandardItem(task_result);
        QString state_str = tr("task open");
        if(state==1) state_str = tr("task begin");
        if(state==2) state_str = tr("task finish");
        QStandardItem *item_state = new QStandardItem(state_str);
        item_num->setEditable(false);
        item_client->setEditable(false);
        item_time->setEditable(false);
        item_desc->setEditable(false);
        item_name->setEditable(false);
        item_phone->setEditable(false);
        item_result->setEditable(false);
        item_state->setEditable(false);
        mTaskModel->setItem(row,0,item_num);
        mTaskModel->setItem(row,1,item_client);
        mTaskModel->setItem(row,2,item_time);
        mTaskModel->setItem(row,3,item_state);
        mTaskModel->setItem(row,4,item_desc);
        mTaskModel->setItem(row,5,item_result);
        mTaskModel->setItem(row,6,item_name);
        mTaskModel->setItem(row,7,item_phone);
        row++;
    }
}

void DialogTaskMore::slot_state_changed(int)
{
    on_btnQuery_clicked(true);
}
