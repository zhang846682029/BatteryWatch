#include "server.h"
#include "tcpclientsocket.h"
#include <QHostAddress>
#include <QJsonDocument>
#include <QJsonObject>

Server::Server(QObject *parent, int port):QTcpServer(parent)
{
    listen(QHostAddress::Any, port); //监听
}

void Server::slotUpdateServer(QString data, int len)
{
    QJsonDocument json_doc=QJsonDocument::fromJson(data.toLatin1());
    QJsonObject json_obj=json_doc.object();
    QJsonObject json_obj_identify = json_obj.value("identify").toObject();
    QString identify_ip = json_obj_identify.value("ip").toString();
    QString identify_mac = json_obj_identify.value("mac").toString();
    int identify_address = json_obj_identify.value("address").toInt();
    QJsonObject json_obj_property = json_obj.value("property").toObject();
    int rate = json_obj_property.value("rate").toInt();
    qDebug()<<__FUNCTION__<<identify_address<<identify_mac<<identify_ip<<rate;

    emit updateserver(data,len);
}

void Server::incomingConnection(int socketDescriptor)
{
    TcpClientSocket *tcpclientsocket = new TcpClientSocket(this);//只要有新的连接就生成一个新的通信套接字
    //将新创建的通信套接字描述符指定为参数socketdescriptor
    tcpclientsocket->setSocketDescriptor(socketDescriptor);
    connect(tcpclientsocket, &TcpClientSocket::updateserver,
            this, &Server::slotUpdateServer);
}
