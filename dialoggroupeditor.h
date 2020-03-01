#ifndef DIALOGGROUPEDITOR_H
#define DIALOGGROUPEDITOR_H

#include <QDialog>

namespace Ui {
class DialogGroupEditor;
}

class DialogGroupEditor : public QDialog
{
    Q_OBJECT

public:
    explicit DialogGroupEditor(QWidget *parent = 0);
    ~DialogGroupEditor();

private:
    Ui::DialogGroupEditor *ui;
};

#endif // DIALOGGROUPEDITOR_H
