#include "dialoggroupeditor.h"
#include "ui_dialoggroupeditor.h"

DialogGroupEditor::DialogGroupEditor(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogGroupEditor)
{
    ui->setupUi(this);
}

DialogGroupEditor::~DialogGroupEditor()
{
    delete ui;
}
