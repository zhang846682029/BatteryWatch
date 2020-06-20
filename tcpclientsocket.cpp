#include "tcpclientsocket.h"
#include <QDebug>
#include <QHostAddress>
#include <QJsonObject>
#include <QJsonDocument>

TcpClientSocket::TcpClientSocket(QObject *parent):QTcpSocket(parent)
{
    //客户端发送数据过来就会触发readyRead信号
    connect(this, &TcpClientSocket::readyRead,
            this, &TcpClientSocket::receivedata);
    connect(this, &QAbstractSocket::disconnected,
            this, &QObject::deleteLater);
}

void TcpClientSocket::receivedata()
{
    QByteArray array=readAll();
//    qDebug()<<peerAddress().toString()<<peerPort()<<array.size()<<array.toHex();
    QHostAddress client_ipv4(peerAddress().toIPv4Address());
    qDebug()<<client_ipv4.toString()<<peerPort()<<array.size()<<array.toHex();

    const unsigned char ack_data[]={0x5A,0xA5,0x03,0x00,0x00,0x00,0x00,0x02};
    QByteArray array1((const char*)ack_data,sizeof(ack_data));
    write(array1);

    QByteArray arrar_head;
    arrar_head.append(0x5A);
    arrar_head.append(0xA5);
    if(array.startsWith(arrar_head)==false){
        return;
    }
    // 数据长度
    unsigned char data_len=array.at(2);
    // 功能码
    unsigned char data_func_code=array.at(3);
    qDebug()<<"1"<<data_len<<data_func_code;
    //
//     char ip1=array.at(4);
//     char ip2=array.at(5);
//     char ip3=array.at(6);
//     char ip4=array.at(7);
//    QString data_client_ip = QString("%1.%2.%3.%4")
//            .arg(ip1).arg(ip2).arg(ip3).arg(ip4);
    QString data_client_ip = client_ipv4.toString();
    // 客户端编号 1-60000 默认为0
//     short data_client_id1=array.at(11);
//     short data_client_id2=array.at(12);
//     short data_client_id = data_client_id1<<8|data_client_id2;
    unsigned char short_data_tmp[2]={0};
    short_data_tmp[0]=array.at(11);
    short_data_tmp[1]=array.at(12);
    unsigned short data_client_id=0;
    data_client_id|=short_data_tmp[0];
    data_client_id=data_client_id<<8;
    data_client_id|=short_data_tmp[1];
    if(data_client_id<1||data_client_id>60000){
        qDebug()<<__FUNCTION__<<"data_client_id"<<data_client_id;
        return;
    }
    unsigned char mac1=array.at(13);
    unsigned char mac2=array.at(14);
    unsigned char mac3=array.at(15);
    unsigned char mac4=array.at(16);
    unsigned char mac5=array.at(17);
    unsigned char mac6=array.at(18);
    QString data_client_mac = QString("%1:%2:%3:%4:%5:%6")\
            .arg(mac1,2,16,QChar(0)).arg(mac2,2,16,QChar(0))\
            .arg(mac3,2,16,QChar(0)).arg(mac4,2,16,QChar(0))\
            .arg(mac5,2,16,QChar(0)).arg(mac6,2,16,QChar(0));
    qDebug()<<"2"<<data_client_id<<data_client_ip<<data_client_mac;
    // 电量百分比
    unsigned char power_rate = array.at(19);
    // 电压
//     short voltage1=array.at(20);
//     short voltage2=array.at(21);
//     short voltage3 = voltage1<<8|voltage2;
     short_data_tmp[0]=array.at(20);
     short_data_tmp[1]=array.at(21);
     unsigned short voltage3=0;
     voltage3|=short_data_tmp[0];
     voltage3=voltage3<<8;
     voltage3|=short_data_tmp[1];
//    float voltage = voltage3;
//    voltage = voltage*0.01;
    // 电流
//     short current1=array.at(22);
//     short current2=array.at(23);
//     short current3 = current1<<8|current2;
     short_data_tmp[0]=array.at(22);
     short_data_tmp[1]=array.at(23);
     unsigned short current3=0;
     current3|=short_data_tmp[0];
     current3=current3<<8;
     current3|=short_data_tmp[1];
//    float current = current3;
//    current = current*0.01;
    // 额定容量
//     short volume1=array.at(24);
//     short volume2=array.at(25);
//     short volume3 = volume1<<8|volume2;
     short_data_tmp[0]=array.at(24);
     short_data_tmp[1]=array.at(25);
     unsigned short volume3=0;
     volume3|=short_data_tmp[0];
     volume3=volume3<<8;
     volume3|=short_data_tmp[1];
//    float volume = volume3;
//    volume = volume*0.1;
//    qDebug()<<"3"<<power_rate<<voltage<<current<<volume;
    // 温度
//    short temp1=array.at(26);
//    short temp2=array.at(27);
//    short temp3 = temp1<<8|temp2;
     short_data_tmp[0]=array.at(26);
     short_data_tmp[1]=array.at(27);
     unsigned short temp3=0;
     temp3|=short_data_tmp[0];
     temp3=temp3<<8;
     temp3|=short_data_tmp[1];
    bool flag1=false; // 最高位F表明温度为负
    if((temp3&0xF000)==0xF000){
        flag1=true;
    }
    temp3 = temp3&0x0FFF;
    temp3=flag1?0-temp3:temp3;
//    float temp = temp3;
//    temp = temp*0.1;
//    temp=flag1?0-temp:temp;
    // 电流方向 0-放电 1-充电 2-待机
    unsigned char direction=array.at(28);
    // 循环次数
//     short count1=array.at(29);
//     short count2=array.at(30);
//     short count3 = count1<<8|count2;
     short_data_tmp[0]=array.at(29);
     short_data_tmp[1]=array.at(30);
     unsigned short count3=0;
     count3|=short_data_tmp[0];
     count3=count3<<8;
     count3|=short_data_tmp[1];
//    float count = count3;
//    count = count*0.1;
    // 通讯状态 0-掉线 1-在线 掉线时数据不可信
    unsigned char alarm=array.at(31);
    // 报警 1-报警 0-不是报警
    unsigned char alive=array.at(32);
//    qDebug()<<"4"<<temp<<direction<<count<<alarm;
    // 上传时间间隔
//     short interval1=array.at(36);
//     short interval2=array.at(37);
//     short interval = interval1<<8|interval2;
     short_data_tmp[0]=array.at(36);
     short_data_tmp[1]=array.at(37);
     unsigned short interval=0;
     interval|=short_data_tmp[0];
     interval=interval<<8;
     interval|=short_data_tmp[1];
    // 库仑计模块地址
    unsigned char address=array.at(38);
    qDebug()<<"5"<<interval<<address;

    QJsonObject json_obj;
    QJsonObject json_obj_identify;
    json_obj_identify["ip"]=data_client_ip;
    json_obj_identify["mac"]=data_client_mac;
    json_obj_identify["id"]=data_client_id; // 电脑编号
    json_obj_identify["address"]=address;
    json_obj_identify["alive"]=alive;
    json_obj["identify"]=json_obj_identify;
    QJsonObject json_obj_property;
    json_obj_property["rate"]=power_rate;
    json_obj_property["voltage"]=voltage3;
    json_obj_property["current"]=current3;
    json_obj_property["volume"]=volume3;
    json_obj_property["temp"]=temp3;
    json_obj_property["direction"]=direction;
    json_obj_property["count"]=count3;
    json_obj_property["alarm"]=alarm;
    json_obj_property["interval"]=interval;
    json_obj["property"]=json_obj_property;
    QJsonDocument json_doc(json_obj);
    QByteArray json_array_data=json_doc.toJson(QJsonDocument::Compact);
    QString data_str=QString(json_array_data);
    emit updateserver(data_str,data_str.size());
}
