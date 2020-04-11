#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class Server;
class DataHandler;
class WarningHandler;
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void slotTreeModeUpdate(void);
    void slotTreeMenu(const QPoint &pos);
    void slotActionZoneEditortriggered(bool);
    void slotActionGroupEditortriggered(bool);
    void slotActionDeviceEditortriggered(bool);

    void slotWarnMenu(const QPoint &pos);
    void slotActionWarnEditorTriggered(bool);
    void slotActionWarnMoreTriggered(bool);
    void slotTaskMenu(const QPoint &pos);
    void slotActionTaskEditorTriggered(bool);
    void slotActionTaskMoreTriggered(bool);

    void slotActionChartsTriggered(bool);

private:
    Ui::MainWindow *ui;
    Server *mTcpServer;
    DataHandler *mDataHandler;
    WarningHandler *mWarningHandler;
};

#endif // MAINWINDOW_H
