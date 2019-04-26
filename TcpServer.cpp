#include "TcpServer.h"

#include <QNetworkInterface>
#include <QDebug>

TcpServer::TcpServer()
{
    tcpServer = new QTcpServer(this);

    connect(tcpServer,SIGNAL(newConnection()),
            this,SLOT(doProcessNewConnect()));//新连接信号和槽

    connect(tcpServer,SIGNAL(acceptError(QAbstractSocket::SocketError)),
            this,SLOT(doProcessAcceptError(QAbstractSocket::SocketError)));//接收错误处理槽

    DC.server_ip_list=getHostIpAddrs();
}

QStringList TcpServer::getHostIpAddrs()  //获取本机IP地址
{
    QString ip;
    QStringList ip_list;
    QList<QHostAddress> ipAddrs = QNetworkInterface::allAddresses();
    for (int i = 0; i < ipAddrs.size(); ++i)
    {
        if (ipAddrs.at(i) != QHostAddress::LocalHost && ipAddrs.at(i).toIPv4Address())
        {
            // 如果这个地址不是127.0.0.1，并且是IPv4的地址，就将其赋值给ip，并结束for循环
            ip = ipAddrs.at(i).toString();
            qDebug()<<u8"本机ip:"<<ip;
            if(!ip.contains("169."))
            {
                ip_list.append(ip);
            }
        }
    }
    // 如果ip为空，则将其赋值为127.0.0.1
    if (ip.isEmpty())
        ip_list.append(QHostAddress(QHostAddress::LocalHost).toString());
    return ip_list;
}

bool TcpServer::TcpServerListen(QString serPort)
{
    bool ret;
    ret = tcpServer->listen(QHostAddress::Any,serPort.toInt());
    if(ret)
    {
        //        tcpServer->setMaxPendingConnections(100);//设置最大连接数量
    }

    DC.tcp_listen_flag=ret;

    return ret;
}

void TcpServer::TcpServerClose()
{
    tcpServer->close();
    emit tcp_server_close();
}

void TcpServer::doProcessNewConnect()
{
    DC.tcp_server_flag=true;
    client = tcpServer->nextPendingConnection();//获取客户端描述符
    clients.append(client);//获取所有的客户端连接
    //读取信息
    connect(client,SIGNAL(readyRead()),
            this,SLOT(doProcessReadyRead()));//客户端读取内容信号连接槽
    connect(client,SIGNAL(disconnected()),
            this,SLOT(doProcessDisconnected()));//客户端断开连接信号和槽

    //客户端的连入
    connect(client,SIGNAL(connected()),
            this,SLOT(doProcessConnected()));
}
void TcpServer::doProcessAcceptError(QAbstractSocket::SocketError)
{

}
void TcpServer::doProcessDisconnected()
{
    DC.tcp_server_flag=false;
}
void TcpServer::doProcessReadyRead()
{
    client = (QTcpSocket *)this->sender();
    QString str;
	RECV_DATA recv_data;
    while(!client->atEnd())//没有读到末尾一直读
    {
        str.append(QString(client->readAll()));
        qDebug()<<u8"机器人接收数据："<<str;
    }

    if(str.compare("inpos", Qt::CaseInsensitive)==0)
    {
        qDebug()<<u8"机器人到位";
		recv_data = InPos;
    }

    if(str.compare("complete", Qt::CaseInsensitive)==0)
    {
        qDebug()<<u8"所有pin脚测试完成";
		recv_data = Complete;
    }

	if (str.compare("recv", Qt::CaseInsensitive) == 0)
	{
		qDebug() << u8"机器人收到信息";
		recv_data = Recv;
	}

	emit robot_recv(recv_data);

}
void TcpServer::doProcessConnected()
{

}

void TcpServer::sendData(QString data)
{
    qDebug()<<u8"机器人发送数据:"<<data;
    for(int i=0;i<clients.length();i++)//遍历客户端
    {
        QString msg = data;
        clients.at(i)->write(msg.toLatin1());
    }
}
