#ifndef DIALOGWARNINGMORE_H
#define DIALOGWARNINGMORE_H

#include <QDialog>

class QStandardItemModel;
namespace Ui {
class DialogWarningMore;
}

class DialogWarningMore : public QDialog
{
    Q_OBJECT

public:
    explicit DialogWarningMore(QWidget *parent = 0);
    ~DialogWarningMore();

public slots:
    void on_btnWarningMore_clicked(bool);

private:
    Ui::DialogWarningMore *ui;
    QStandardItemModel *mWarningModel;
};

#endif // DIALOGWARNINGMORE_H
