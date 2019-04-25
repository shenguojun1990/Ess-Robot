#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QMessageBox>
#include "SuperDog.h"
#include <QDateTime>
#include <QInputDialog>
#include <QSettings>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle(tr(u8"机器人-静电枪控制程序"));

    ui->stackedWidget->setCurrentIndex(0);

    m_essCom=new EssCom();

    m_TcpServer=new TcpServer();

    get_com_list();

    get_server_list();

    connect(m_essCom,SIGNAL(com_opened()),this,SLOT(com_opend_slots()));
    connect(m_essCom, SIGNAL(com_closed()), this, SLOT(com_closed_slots()));
    connect(m_essCom, SIGNAL(ess_setting_finished()), this, SLOT(ess_setting_finished_slots()));
    connect(m_essCom, SIGNAL(ess_setting_unfinished()), this, SLOT(ess_setting_unfinished_slots()));
    connect(m_essCom, SIGNAL(completed_count()), this, SLOT(completed_count_slots()));
    connect(m_essCom, SIGNAL(ess_finished()), this, SLOT(ess_finished_slots()));

    connect(m_TcpServer, SIGNAL(tcp_server_close()), this, SLOT(tcp_server_close_slots()));
    connect(m_TcpServer, SIGNAL(robot_inpos()), this, SLOT(robot_inpos_slots()));

    initUI();

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update_UI()));
    timer->start(100);

    on_tcpServer_listen_pushButton_clicked();
    on_openCom_pushButton_2_clicked();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QMessageBox::StandardButton rb =QMessageBox::question(this, u8"提醒", QString(u8"是否关闭？"), QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes);
    if(rb==QMessageBox::Yes)
    {
        //静电枪停下
        m_essCom->sendData(AB);
        m_essCom->Com_Close();

        m_TcpServer->TcpServerClose();
        event->accept();
    }
    else if(rb==QMessageBox::No)
    {
        event->ignore();
    }
}

void MainWindow::update_UI()
{
    //客户端已连接
    if(DC.tcp_server_flag)
    {
        ui->tcp_server_connect_status_label->setStyleSheet("background-color: rgb(0, 0, 0);\ncolor: rgb(0, 255, 0);");
        ui->tcp_server_connect_status_label->setText(tr(u8"已连接"));
    }
    else
    {
        ui->tcp_server_connect_status_label->setStyleSheet("background-color: rgb(0, 0, 0);\ncolor: rgb(255, 0, 0);");
        ui->tcp_server_connect_status_label->setText(tr(u8"未连接"));
    }

    //串口已打开
    if(DC.com_flag)
    {
        ui->com_open_status_label->setStyleSheet("background-color: rgb(0, 0, 0);\ncolor: rgb(0, 255, 0);");
        ui->com_open_status_label->setText(tr(u8"已打开"));

        ui->com_open_status_label_2->setStyleSheet("background-color: rgb(0, 0, 0);\ncolor: rgb(0, 255, 0);");
        ui->com_open_status_label_2->setText(tr(u8"已打开"));
    }
    else
    {
        ui->com_open_status_label->setStyleSheet("background-color: rgb(0, 0, 0);\ncolor: rgb(255, 0, 0);");
        ui->com_open_status_label->setText(tr(u8"未打开"));

        ui->com_open_status_label_2->setStyleSheet("background-color: rgb(0, 0, 0);\ncolor: rgb(255, 0, 0);");
        ui->com_open_status_label_2->setText(tr(u8"未打开"));
    }

    //监听已打开
    if(DC.tcp_listen_flag)
    {
        ui->tcp_server_listen_status_label->setStyleSheet("background-color: rgb(0, 0, 0);\ncolor: rgb(0, 255, 0);");
        ui->tcp_server_listen_status_label->setText(tr(u8"已监听"));

        ui->tcp_server_listen_status_label_2->setStyleSheet("background-color: rgb(0, 0, 0);\ncolor: rgb(0, 255, 0);");
        ui->tcp_server_listen_status_label_2->setText(tr(u8"已监听"));
    }
    else
    {
        ui->tcp_server_listen_status_label->setStyleSheet("background-color: rgb(0, 0, 0);\ncolor: rgb(255, 0, 0);");
        ui->tcp_server_listen_status_label->setText(tr(u8"未监听"));

        ui->tcp_server_listen_status_label_2->setStyleSheet("background-color: rgb(0, 0, 0);\ncolor: rgb(255, 0, 0);");
        ui->tcp_server_listen_status_label_2->setText(tr(u8"未监听"));
    }
}

