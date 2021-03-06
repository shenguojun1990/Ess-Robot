﻿#ifndef DATACENTER_H
#define DATACENTER_H

#include <QStringList>
#include <QSerialPortInfo>
#include <QList>

enum ESS_CMD
{
    PG = 0,//电压正负设定
    QG=1,//询问电压正负
    PH = 2,//设置接触模式（空气/接触）
    QH = 3,//询问接触模式（空气/接触）
    PJ = 4,//设置触发模式（本机/枪/外部）
    QJ = 5,//询问触发模式（本机/枪/外部）
    PI = 6,//设置触发次数
    QI = 7,//询问触发次数
    PK = 8,//设置间隔（s）
    QK = 9,//询问间隔（s）
    PL = 10,//设置电压设定
    QL = 11,//询问电压设定
    QF = 12,//询问已完成次数
    AA = 13,//实验开始
    AB = 14,//实验停止
    AC = 15,//trigger设定命令
    AD = 16,//trigger放开命令
	QE = 17,//剩余放电时间
};

enum PROLARITY
{
    Positive = 0,//正压
    Negative = 1,//负压
};

enum DISCHARGE
{
    Contect = 0,//直接接触
    Air = 1,//空气接触
};

enum TRIGGER
{
    Controller = 0,//主机触发模式
    Gun = 1,//枪触发模式
    External = 2,//外部触发模式
};

enum TEST_MODE//测试模式
{
    ALL = 0,//所有点
    One_Step = 1,//单点
};

enum RECV_DATA//测试模式
{
    Recv = 0,//机器人收到信息
    InPos = 1,//机器人到位(直接接触时)
    Complete = 2,//机器人完成所有测试点(直接接触时)

    InPos1 = 3,//机器人到位(空气接触时，到达测试点上位)
    InPos2 = 4,//机器人到位(空气接触时，到达测试点下位)
    Complete1 = 5,//机器人到位(空气接触时，到达测试点上位)
    Complete2 = 6,//机器人到位(空气接触时，到达测试点下位)
};


class DataCenter: public QObject
{
    Q_OBJECT
public:
    explicit DataCenter(QObject *parent = nullptr);

    QList<QSerialPortInfo> serialPortInfo;//本机可用Com

    float voltage;//电压值
    int repeat_count;//重复次数
    PROLARITY prolarity;//电压正负
    DISCHARGE discharge;//接触方式
    TRIGGER trigger;//触发方式
    int interval;//间隔时间

    int completed_count;//已完成次数

    bool com_flag;//串口打开状态 true：已打开
    bool tcp_server_flag;//客戶端已连接 true：已打开
    bool tcp_listen_flag;//tcp_server监听打开状态 true：已打开

    ESS_CMD last_cmd;//上一个ess命令

    QStringList server_ip_list;//本机ip列表

    QString first_reg_date;//第一次注册时间

    int pin_index;//测试第几个pin脚

    TEST_MODE test_mode;//测试模式

    bool loop_finished_flag;//直接接触，全部测试时，所有点都完成

};
extern DataCenter DC;

#endif // DATACENTER_H
