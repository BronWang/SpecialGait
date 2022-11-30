#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "datavisioncorrectgait.h"
#include "datavisiondynamixel.h"
#include "datavisionnineaxissensor.h"
#include "datavisionzmp.h"
#include "dialogserialport.h"
#include "dialognetworkconnect.h"
#include "sshconnectwindow.h"
#include "packettransformer.h"
#include <QMessageBox>
#include <QTimer>
#include <QLabel>
#include <QDebug>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    bool isSerialSendEnable = false;  // 判断串口是否在发送数据
    bool isNetworkConnect;  // 判断网络是否连接
    QSerialPort *mySerialPort = NULL;
    DialogSerialPort *dlgSerialPort = NULL;  // 串口连接窗口
    QTimer *timer; // 读取串口的定时器
    QByteArray serialPortBuffer;  // 串口接收数据缓冲区

    DialogNetworkConnect *dlgNetwork = NULL;  // 网络连接窗口
    QTcpSocket *tcpClient = NULL;  // 客户端socket
    QLabel *labelSocketState;  // 客户端socket状态标签
    QLabel *labelSeverIP;  // 服务端IP标签
    QByteArray networkBuffer;  // 串口接收数据缓冲区

    sshConnectWindow *sshConWin = NULL;  // ssh连接窗口




    DataVisionNineAxisSensor *datavisionNas = NULL;


    DataVisionCorrectGait *datavisionCorrectGait = NULL;


    DataVisionDynamixel *datavisionDyna = NULL;


    DataVisionZmp *datavisionZmp = NULL;

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;

private slots:
    void on_serialPort_triggered();

    void on_networkCom_triggered();

    void on_nineAxisSensor_triggered();

    void on_gaitCorrect_triggered();

    void on_steeringEngine_triggered();

    void on_zmpTrajectory_triggered();

    void onSerialPortConnect(const QString, QString);
    void sendSerialPortData();
    void onReadyRead();
    void on_btnCancelSerialPortCon_clicked();

    void on_openFile_triggered();
    void on_saveFile_triggered();

    void on_btnfilePathSensorDynaZMP_clicked();

    void onConnectNetwork(QString IP, quint16 port);
    void onDisConnectNetwork();
    void onNetworkConnected();
    void onNetworkDisconnected();
    void onSocketStateChange(QAbstractSocket::SocketState);
    void onSocketReadyRead();

    void on_btnCancelNetCon_clicked();

    void on_btnSelSdzFilePath_clicked();

    void on_sshConnect_triggered();

    void on_btnStartGetData_clicked();

    void on_pushButtonClearText_clicked();

    void on_btnStopGetData_clicked();

private:
    Ui::MainWindow *ui;    

};
#endif // MAINWINDOW_H
