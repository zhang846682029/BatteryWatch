#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class Server;
class DataHandler;
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

private:
    Ui::MainWindow *ui;
    Server *mTcpServer;
    DataHandler *mDataHandler;
};

#endif // MAINWINDOW_H