void MainWindow::initUI()
{
//    ui->openCom_pushButton->setStyleSheet("QPushButton{background-color:grey;}");

    get_limit_date();
}

void MainWindow::get_limit_date()
{
    SuperDog sd;
    sd.read_reg();

    QString day = sd.get_limit_date();

    if (day== "error")//未注册
    {
        ui->limit_day_label->setText(tr(u8"未注册"));
    }
    else if (day == "0")//∞
    {
        ui->limit_day_label->setText(tr(u8"∞"));
    }
    else
    {
//        qDebug() << DC.first_reg_date;

        QString starttime = DC.first_reg_date;

        QDateTime start = QDateTime::fromString(starttime, "yyyy-M-dd");
        QDateTime end = QDateTime::fromString(day, "yyyy-MM-dd");

//        qDebug() << end.toString("yyyy-MM-dd");

        ui->limit_day_label->setText(end.toString("yyyy-MM-dd"));
    }



}


void MainWindow::get_com_list()
{
    for(int i=0;i<DC.serialPortInfo.size();i++)
    {
        ui->comlist_comboBox_2->addItem(DC.serialPortInfo.at(i).portName());
    }
}

void MainWindow::get_server_list()
{
    for(int i=0;i<DC.server_ip_list.size();i++)
    {
        ui->tcpServer_ip_comboBox->addItem(DC.server_ip_list.at(i));
    }
}

void MainWindow::com_opend_slots()
{
    //    ui->openCom_pushButton->setStyleSheet("QPushButton{background-color:lightgreen;}");
    ui->openCom_pushButton_2->setText(tr(u8"关闭"));
}

void MainWindow::com_closed_slots()
{
    //    ui->openCom_pushButton->setStyleSheet("QPushButton{background-color:grey;}");
    ui->openCom_pushButton_2->setText(tr(u8"打开"));
}

void MainWindow::ess_setting_finished_slots()
{
    ui->ess_setting_finished_label->setText(tr(u8"已写入"));
    ui->ess_setting_finished_label->setStyleSheet("background-color: rgb(0, 0, 0);\ncolor: rgb(0, 255, 0);");
}

void MainWindow::ess_setting_unfinished_slots()
{
    ui->ess_setting_finished_label->setText(tr(u8"未写入"));
    ui->ess_setting_finished_label->setStyleSheet("background-color: rgb(0, 0, 0);\ncolor: rgb(255, 0, 0);");
}

void MainWindow::completed_count_slots()
{
    ui->completed_label->setText(QString::number(DC.completed_count));
}

void MainWindow::ess_finished_slots()
{
    QMessageBox::information(this, u8"提醒", u8"本次测试完成\n"
                                           "请重新设置静电枪参数",
                             QMessageBox::Yes, QMessageBox::Yes);
    return;
}

void MainWindow::tcp_server_close_slots()
{
    ui->tcpServer_listen_pushButton->setText(tr(u8"监听"));
}

void MainWindow::robot_inpos_slots()
{
    on_start_pushButton_clicked();
}

void MainWindow::on_voltage_lineEdit_editingFinished()
{
    //    float vol=ui->voltage_lineEdit->text().toFloat();
    //    qDebug()<<"vol:"<<vol;

    //    DC.voltage=qAbs(vol);
    //    if(vol>=0)//电压值为正
    //    {
    //        DC.prolarity=Positive;
    //    }
    //    else//电压值为负
    //    {
    //        DC.prolarity=Negative;
    //    }
    //    m_essCom->sendData(PL);
}


void MainWindow::on_Discharge_comboBox_currentIndexChanged(int index)
{
    (void) index;
    //    QString mode=ui->Discharge_comboBox->currentText();
    //    if(mode==tr("直接接触"))
    //    {
    //        DC.discharge=Contect;
    //    }
    //    else
    //    {
    //        DC.discharge=Air;
    //    }
    //    m_essCom->sendData(PH);
}

