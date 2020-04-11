#ifndef DIALOGTASKEDITOR_H
#define DIALOGTASKEDITOR_H

#include <QDialog>

namespace Ui {
class DialogTaskEditor;
}

class DialogTaskEditor : public QDialog
{
    Q_OBJECT

public:
    explicit DialogTaskEditor(QWidget *parent = 0);
    ~DialogTaskEditor();

public slots:
    void on_btnTaskModify_clicked(bool);
    void on_combxTask_changed(QString task);

private:
    void init_task_list(void);

private:
    Ui::DialogTaskEditor *ui;
};

#endif // DIALOGTASKEDITOR_H
