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

	bool emit_flag;//信号槽标志位，true时不执行

signals:
    void com_opened();//成功打开串口
    void com_closed();//成功关闭串口
    void ess_setting_finished();//UI静电枪参数已写入
    void ess_setting_unfinished();//UI静电枪参数未写入

    void completed_count();//主窗口顯示静电枪测试完成次数

    void ess_finished();//测试完成

	void to_next();//全部测试模式下，去下一个pin脚

    void get_finished_count_start();//触发计时器，每1S获取一次已触发次数
    void get_finished_count_stop();

    void airmode_go_down();//空气接触模式下，走到测试下位点

public slots:
    void receiveInfo();
};

#endif // ESSCOM_H
