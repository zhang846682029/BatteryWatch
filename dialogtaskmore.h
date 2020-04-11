#ifndef DIALOGTASKMORE_H
#define DIALOGTASKMORE_H

#include <QDialog>

namespace Ui {
class DialogTaskMore;
}

class QStandardItemModel;
class DialogTaskMore : public QDialog
{
    Q_OBJECT

public:
    explicit DialogTaskMore(QWidget *parent = 0);
    ~DialogTaskMore();

public slots:
    void on_btnQuery_clicked(bool);
    void slot_state_changed(int);

private:
    Ui::DialogTaskMore *ui;
    QStandardItemModel *mTaskModel;
};

#endif // DIALOGTASKMORE_H
