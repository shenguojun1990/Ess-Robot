#include "SuperDog.h"
#include <QDebug>
#include <QString>
#include <QNetworkInterface>
#include <QSettings>
#include <QCryptographicHash>
#include <QTime>

static unsigned char code[] ="BFdl/Z2wK/TG88qkcsRksIr7Bff37HyLcI0mvoslbm8ScVuEAL3y4fuYuQWMFMTswYGK+HFKZNtoyCcruvF0y6hpRqjV7ati9V9NL0ImIm9l4XhZy1cw25LPTNLZwwcQ6W8eLwJSvsQAtHUolekuR5HZadfV4AbuJC9tIXLexuV/l4nJpisnwhPS34GvJ9VZ3mrzwycISaQEeBG25roCavw6ccRbZzocZE1bwQjEzY4SwxZIZSJySK87ALO+AanfcLrgSMWpN1a+43Pih7dUvYZ6lnAlh0d9rybFpvnQpdwwh1h47c2xu54mK8hS1scD9s+bnsjiImDJwz4BcjgFCR/XutIHceqGk0pBd1N6i1GRbOmkMHh0Cb4CaR9MPsD9zNA+wXOd0I6i6mUba32lLabc2SEOYL0E49RvBnFgZ8A3XOm3hV4WBtSErjiZHWNFoHot1MZXZEGkHCyoAcV3d8uj5MroS/r5EePIct5VplEk3MFDXM77YMOBnFolGcUEl8nqfqltT9XvutZWY67jjPjdK0IfzQ4zzBXcd/EmmaWwkWN5yjW7kJt8dRocDdU3dv9agSbC9pPBdp9q8izwFJemP3LFxzaMPnz8iLm+WM/q8ZEDUYyFClVZzUI/O1yjfjIUbtZ+RZg4J6DdFvKe5UrPhyLNyqZDQk/RvxdHxnssXKsOE9ejKv2QadEHt8Jp3iQk7XwELne9qvfO7gnrnU3EQnMk//AHxsvXxMBtztLf/HW6sVs0wqvPJwMAhkQO3qYhltDQwU8xw5tz/g/vbH1jGlSZbeSEChxtmU6NTto4iBG1rxB24/pzYoTcyRl2jcRiPIZFSHuouX+Mtxk4PTDPQkqEzt4kYfHSOEAS28pN5puN7yROPfWncf5eMZ4dfWVbz24OhneCXYF+TIh45PTUgOkSYGBY8MMGyY/voySP8zcBV9y32xQrX3XqFBuBFuwy4YsqBoUWNyjvaY00LQ==";


SuperDog::SuperDog(QObject *parent) : QObject(parent)
{
    softdog_login();
}

void SuperDog::softdog_login()
{
    dog_feature_t feature_id=0;
    dog_vendor_code_t vendor_code;

    vendor_code=code;

    dog_status_t a=dog_login(feature_id,vendor_code,&handle);
    qDebug()<<"softdog_login:"<<a;
}

bool SuperDog::read_dog()
{
    dog_fileid_t fileid=1;
    dog_size_t offset=0;
    dog_size_t length=13;
    char buffer[255];

    dog_status_t a=dog_read(handle,fileid,offset,length,buffer);
    qDebug()<<"read_dog:"<<a;
//    for (int i=0;i<static_cast<int>(length);i++)
//    {
//        qDebug() << buffer[i];
//    }
    QString str(buffer);

//    qDebug()<<str.contains("Su_Zhou_He_Yi");

    return str.contains("Su_Zhou_He_Yi");
}

QString SuperDog::get_time()
{
    dog_time_t time;

    dog_get_time(handle,
                 &time);

    unsigned int day;
    unsigned int month;
    unsigned int year;
    unsigned int hour;
    unsigned int minute;
    unsigned int second;

    dog_status_t a = dog_dogtime_to_datetime(time,
                                             &day,
                                             &month,
                                             &year,
                                             &hour,
                                             &minute,
                                             &second);

//    qDebug()<<a<<day<<month<<year;

    QString date_str;
    date_str.append(QString::number(year)).append("-")
            .append(QString::number(month)).append("-")
            .append(QString::number(day));

    return date_str;
}

