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
    qDebug()<<peerAddress().toString()<<peerPort()<<array.size()<<array.toHex();

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
    unsigned char ip1=array.at(4);
    unsigned char ip2=array.at(5);
    unsigned char ip3=array.at(6);
    unsigned char ip4=array.at(7);
    QString data_client_ip = QString("%1.%2.%3.%4")\
            .arg(ip1).arg(ip2).arg(ip3).arg(ip4);
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
    qDebug()<<"2"<<data_client_ip<<data_client_mac;
    // 电量百分比
    unsigned char power_rate = array.at(19);
    // 电压
    unsigned short voltage1=array.at(20);
    unsigned short voltage2=array.at(21);
    unsigned short voltage3 = voltage1<<8|voltage2;
//    float voltage = voltage3;
//    voltage = voltage*0.01;
    // 电流
    unsigned short current1=array.at(22);
    unsigned short current2=array.at(23);
    unsigned short current3 = current1<<8|current2;
//    float current = current3;
//    current = current*0.01;
    // 额定容量
    unsigned short volume1=array.at(24);
    unsigned short volume2=array.at(25);
    unsigned short volume3 = volume1<<8|volume2;
//    float volume = volume3;
//    volume = volume*0.1;
//    qDebug()<<"3"<<power_rate<<voltage<<current<<volume;
    // 温度
    short temp1=array.at(26);
    short temp2=array.at(27);
    short temp3 = temp1<<8|temp2;
    bool flag1=false; // 最高位F表明温度为负
    if(temp3&0xF000==0xF000){
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
    unsigned short count1=array.at(29);
    unsigned short count2=array.at(30);
    unsigned short count3 = count1<<8|count2;
//    float count = count3;
//    count = count*0.1;
    // 报警 1-报警 0-不是报警
    unsigned char alarm=array.at(31);
//    qDebug()<<"4"<<temp<<direction<<count<<alarm;
    // 上传时间间隔
    unsigned short interval1=array.at(36);
    unsigned short interval2=array.at(37);
    unsigned short interval = interval1<<8|interval2;
    // 库仑计模块地址
    unsigned char address=array.at(38);
    qDebug()<<"5"<<interval<<address;

    QJsonObject json_obj;
    QJsonObject json_obj_identify;
    json_obj_identify["ip"]=data_client_ip;
    json_obj_identify["mac"]=data_client_mac;
    json_obj_identify["address"]=address;
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
