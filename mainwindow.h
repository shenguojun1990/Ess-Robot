﻿#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "EssCom.h"
#include "DataCenter.h"
#include "TcpServer.h"
#include <QTimer>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void get_com_list();

    void get_server_list();

    void initUI();

    void get_limit_date();//获取到期日期

    void closeEvent(QCloseEvent *event);

	void set_ui(bool status);

private slots:
    void com_opend_slots();
    void com_closed_slots();
    void ess_setting_finished_slots();
    void ess_setting_unfinished_slots();

    void completed_count_slots();

    void ess_finished_slots();

	void to_next_slots();

    void tcp_server_close_slots();

    void robot_recv_slots(RECV_DATA);

    void on_ess_setting_pushButton_clicked();

    void on_start_pushButton_clicked();

    void on_stop_pushButton_clicked();

    void on_tcpServer_listen_pushButton_clicked();

    void on_setting_action_triggered();

    void on_exit_action_triggered();

    void on_back_pushButton_clicked();

    void on_openCom_pushButton_2_clicked();

    void on_reg_action_triggered();

    void on_comboBox_activated(const QString &arg1);

public slots:
    void update_UI();

private:
    Ui::MainWindow *ui;
    EssCom *m_essCom;
    TcpServer *m_TcpServer;

    QTimer *timer;

signals:
	void ess_setting_unfinished();
	void ess_finished();
};

#endif // MAINWINDOW_H
