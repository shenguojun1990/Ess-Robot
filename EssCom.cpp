#include "EssCom.h"
#include <QSerialPortInfo>
#include <QDebug>
#include <QByteArray>
#include <QThread>

static char Fun_Bcc_Create (char *p1, int len)
{
    char bcc = 0;
    for(int i=1;i<len;i++)
    {
        bcc ^= p1[i];
    }

    return bcc;
}

EssCom::EssCom()
{
    Com_Init();

    Get_Com_List();

    //连接信号槽 当下位机发送数据QSerialPortInfo 会发送个 readyRead 信号,我们定义个槽void receiveInfo()解析数据
    connect(serial,SIGNAL(readyRead()),this,SLOT(receiveInfo()));

}

void EssCom::Com_Init()
{
    serial = new QSerialPort;
}


void EssCom::Get_Com_List()//查找可用的串口
{
    //    for(int i=0;i<DC.serialPortInfo.size();i++)
    //    {
    //        qDebug() << "Name : " << DC.serialPortInfo.at(i).portName();
    //    }
}

void EssCom::Com_Open(QString portName)//com连接
{
    //设置串口名
    serial->setPortName(portName);
    //打开串口
    bool ret=serial->open(QIODevice::ReadWrite);
    if(ret)
    {
        emit com_opened();
        serial->setBaudRate(QSerialPort::Baud9600);//设置波特率为9600
        serial->setDataBits(QSerialPort::Data8);//设置数据位8
        serial->setParity(QSerialPort::NoParity);//设置校验位0
        serial->setStopBits(QSerialPort::OneStop);//停止位设置为1
        serial->setFlowControl(QSerialPort::NoFlowControl);//设置为无流控制

        DC.com_flag=true;
    }

}

void EssCom::Com_Close()//com中断
{
    serial->close();
    DC.com_flag=false;
    emit com_closed();
}

void EssCom::receiveInfo()
{
    QByteArray info = serial->readAll();
    QByteArray hexData = info.toHex();
    qDebug()<<u8"串口接收数据:"<< hexData;

    if(hexData=="06")
    {
        //设置接触方式
        if(DC.last_cmd==PH)
        {
            sendData(PJ);
            return;
        }
        //设置触发方式
        if(DC.last_cmd==PJ)
        {
            sendData(PG);
            return;
        }
        //设置电压正负
        if(DC.last_cmd==PG)
        {
            sendData(PL);
            return;
        }
        //设置电压值
        if(DC.last_cmd==PL)
        {
            sendData(PK);
            return;
        }
        //设置间隔时间
        if(DC.last_cmd==PK)
        {
            sendData(PI);
            return;
        }
        //设置次数
        if(DC.last_cmd==PI)
        {
//            sendData(PG);
            emit ess_setting_finished();
            return;
        }
        //开始
        if(DC.last_cmd==AA)
        {
            sendData(AC);
            return;
        }
        //触发
        if(DC.last_cmd==AC)
        {
            sendData(QF);
            return;
        }

        //停止
        if(DC.last_cmd==AB)
        {
//            sendData(AC);
            return;
        }
        //松开
        if(DC.last_cmd==AD)
        {
            sendData(AB);
            return;
        }
    }

    if(hexData.contains("024146"))
    {
        int a=0;
        qDebug()<<tr(u8"查询已完成次数");
        int info_len=info.length();
        char *ch=info.data();
        for(int i=3;i<info_len-2;i++)
        {
            a=a*10;
            QString str_ascii = QString(ch[i]);
            int str_int = str_ascii.toInt();
            a=a+str_int;

        }
        DC.completed_count=a;
        emit completed_count();
        if(a==DC.repeat_count)
        {
            sendData(AD);
			if (DC.test_mode==One_Step) 
			{
				//emit ess_finished();
				//emit ess_setting_unfinished();
			}
			else if (DC.test_mode == ALL)
			{
				emit to_next();
			}
            
        }
        else
        {
            sendData(QF);
        }
    }
}


