#ifndef DIALOGGROUPEDITOR_H
#define DIALOGGROUPEDITOR_H

#include <QDialog>

class EditorDataHandler;
class QTabWidget;
namespace Ui {
class DialogGroupEditor;
}

class DialogGroupEditor : public QDialog
{
    Q_OBJECT

public:
    explicit DialogGroupEditor(int tab, QWidget *parent = 0);
    ~DialogGroupEditor();

public slots:
    void on_btnZoneAdd_clicked(bool);
    void on_btnZoneModify_clicked(bool);
    void on_btnZoneDelete_clicked(bool);

    void on_btnGroupAdd_clicked(bool);
    void on_btnGroupModify_clicked(bool);
    void on_btnGroupDelete_clicked(bool);
    void on_btnDetailInfo_clicked(bool);

    void slotDetailSelected(QByteArray info);

signals:
    void sigDetailModified(QByteArray info);

private:
    Ui::DialogGroupEditor *ui;
    EditorDataHandler *dataHandler;
};

#endif // DIALOGGROUPEDITOR_H
