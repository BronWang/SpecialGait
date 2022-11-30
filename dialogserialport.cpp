#include "dialogserialport.h"
#include "ui_dialogserialport.h"
#include <QDebug>

DialogSerialPort::DialogSerialPort(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogSerialPort)
{
    ui->setupUi(this);
}

DialogSerialPort::~DialogSerialPort()
{
    delete ui;
}

void DialogSerialPort::initPort()
{
    // 读取本机的串口信息
    foreach (auto info, QSerialPortInfo::availablePorts()) {
//        qDebug()<<"Name:"<<info.portName();
//        qDebug()<<"Description:"<<info.description();
//        qDebug()<<"Manufacturer:"<<info.manufacturer();

        // 添加本机可使用的串口号到UI界面中
        QSerialPort serial;
        serial.setPort(info);
        if(serial.open(QIODevice::ReadWrite)){
            ui->comboBoxPort->addItem(info.portName());
            serial.close();
        }else{
            QMessageBox::information(this,"提示",QString("识别串口%1失败 ").arg(info.portName()));
        }

    }
    QStringList listBaud; // 只设置串口的波特率，其它默认
    listBaud<<"1200"<<"1800"<<"2400"<<"4800"<<"9600"
          <<"14400"<<"19200"<<"38400"<<"56000"<<"57600"
         <<"76800"<<"115200"<<"128000"<<"256000";
    ui->comboBoxBaud->addItems(listBaud);
    ui->comboBoxBaud->setCurrentIndex(11);
    ui->btnPortConnect->setEnabled(true);
}

void DialogSerialPort::on_btnPortConnect_clicked()
{
    emit SerialPortConnect(ui->comboBoxPort->currentText(),ui->comboBoxBaud->currentText());
}


void DialogSerialPort::PortRefresh()
{
    ui->comboBoxPort->clear();
    ui->comboBoxBaud->clear();
    foreach (auto info, QSerialPortInfo::availablePorts()) {
        // 添加本机可使用的串口号到UI界面中
            ui->comboBoxPort->addItem(info.portName());
    }
    QStringList listBaud; // 只设置串口的波特率，其它默认
    listBaud<<"1200"<<"1800"<<"2400"<<"4800"<<"9600"
          <<"14400"<<"19200"<<"38400"<<"56000"<<"57600"
         <<"76800"<<"115200"<<"128000"<<"256000";
    ui->comboBoxBaud->addItems(listBaud);
    ui->comboBoxBaud->setCurrentIndex(11);
    ui->btnPortConnect->setEnabled(true);
}


void DialogSerialPort::on_btnPortRefresh_clicked()
{
    PortRefresh();
}

