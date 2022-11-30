#ifndef DIALOGSERIALPORT_H
#define DIALOGSERIALPORT_H

#include <QDialog>
#include <QSerialPort>        //提供访问串口的功能
#include <QSerialPortInfo>    //提供系统中存在的串口的信息
#include <QMessageBox>

namespace Ui {
class DialogSerialPort;
}

class DialogSerialPort : public QDialog
{
    Q_OBJECT

public:
    explicit DialogSerialPort(QWidget *parent = nullptr);
    ~DialogSerialPort();

    void initPort();  // 初始化串口
    void PortRefresh(); // 刷新串口

private:
    Ui::DialogSerialPort *ui;

signals:
    void SerialPortConnect(const QString, QString);
private slots:
    void on_btnPortConnect_clicked();

    void on_btnPortRefresh_clicked();
};

#endif // DIALOGSERIALPORT_H