void EssCom::sendData(ESS_CMD ess_cmd)
{
    DC.last_cmd=ess_cmd;

    int len=0;
    char *buf=new char[255];

    if(!DC.com_flag)
    {
        return;
    }



    switch(ess_cmd)
    {
    case PG://设置电压正负
    {
        len=6;
        if(DC.prolarity==Positive)//正压
        {
            //STX P G 0 ETX BCC
            buf[0]=0x02;
            buf[1]='P';
            buf[2]='G';
            buf[3]='0';
            buf[4]=0x03;
            buf[5]=Fun_Bcc_Create(buf,len-1);
            serial->write(buf,len);
        }
        else//负压
        {
            //STX P G 1 ETX BCC
            buf[0]=0x02;
            buf[1]='P';
            buf[2]='G';
            buf[3]='1';
            buf[4]=0x03;
            buf[5]=Fun_Bcc_Create(buf,len-1);
            serial->write(buf,len);
        }

    }
        break;
    case QG://询问电压正负
    {
        //STX Q G ETX BCC
        buf[0]=0x02;
        buf[1]='Q';
        buf[2]='G';
        buf[3]=0x03;
        buf[4]=Fun_Bcc_Create(buf,len-1);
        serial->write(buf,len);
    }
        break;
    case PH://设置接触方式
    {
        len=6;
        if(DC.discharge==Contect)//直接接触
        {
            //STX P H 0 ETX BCC
            buf[0]=0x02;
            buf[1]='P';
            buf[2]='H';
            buf[3]='0';
            buf[4]=0x03;
            buf[5]=Fun_Bcc_Create(buf,len-1);
            serial->write(buf,len);
        }
        else//空气接触
        {
            //STX P H 1 ETX BCC
            buf[0]=0x02;
            buf[1]='P';
            buf[2]='H';
            buf[3]='1';
            buf[4]=0x03;
            buf[5]=Fun_Bcc_Create(buf,len-1);
            serial->write(buf,len);
        }
    }
        break;
    case QH://询问接触方式
    {
        //STX Q H ETX BCC
        buf[0]=0x02;
        buf[1]='Q';
        buf[2]='H';
        buf[3]=0x03;
        buf[4]=Fun_Bcc_Create(buf,len-1);
        serial->write(buf,len);
    }
        break;
    case PJ://重复次数
    {
        //STX P J <次数> ETX BCC
        QString count_str=QString::number((int)(DC.repeat_count));

        QByteArray byte = count_str.toUtf8();

        len=5+count_str.size();

        buf[0]=0x02;
        buf[1]='P';
        buf[2]='J';
        for(int i = 0; i < count_str.size(); i++)
        {
            //            qDebug() << int(byte.at(i));
            buf[3+i]=byte.at(i);
        }
        buf[len-2]=0x03;
        buf[len-1]=Fun_Bcc_Create(buf,len-1);
        serial->write(buf,len);
    }
        break;
    case QJ://询问重复次数
    {
        //STX Q J ETX BCC
        buf[0]=0x02;
        buf[1]='Q';
        buf[2]='J';
        buf[3]=0x03;
        buf[4]=Fun_Bcc_Create(buf,len-1);
        serial->write(buf,len);
    }
        break;
    case PI://触发方式
    {
        //STX P I <0~2> EXT BCC
        len=6;
        buf[0]=0x02;
        buf[1]='P';
        buf[2]='I';
        if(DC.trigger==Controller)//本机
        {
            buf[3]='0';
        }
        if(DC.trigger==Gun)//枪
        {
            buf[3]='0';
        }
        if(DC.trigger==External)//外部
        {
            buf[3]='0';
        }
        buf[4]=0x03;
        buf[5]=Fun_Bcc_Create(buf,len-1);
        serial->write(buf,len);
    }
        break;
    case QI://询问触发方式
    {
        //STX Q I ETX BCC
        len=5;
        buf[0]=0x02;
        buf[1]='Q';
        buf[2]='I';
        buf[3]=0x03;
        buf[4]=Fun_Bcc_Create(buf,len-1);
        serial->write(buf,len);
    }
        break;
    case PK://间隔时间
    {
        //STX P K <时间> ETX BCC
        QString interval_str=QString::number((int)(DC.interval*100));

        QByteArray byte = interval_str.toUtf8();

        len=5+interval_str.size();

        buf[0]=0x02;
        buf[1]='P';
        buf[2]='K';
        for(int i = 0; i < interval_str.size(); i++)
        {
            //            qDebug() << int(byte.at(i));
            buf[3+i]=byte.at(i);
        }
        buf[len-2]=0x03;
        buf[len-1]=Fun_Bcc_Create(buf,len-1);
        serial->write(buf,len);
    }
        break;
    case QK://询问间隔时间
    {
        //STX Q K ETX BCC
        len=5;
        buf[0]=0x02;
        buf[1]='Q';
        buf[2]='K';
        buf[3]=0x03;
        buf[4]=Fun_Bcc_Create(buf,len-1);
        serial->write(buf,len);
    }
        break;
    case PL://设置电压值
    {
        QString vol_str=QString::number((int)(DC.voltage*100));

        QByteArray byte = vol_str.toUtf8();

        len=5+vol_str.size();
        // STX P L <电压> ETX BCC
        buf[0]=0x02;
        buf[1]=0x50;
        buf[2]=0x4C;
        for(int i = 0; i < vol_str.size(); i++)
        {
            //            qDebug() << int(byte.at(i));
            buf[3+i]=byte.at(i);
        }
        buf[len-2]=0x03;
        buf[len-1]=Fun_Bcc_Create(buf,len-1);
        serial->write(buf,len);

    }

        break;

    case QL://询问电压
    {
        //STX Q L ETX BCC
        len=5;
        buf[0]=0x02;
        buf[1]='Q';
        buf[2]='L';
        buf[3]=0x03;
        buf[4]=Fun_Bcc_Create(buf,len-1);
        serial->write(buf,len);
    }
        break;

    case QF://询问已完成次数
    {
        //STX Q F ETX BCC
        len=5;
        buf[0]=0x02;
        buf[1]='Q';
        buf[2]='F';
        buf[3]=0x03;
        buf[4]=Fun_Bcc_Create(buf,len-1);
        serial->write(buf,len);
    }
        break;

    case AA://试验开始
    {
        //STX A A ETX BCC
        len=5;
        buf[0]=0x02;
        buf[1]='A';
        buf[2]='A';
        buf[3]=0x03;
        buf[4]=Fun_Bcc_Create(buf,len-1);
        serial->write(buf,len);
    }
        break;
    case AB://试验结束
    {
        //STX A B ETX BCC
        len=5;
        buf[0]=0x02;
        buf[1]='A';
        buf[2]='B';
        buf[3]=0x03;
        buf[4]=Fun_Bcc_Create(buf,len-1);
        serial->write(buf,len);
		if (DC.test_mode==One_Step) 
		{
			emit ess_setting_unfinished();
			emit ess_finished();
		}
        
    }
        break;
    case AC://trigger设定
    {
        //STX A C ETX BCC
        len=5;
        buf[0]=0x02;
        buf[1]='A';
        buf[2]='C';
        buf[3]=0x03;
        buf[4]=Fun_Bcc_Create(buf,len-1);
        serial->write(buf,len);
    }
        break;
    case AD://trigger放开
    {
        //STX A D ETX BCC
        len=5;
        buf[0]=0x02;
        buf[1]='A';
        buf[2]='D';
        buf[3]=0x03;
        buf[4]=Fun_Bcc_Create(buf,len-1);
        serial->write(buf,len);
    }
        break;

       
    }
	qDebug() << u8"串口发送"<< QString(QByteArray(buf, len));
}