QStringList SuperDog::gethostMac()
{
    QList<QNetworkInterface> nets = QNetworkInterface::allInterfaces();// 获取所有网络接口列表

    QStringList mac_list;

    int nCnt = nets.count();
    QString strMacAddr = "";
    for(int i = 0; i < nCnt; i ++)
    {
        // 如果此网络接口被激活并且正在运行并且不是回环地址，则就是我们需要找的Mac地址
        /*if(nets[i].flags().testFlag(QNetworkInterface::IsUp) && nets[i].flags().testFlag(QNetworkInterface::IsRunning)
                && !nets[i].flags().testFlag(QNetworkInterface::IsLoopBack))*/
        {
            strMacAddr = nets[i].hardwareAddress();
            qDebug()<<u8"本机Mac地址："<<strMacAddr;
            mac_list.append(strMacAddr);
//            break;
        }
    }
    return mac_list;
}

QString SuperDog::encryption(QString str)//加密
{
    QByteArray strPwdMd5 = str.toLatin1().toBase64();
//    qDebug()<<strPwdMd5;

    return strPwdMd5;
}

QString SuperDog::Decrypt(QString str)//解密
{
    QByteArray strPwdMd5 = QByteArray::fromBase64(str.toLatin1());
//    qDebug()<<strPwdMd5;

    return strPwdMd5;
}

void SuperDog::reg()//写入第一次启动时间
{
    if(!read_reg())
    {
        return;
    }
    QSettings settings("HKEY_CURRENT_USER\\Software\\Ess_Robot",QSettings::NativeFormat);
    QString reg_str=encryption(get_time());
    settings.setValue("reg", reg_str);//写
}

bool SuperDog::read_reg()//读取第一次启动时间
{
    QSettings settings("HKEY_CURRENT_USER\\Software\\Ess_Robot",QSettings::NativeFormat);
    QString value = settings.value("reg", "error").toString();//读
    if(value=="error")
    {
        return false;
    }
    first_reg_date=Decrypt(value);
    return true;
}

bool SuperDog::check_time()
{
    QString now_date="2019-4-27";//当前日期
    read_reg();

    QString endtime =now_date;
    QString starttime = first_reg_date;

    QDateTime start = QDateTime::fromString(starttime, "yyyy-M-dd");
    QDateTime end = QDateTime::fromString(endtime, "yyyy-M-dd");


//    qDebug()<<start<<end;

    uint stime = start.toTime_t();
    uint etime = end.toTime_t();

    int ndaysec = 24*60*60;
//    qDebug() << "Day = " << (etime - stime)/(ndaysec);

    return 0;
}

QString SuperDog::get_limit_date()
{
    int limit_date=0;

    QSettings settings("./config.ini", QSettings::IniFormat);

    QString value = settings.value("reg", "error").toString();//读

    QString day;

    if (value=="error")
    {
        day = "error";
        return day;;
    }

    QString limit_str=Decrypt(value);

    QStringList str_list=limit_str.split(";");

    QString mac;
    if(str_list.size()>1)
    {
        mac=limit_str.split(";").at(0);

        day=limit_str.split(";").at(1);
    }

    mac.replace("-",":");

//    qDebug()<<mac<<day;

    QStringList mac_list=gethostMac();

    for (int i=0;i<mac_list.size();i++)
    {
        if (mac.compare(mac_list.at(i),Qt::CaseInsensitive) == 0)
        {
            return day;;
        }
    }

    day = "error";

    limit_date=day.toInt();

    return day;
}

void SuperDog::set_limit_date(QString day,QString mac)
{
    QSettings settings("./config.ini", QSettings::IniFormat);

    QString limit_str;
    limit_str=mac.append(";").append(day);

    QString reg_str=encryption(limit_str);

    settings.setValue("reg", reg_str);//写
}
