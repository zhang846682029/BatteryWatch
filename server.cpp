#include "server.h"
#include "tcpclientsocket.h"
#include <QHostAddress>
#include <QJsonDocument>
#include <QJsonObject>

Server::Server(QObject *parent, int port):QTcpServer(parent)
{
    listen(QHostAddress::Any, port); //监听
}

void Server::incomingConnection(int socketDescriptor)
{
    TcpClientSocket *tcpclientsocket = new TcpClientSocket(this);//只要有新的连接就生成一个新的通信套接字
    //将新创建的通信套接字描述符指定为参数socketdescriptor
    tcpclientsocket->setSocketDescriptor(socketDescriptor);
    connect(tcpclientsocket, &TcpClientSocket::updateserver,
            this, &Server::slotPowerInfo);
}

void Server::slotPowerInfo(QString data, int len)
{
    QJsonDocument json_doc=QJsonDocument::fromJson(data.toLatin1());
    QJsonObject json_obj=json_doc.object();
    QJsonObject json_obj_identify = json_obj.value("identify").toObject();
    QString identify_ip = json_obj_identify.value("ip").toString();
//    QString identify_mac = json_obj_identify.value("mac").toString();
    int identify_id = json_obj_identify.value("id").toInt();//电脑编号 1-60000
    int identify_alive = json_obj_identify.value("alive").toInt();// 通讯状态 0-掉线 1-在线 掉线时数据不可信

    bool isClientRepeat=false;
    auto it1=mClientAddress.find(identify_id);
    if(it1!=mClientAddress.end()){
        if(it1.value()!=identify_ip){
            isClientRepeat=true;
            emit sigRepeatClient(identify_id);
        }
    } else {
        mClientAddress[identify_id]=identify_ip;
    }

    if(identify_alive==0){ // 0-掉线状态 数据不可信
        isClientRepeat=true;
    }

    emit sigPowerInfo(data,!isClientRepeat);
}
