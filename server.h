#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QTcpServer>
#include <QMap>

class Server : public QTcpServer
{
    Q_OBJECT
public:
    Server(QObject* parent, int port);

protected:
    void incomingConnection(int socketDescriptor);//只要出现一个新的连接，就会自动调用这个函数

signals:
    // 发现重复的电脑编号
    void sigRepeatClient(int);
    // 如果电脑编号重复了 1.数据不写入数据库
    // 2.设备列表里显示为在线状态 3.主界面提示编号重复
    void sigPowerInfo(QString, bool);


public slots:
    void slotPowerInfo(QString data, int len);

private:
    QMap<int,QString> mClientAddress; // 电脑编号:电脑IP
};

#endif // SERVER_H
