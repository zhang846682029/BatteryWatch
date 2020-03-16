#ifndef DIALOGWARNINGSERTTING_H
#define DIALOGWARNINGSERTTING_H

#include <QDialog>

class WarnDataHandler;
namespace Ui {
class DialogWarningSertting;
}

class DialogWarningSertting : public QDialog
{
    Q_OBJECT

public:
    explicit DialogWarningSertting(QWidget *parent = 0);
    ~DialogWarningSertting();

    void on_btnWarnConfirm_clicked(bool);
    void on_checkWarnMinEnable_toggled(bool);
    void on_checkWarnMaxEnable_toggled(bool);

public slots:
    void slotPropertySelected(QByteArray data);

signals:
    void sigPropertyModified(QByteArray data);

private:
    Ui::DialogWarningSertting *ui;
    WarnDataHandler *dataHandler;
};

#endif // DIALOGWARNINGSERTTING_H
