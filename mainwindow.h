#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
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
#include <QStandardItemModel>
#include <QItemSelectionModel>

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
    QByteArray networkBuffer;  // 网络接收数据缓冲区

    sshConnectWindow *sshConWin = NULL;  // ssh连接窗口

    DataVisionNineAxisSensor *datavisionNas = NULL;

    DataVisionDynamixel *datavisionDyna = NULL;

    DataVisionZmp *datavisionZmp = NULL;

    // 用来显示tableview数据
    QStandardItemModel *theModel;
    QItemSelectionModel *theSelection;

    //当前打开的文件名
    QString currentFileName;
    //存储步态零点
    QString currentZeroPoint;
    //存储步态数据
    QStringList currentGaitFrames;
    //存储步态帧率
    int currentGaitRate=0;

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;

private slots:
    void on_serialPort_triggered();

    void on_networkCom_triggered();

    void on_nineAxisSensor_triggered();

    void on_steeringEngine_triggered();

    void on_zmpTrajectory_triggered();

    void on_tableCurrentChanged(const QModelIndex& current,const QModelIndex& previous);
    void on_tableSelectionChanged(const QItemSelection& current ,const QItemSelection& previous);

    void onSerialPortConnect(const QString, QString);
    void sendSerialPortData();
    void onReadyRead();
    void on_btnCancelSerialPortCon_clicked();

    void on_openFile_triggered();
    void on_saveFile_triggered();

    void onConnectNetwork(QString IP, quint16 port);
    void onDisConnectNetwork();
    void onNetworkConnected();
    void onNetworkDisconnected();
    void onSocketStateChange(QAbstractSocket::SocketState);
    void onSocketReadyRead();

    void on_btnCancelNetCon_clicked();

    void on_sshConnect_triggered();

    void on_pushButtonClearText_clicked();


    void on_tableView_clicked(const QModelIndex &index);

    void on_anotherSaveFile_triggered();



    void on_btnAppendFrame_clicked();

    void on_btnInsertFrame_clicked();

    void on_btnDelCurFrame_clicked();

    void send_horizontalSlider_Data();

    void on_horizontalSlider_valueChanged(int value);

    void on_horizontalSlider_2_valueChanged(int value);

    void on_horizontalSlider_3_valueChanged(int value);

    void on_horizontalSlider_4_valueChanged(int value);

    void on_horizontalSlider_5_valueChanged(int value);

    void on_horizontalSlider_6_valueChanged(int value);

    void on_horizontalSlider_7_valueChanged(int value);

    void on_horizontalSlider_8_valueChanged(int value);

    void on_horizontalSlider_9_valueChanged(int value);

    void on_horizontalSlider_10_valueChanged(int value);

    void on_horizontalSlider_11_valueChanged(int value);

    void on_horizontalSlider_12_valueChanged(int value);

    void on_horizontalSlider_13_valueChanged(int value);

    void on_horizontalSlider_14_valueChanged(int value);

    void on_horizontalSlider_15_valueChanged(int value);

    void on_horizontalSlider_16_valueChanged(int value);

    void on_horizontalSlider_17_valueChanged(int value);

    void on_horizontalSlider_18_valueChanged(int value);

    void on_horizontalSlider_19_valueChanged(int value);

    void on_horizontalSlider_20_valueChanged(int value);

    void on_spinBox_valueChanged(int arg1);

    void on_spinBox_2_valueChanged(int arg1);

    void on_spinBox_3_valueChanged(int arg1);

    void on_spinBox_4_valueChanged(int arg1);

    void on_spinBox_5_valueChanged(int arg1);

    void on_spinBox_6_valueChanged(int arg1);

    void on_spinBox_7_valueChanged(int arg1);

    void on_spinBox_8_valueChanged(int arg1);

    void on_spinBox_9_valueChanged(int arg1);

    void on_spinBox_10_valueChanged(int arg1);

    void on_spinBox_11_valueChanged(int arg1);

    void on_spinBox_12_valueChanged(int arg1);

    void on_spinBox_13_valueChanged(int arg1);

    void on_spinBox_14_valueChanged(int arg1);

    void on_spinBox_15_valueChanged(int arg1);

    void on_spinBox_16_valueChanged(int arg1);

    void on_spinBox_17_valueChanged(int arg1);

    void on_spinBox_18_valueChanged(int arg1);

    void on_spinBox_19_valueChanged(int arg1);

    void on_spinBox_20_valueChanged(int arg1);

    void on_btnReturnCentralValue_clicked();

    void on_btnRecordCurFrame_clicked();

    void on_btnExecPreFrame_clicked();

    void on_btnExecLaterFrame_clicked();

    void on_btnResetFrame_clicked();

    void on_btnExecList_clicked();

    void on_newFile_triggered();


    void on_rbtnAdjustZero_clicked();

    void on_rbtnAdjustGait_clicked();

private:
    Ui::MainWindow *ui;
    void iniModelFromStringList(QStringList& fileContent);
    void iniModelFromStringList_zeroPoint(QStringList& fileContent);

};
#endif // MAINWINDOW_H
