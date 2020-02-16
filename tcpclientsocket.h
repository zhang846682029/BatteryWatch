#ifndef TCPCLIENTSOCKET_H
#define TCPCLIENTSOCKET_H

#include <QObject>
#include <QTcpSocket>

class TcpClientSocket : public QTcpSocket
{
    Q_OBJECT
public:
    TcpClientSocket(QObject *parent);


protected slots:
    void receivedata();//处理readyRead信号读取数据

signals:
    void updateserver(QString, int);//用来告诉tcpserver需要跟新界面的显示
};

#endif // TCPCLIENTSOCKET_H