void MainWindow::on_ess_setting_pushButton_clicked()
{
    //接触方式
    QString discharge_mode=ui->Discharge_comboBox->currentText();
    if(discharge_mode==tr(u8"直接接触"))
    {
        DC.discharge=Contect;
    }
    else
    {
        DC.discharge=Air;
    }

    //触发方式
    QString trigger_mode=ui->Trigger_comboBox->currentText();
    if(trigger_mode==tr(u8"本机"))
    {
        DC.trigger=Controller;
    }
    else if(trigger_mode==tr(u8"枪"))
    {
        DC.trigger=Gun;
    }
    else
    {
        DC.trigger=External;
    }

    //电压
    float vol=ui->voltage_lineEdit->text().toFloat();
    //    qDebug()<<"vol:"<<vol;
    DC.voltage=qAbs(vol);//电压值
    if(vol>=0)//电压值为正
    {
        DC.prolarity=Positive;
    }
    else//电压值为负
    {
        DC.prolarity=Negative;
    }

    //间隔时间(S)
    int interval=ui->interval_lineEdit->text().toInt();
    if(interval<=0)
    {
        QMessageBox::information(this, u8"提醒", u8"间隔时间非法", QMessageBox::Yes, QMessageBox::Yes);
        ui->interval_lineEdit->setFocus();
        return;
    }
    DC.interval=interval;

    //重复次数
    int repeat_count=ui->repeat_count_lineEdit->text().toInt();
    if(repeat_count<=0)
    {
        QMessageBox::information(this, u8"提醒", u8"间隔时间非法", QMessageBox::Yes, QMessageBox::Yes);
        ui->repeat_count_lineEdit->setFocus();
        return;
    }
    DC.repeat_count=repeat_count;

    m_essCom->sendData(PH);
}

void MainWindow::on_start_pushButton_clicked()
{
    qDebug()<<u8"===开始===";
    QString discharge_str="";
    QString trigger_str="";
    QString vol_str=QString::number((double)DC.voltage);
    QString count_str=QString::number(DC.repeat_count);
    QString interval_str=QString::number(DC.interval);
    if(DC.discharge==Contect)
    {
        discharge_str=tr(u8"直接接触");
    }
    else
    {
        discharge_str=tr(u8"空气接触");
    }

    if(DC.trigger==Controller)
    {
        discharge_str=tr(u8"主机触发");
    }
    else if(DC.trigger==Gun)
    {
        discharge_str=tr(u8"枪触发");
    }
    else
    {
        discharge_str=tr(u8"外部触发");
    }
    SuperDog sd;

    //检测软件狗
    if(!sd.read_dog())
    {
        qDebug()<<u8"软件狗未识别";
        QMessageBox::information(nullptr,u8"提醒",u8"未检测到软件狗，请联系厂商",QMessageBox::Ok,QMessageBox::Ok);
        return;
    }

    //检测使用期限
    QString curr_date_str = sd.get_time();

    QString limit_date_str = ui->limit_day_label->text();
    if (limit_date_str==tr(u8"未注册"))
    {
        qDebug()<<u8"未注册";
        QMessageBox::information(this, u8"提醒", u8"未注册，请联系厂商", QMessageBox::Yes, QMessageBox::Yes);
        return;
    }
    else if(limit_date_str != tr(u8"∞"))
    {
        QDateTime curr_date = QDateTime::fromString(curr_date_str, "yyyy-M-dd");
        QDateTime limit_date = QDateTime::fromString(limit_date_str, "yyyy-MM-dd");

//        qDebug()<<curr_date;
//        qDebug()<<limit_date;
//        qDebug()<< curr_date.toTime_t() - limit_date.toTime_t();

        if (curr_date.toTime_t() - limit_date.toTime_t() < 0)
        {
            qDebug()<<u8"使用期限已到";
            QMessageBox::information(this, u8"提醒", u8"使用期限已到，请联系厂商", QMessageBox::Yes, QMessageBox::Yes);
            return;
        }
    }

    //检测串口打开状态
    if(ui->com_open_status_label->text()!=tr(u8"已打开"))
    {
        qDebug()<<u8"PC串口打开失败";
        QMessageBox::information(this, u8"提醒", u8"PC串口打开失败\n"
                                               "请手动打开", QMessageBox::Yes, QMessageBox::Yes);
        ui->stackedWidget->setCurrentIndex(1);
        return;
    }

    //检测tcp服务端监听打开状态
    if(ui->tcp_server_listen_status_label->text()!=tr(u8"已监听"))
    {
        qDebug()<<u8"TCP服务端监听打开失败";
        QMessageBox::information(this, u8"提醒", u8"TCP服务端监听打开失败\n"
                                               "请手动打开", QMessageBox::Yes, QMessageBox::Yes);
        ui->stackedWidget->setCurrentIndex(1);
        return;
    }

    //检测静电枪参数写入状态
    if(ui->ess_setting_finished_label->text()!=tr(u8"已写入"))
    {
        qDebug()<<u8"静电枪参数尚未写入";
        QMessageBox::information(this, u8"提醒", u8"静电枪参数尚未写入", QMessageBox::Yes, QMessageBox::Yes);
        return;
    }

    //检测机器人连接状态
    if(ui->tcp_server_connect_status_label->text()!=tr(u8"已连接"))
    {
        qDebug()<<u8"机器人尚未接入";
        QMessageBox::information(this, u8"提醒", u8"机器人尚未接入", QMessageBox::Yes, QMessageBox::Yes);
        return;
    }


    QMessageBox::StandardButton rb =QMessageBox::information(this, u8"提醒", QString(u8"请确认：\n"
                                                                                   "接触方式：%1\n"
                                                                                   "触发方式：%2\n"
                                                                                   "电压值:%3(千伏)\n"
                                                                                   "间隔时间:%4(秒)\n"
                                                                                   "重复次数:%5(次)")
                                                             .arg(discharge_str)
                                                             .arg(trigger_str)
                                                             .arg(vol_str)
                                                             .arg(interval_str)
                                                             .arg(count_str)
                                                             , QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes);

    if(rb==QMessageBox::No)
    {
        return;
    }
    else
    {
        m_essCom->sendData(AA);
    }

}

