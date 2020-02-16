#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QTcpServer>
#include <QList>

class Server : public QTcpServer
{
    Q_OBJECT
public:
    Server(QObject* parent, int port);
public slots:
    void slotUpdateServer(QString data, int len);

protected:
    void incomingConnection(int socketDescriptor);//只要出现一个新的连接，就会自动调用这个函数

signals:
    void updateserver(QString, int);
};

#endif // SERVER_H
