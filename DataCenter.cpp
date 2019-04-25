#include "DataCenter.h"

DataCenter::DataCenter(QObject *parent) : QObject(parent)
{
    serialPortInfo = QSerialPortInfo::availablePorts();

    com_flag=false;

    tcp_server_flag=false;

    tcp_listen_flag=false;
}
