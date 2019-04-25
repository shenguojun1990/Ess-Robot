#ifndef SUPERDOG_H
#define SUPERDOG_H

#include "dog_api.h"
#include <QObject>

class SuperDog : public QObject
{
    Q_OBJECT
public:
    explicit SuperDog(QObject *parent = nullptr);
public:
    dog_handle_t handle;

    void softdog_login();

    bool read_dog();

    QString get_time();

    QStringList gethostMac();

    QString encryption(QString str);//加密

    QString Decrypt(QString str);//解密

    void reg();

    bool read_reg();

    QString first_reg_date;//第一次注册时间

    bool check_time();//检测已使用日期

    QString get_limit_date();//获取使用期限

    void set_limit_date(QString day, QString mac);//设置使用期限

signals:

public slots:
};

#endif // SUPERDOG_H
