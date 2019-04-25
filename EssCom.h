#ifndef ESSCOM_H
#define ESSCOM_H

#include <QSerialPort>
#include <DataCenter.h>

class EssCom: public QObject
{
    Q_OBJECT
public:
    EssCom();

private:

    void Com_Init();//变量初始化

public:

    void Com_Open(QString portName);//com连接

    void Com_Close();//com中断

    void Get_Com_List();//查找可用的串口

    void sendData(ESS_CMD ess_cmd);


private:

    QSerialPort *serial;

signals:
    void com_opened();//成功打开串口
    void com_closed();//成功关闭串口
    void ess_setting_finished();//UI静电枪参数已写入
    void ess_setting_unfinished();//UI静电枪参数未写入

    void completed_count();

    void ess_finished();//测试完成

public slots:
    void receiveInfo();
};

#endif // ESSCOM_H
