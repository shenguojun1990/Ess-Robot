#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QObject>

#include "DataCenter.h"
#include <QTcpServer>
#include <QTcpSocket>

class TcpServer: public QObject
{
    Q_OBJECT
public:
    TcpServer();

    bool TcpServerListen(QString serPort);

    void TcpServerClose();

    void sendData(QString data);

private:
    QStringList getHostIpAddrs();//获取本机IP

    void Init();//初始化函数

    QTcpServer *tcpServer;//定义一个TCP服务器
    QList<QTcpSocket *>clients;   //定义连接的客户端
    QTcpSocket *client;

public slots:
    //处理客户端连接断开发送数据等槽函数
    void doProcessNewConnect();
    void doProcessAcceptError(QAbstractSocket::SocketError);
    void doProcessDisconnected();
    void doProcessReadyRead();
    void doProcessConnected();

signals:
    void tcp_server_close();

    void robot_recv(RECV_DATA	data,DISCHARGE discharge);

};

#endif // TCPSERVER_H