void MainWindow::on_stop_pushButton_clicked()
{
    m_essCom->sendData(AD);
}

void MainWindow::on_tcpServer_listen_pushButton_clicked()
{
    //    if(ui->openCom_pushButton->text()== tr(u8"打开"))
    //    {
    //        qDebug()<<ui->comlist_comboBox->currentText();
    //        QString portName= ui->comlist_comboBox->currentText();//端口号
    //        m_essCom->Com_Open(portName);
    //    }
    //    else
    //    {
    //        m_essCom->Com_Close();
    //    }
    if(ui->tcpServer_listen_pushButton->text()== tr(u8"监听"))
    {
        bool ret=m_TcpServer->TcpServerListen("8899");
        if(ret)
        {
            ui->tcpServer_listen_pushButton->setText(tr(u8"断开"));
        }
    }
    else
    {
        m_TcpServer->TcpServerClose();
    }

}

void MainWindow::on_setting_action_triggered()
{
   ui->stackedWidget->setCurrentIndex(1);
}

void MainWindow::on_exit_action_triggered()
{
    this->close();
}

void MainWindow::on_back_pushButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::on_openCom_pushButton_2_clicked()
{
    //    qDebug()<<ui->openCom_pushButton->text();
    if(ui->openCom_pushButton_2->text()== tr(u8"打开"))
    {
//        qDebug()<<ui->comlist_comboBox_2->currentText();
        QString portName= ui->comlist_comboBox_2->currentText();//端口号
        m_essCom->Com_Open(portName);
    }
    else
    {
        m_essCom->Com_Close();
    }
}

void MainWindow::on_reg_action_triggered()
{
    bool isOK;
    QString reg_str = QInputDialog::getText(this, u8"注册码",
                                                       u8"请输入注册码：",
                                                       QLineEdit::Normal,
                                                       "",
                                                       &isOK);
    if(isOK)
    {
        SuperDog sd;

        QString limit_str=sd.Decrypt(reg_str);
//        qDebug()<<limit_str;


        QString mac="";

        QString day="";

        QStringList str_list=limit_str.split(";");

        if(str_list.size()>1)
        {
            mac=limit_str.split(";").at(0);

            day=limit_str.split(";").at(1);
        }




        mac.replace("-",":");

//        qDebug()<<"on_reg_action_triggered:"<<mac<<day;

        QStringList mac_list=sd.gethostMac();

        bool reg_flag=false;

        for (int i=0;i<mac_list.size();i++)
        {
            if (mac.compare(mac_list.at(i),Qt::CaseInsensitive) == 0 &&
                    mac.length()>0)
            {
                //注册码合法
                reg_flag=true;
            }
        }
        if(reg_flag)
        {
            QSettings settings("./config.ini", QSettings::IniFormat);
            settings.setValue("reg", reg_str);//写
            get_limit_date();
        }
        else
        {
            QMessageBox::information(this, u8"提醒", QString(u8"注册码：%1不合法\n"
                                                   "请联系厂商").arg(reg_str), QMessageBox::Yes, QMessageBox::Yes);
            return;
        }
    }
}
