#include "dialogwarningmore.h"
#include "ui_dialogwarningmore.h"
#include <QStandardItemModel>
#include <QDateEdit>
#include <QSqlQuery>
#include <QDebug>

DialogWarningMore::DialogWarningMore(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogWarningMore)
{
    ui->setupUi(this);

    ui->dateMoreBegin->setDate(QDate::currentDate());
    ui->dateMoreEnd->setDate(QDate::currentDate());

    mWarningModel=new QStandardItemModel();
    QStringList headers;
    headers<<tr("num")<<tr("battery")<<tr("time")<<tr("state")<<tr("desc");
    mWarningModel->setHorizontalHeaderLabels(headers);
    ui->tableView->setModel(mWarningModel);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
}

DialogWarningMore::~DialogWarningMore()
{
    delete ui;
}

void DialogWarningMore::on_btnWarningMore_clicked(bool)
{
    mWarningModel->rowCount();
    mWarningModel->removeRows(0,mWarningModel->rowCount());

    QSqlQuery query;

    QMap<int,QString> m_event_desc;
    QString sql = QString("SELECT property,event_desc FROM eventInfo;");
    bool r1 = query.exec(sql);
    qDebug()<<r1<<sql;
    while(query.next()){
        m_event_desc[query.value(0).toInt()]=query.value(1).toString();
    }


    sql = QString("SELECT * FROM warningInfo WHERE time_start "
                          "BETWEEN strftime('%Y-%m-%d','%1','start of day') "
                          "AND strftime('%Y-%m-%d','%2','1 day','start of day')")
            .arg(ui->dateMoreBegin->date().toString("yyyy-MM-dd"))
            .arg(ui->dateMoreEnd->date().toString("yyyy-MM-dd"));
    r1 = query.exec(sql);
    qDebug()<<r1<<sql;

    int row=0;
    while(query.next()){
        int client = query.value(1).toInt();
        QDateTime event_time_start = query.value(6).toDateTime();
        QDateTime event_time_end = query.value(7).toDateTime();
        int event = query.value(5).toInt();
        bool state = query.value(8).toBool();

        QStandardItem *item_sn = new QStandardItem(QString::number(row+1));
        QStandardItem *item_client = new QStandardItem(QString::number(client));
        QStandardItem *item_time_start = new QStandardItem(event_time_start.toString("yyyy-MM-dd hh:mm:ss"));
        QStandardItem *item_time_end = new QStandardItem(event_time_end.toString("yyyy-MM-dd hh:mm:ss"));
        QStandardItem *item_desc = new QStandardItem(m_event_desc.value(event));
        QStandardItem *item_state = new QStandardItem(state?tr("Warning On"):tr("Warning Off"));
        item_client->setEditable(false);
        item_time_start->setEditable(false);
        item_time_end->setEditable(false);
        item_desc->setEditable(false);
        item_state->setEditable(false);
        mWarningModel->setItem(row,0,item_sn);
        mWarningModel->setItem(row,1,item_client);
        mWarningModel->setItem(row,2,item_time_start);
        mWarningModel->setItem(row,3,item_state);
        mWarningModel->setItem(row,4,item_desc);
        row++;
    }
}
