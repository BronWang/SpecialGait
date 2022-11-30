#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDir>
#include <QFileDialog>


#pragma execution_character_set("utf-8")

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("机器人平衡");
    ui->checkBoxSerial->setEnabled(false);
    ui->checkBoxNetwork->setEnabled(false);
    ui->btnCancelSerialPortCon->setEnabled(false);
    ui->btnCancelNetCon->setEnabled(false);
    ui->btnStopGetData->setEnabled(false);
    ui->btnStartGetData->setEnabled(true);

    labelSocketState = new QLabel("Socket状态：");
    labelSocketState->setMinimumWidth(250);
    labelSeverIP = new QLabel("服务端IP：");
    labelSeverIP->setMinimumWidth(250);
    ui->statusbar->addWidget(labelSocketState);
    ui->statusbar->addWidget(labelSeverIP);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if ((tcpClient != NULL)&&(tcpClient->state() == QAbstractSocket::ConnectedState)){
        tcpClient->disconnectFromHost();
    }
    event->accept();
}


void MainWindow::on_serialPort_triggered()
{
    if (dlgSerialPort == NULL){
        dlgSerialPort = new DialogSerialPort(this);
        dlgSerialPort->initPort();
    }
    dlgSerialPort->setWindowTitle("连接串口");
    // 功能细节
    connect(dlgSerialPort, SIGNAL(SerialPortConnect(const QString, QString)), this, SLOT(onSerialPortConnect(const QString, QString)));  // 连接串口窗口连接串口发送的信号与主窗口的槽函数

    dlgSerialPort->show();
}


void MainWindow::on_networkCom_triggered()
{
    if (dlgNetwork == NULL)
        dlgNetwork = new DialogNetworkConnect(this);
    dlgNetwork->setWindowTitle("连接网络");
    // 功能细节
    if (tcpClient == NULL) tcpClient = new QTcpSocket(this);  //创建socket变量

    dlgNetwork->getLocalIPv4();

    connect(dlgNetwork,SIGNAL(connectNetwork(QString,quint16)),this,SLOT(onConnectNetwork(QString,quint16)));
    connect(dlgNetwork,SIGNAL(disConnectNetwork()),this,SLOT(onDisConnectNetwork()));
    connect(tcpClient,SIGNAL(connected()),this,SLOT(onNetworkConnected()));
    connect(tcpClient,SIGNAL(disconnected()),this,SLOT(onNetworkDisconnected()));

    connect(tcpClient,SIGNAL(stateChanged(QAbstractSocket::SocketState)),
            this,SLOT(onSocketStateChange(QAbstractSocket::SocketState)));
    connect(tcpClient,SIGNAL(readyRead()),
            this,SLOT(onSocketReadyRead()));

    dlgNetwork->show();
}


void MainWindow::on_nineAxisSensor_triggered()
{
    if (datavisionNas == NULL)
        datavisionNas = new DataVisionNineAxisSensor(this);
    datavisionNas->setWindowTitle("九轴传感器");
    // 功能细节

    datavisionNas->show();
}


void MainWindow::on_gaitCorrect_triggered()
{
    if (datavisionCorrectGait == NULL)
        datavisionCorrectGait = new DataVisionCorrectGait(this);
    datavisionCorrectGait->setWindowTitle("矫正步态");
    // 功能细节

    datavisionCorrectGait->show();
}


void MainWindow::on_steeringEngine_triggered()
{
    if (datavisionDyna == NULL)
        datavisionDyna = new DataVisionDynamixel(this);
    datavisionDyna->setWindowTitle("舵机");
    // 功能细节

    datavisionDyna->show();
}


void MainWindow::on_zmpTrajectory_triggered()
{
    if (datavisionZmp == NULL)
        datavisionZmp = new DataVisionZmp(this);
    datavisionZmp->setWindowTitle("ZMP轨迹");
    // 功能细节

    datavisionZmp->show();
}

void MainWindow::onSerialPortConnect(const QString portName, QString baudRate)
{
    if (portName.isEmpty()) return;
    /*设置并连接串口*/
    if(mySerialPort != NULL){
        mySerialPort->close();
    }else {
        mySerialPort = new QSerialPort(this);
        connect(mySerialPort,SIGNAL(readyRead()),this,SLOT(onReadyRead()));
    }
    mySerialPort->setPortName(portName);
    // 读写方式打开串口
    if(mySerialPort->open(QIODevice::ReadWrite)){
        //设置波特率
        mySerialPort->setBaudRate(baudRate.toInt());
        //设置数据位
        mySerialPort->setDataBits(QSerialPort::Data8);
        //设置校验位
        mySerialPort->setParity(QSerialPort::NoParity);
        //设置流控制
        mySerialPort->setFlowControl(QSerialPort::NoFlowControl);
        //设置停止位
        mySerialPort->setStopBits(QSerialPort::OneStop);

        ui->checkBoxSerial->setChecked(true);
        ui->checkBoxSerial->setText(QString("串口%1已连接").arg(mySerialPort->portName()));
        ui->btnCancelSerialPortCon->setEnabled(true);

    }else{
        QMessageBox::information(this,"提示","串口未打开！请重新尝试打开串口！");
        return;
    }
}

void MainWindow::onReadyRead()
{
    serialPortBuffer = mySerialPort->readAll();
    // 对数据进行处理
    /*******测试代码↓********/
    PacketTransformer packet;
    struct SensorsRaw sensorsRaw;
    QString showtext;
    //ui->plainTextEditGaitData->appendPlainText(serialPortBuffer);
    qDebug()<<serialPortBuffer;
    packet.SetRawPacket(serialPortBuffer);

    packet.TryDestructOnePacket();
    //qDebug()<<QString::number( packet.dspInst->instruction,16);
    //qDebug()<<"1";
    if(packet.dspInst->instruction == INST_INCLINOMETER_REQUIRED){
        sensorsRaw.gyro[0]=(short)packet.dspInst->parameter[0]+(short)packet.dspInst->parameter[1]*256;
        sensorsRaw.gyro[1]=(short)packet.dspInst->parameter[2]+(short)packet.dspInst->parameter[3]*256;
        sensorsRaw.gyro[2]=(short)packet.dspInst->parameter[4]+(short)packet.dspInst->parameter[5]*256;

        sensorsRaw.accel[0]=(short)packet.dspInst->parameter[6]+(short)packet.dspInst->parameter[7]*256;
        sensorsRaw.accel[1]=(short)packet.dspInst->parameter[8]+(short)packet.dspInst->parameter[9]*256;
        sensorsRaw.accel[2]=(short)packet.dspInst->parameter[10]+(short)packet.dspInst->parameter[11]*256;

        sensorsRaw.mag[0]=(short)packet.dspInst->parameter[12]+(short)packet.dspInst->parameter[13]*256;
        sensorsRaw.mag[1]=(short)packet.dspInst->parameter[14]+(short)packet.dspInst->parameter[15]*256;
        sensorsRaw.mag[2]=(short)packet.dspInst->parameter[16]+(short)packet.dspInst->parameter[17]*256;


        showtext = QString::asprintf("gyro:%d,%d,%d,accel:%d,%d,%d,mag:%d,%d,%d\n",
                sensorsRaw.gyro[0],
                sensorsRaw.gyro[1],
                sensorsRaw.gyro[2],
                sensorsRaw.accel[0],
                sensorsRaw.accel[1],
                sensorsRaw.accel[2],
                sensorsRaw.mag[0],
                sensorsRaw.mag[1],
                sensorsRaw.mag[2]
                    );
        ui->plainTextEditGaitData->appendPlainText(showtext);
    }
    if(packet.dspInst->instruction == INST_GET_DXL_ID){
        showtext = QString::asprintf("Exist ID:%d\n",packet.dspInst->parameter[0]);
        ui->plainTextEditGaitData->appendPlainText(showtext);
    }


    /*******测试代码↑********/
    // 暂时借用plainTextEditGaitData测试显示
    //ui->plainTextEditGaitData->appendPlainText(showtext);

    mySerialPort->clear();
    serialPortBuffer.clear();

}


void MainWindow::sendSerialPortData()
{
    // 整理好发送的数据，放进缓冲数组SerialPortBuffer

    /*******测试代码↓********/
    PacketTransformer packet;
    int N = 30;
    packet.dspInst->id=ID_DSP;
    packet.dspInst->instruction=INST_TORQUE_ON;
    packet.dspInst->length=2;
    packet.ConstructPacket();
    serialPortBuffer = packet.GetByteArray();
    mySerialPort->write(serialPortBuffer);
    qDebug()<<serialPortBuffer;
    mySerialPort->waitForReadyRead(10);
    packet.Reset();
    qApp->processEvents();

    packet.dspInst->id=ID_DSP;
    packet.dspInst->instruction=INST_TORQUE_ON;
    packet.dspInst->length=2;
    packet.ConstructPacket();
    serialPortBuffer = packet.GetByteArray();
    mySerialPort->write(serialPortBuffer);
    qDebug()<<serialPortBuffer;
    mySerialPort->waitForReadyRead(10);
    packet.Reset();
    qApp->processEvents();

    for(int i = 1; i<2; i++){
        qDebug()<<"1";
        packet.dspInst->id=ID_DSP;
        packet.dspInst->instruction=INST_SET_DXL_ID;
        packet.dspInst->parameter[0] = (i&0xff);
        packet.dspInst->length=3;
        packet.ConstructPacket();
        serialPortBuffer = packet.GetByteArray();
        mySerialPort->write(serialPortBuffer);
        qDebug()<<serialPortBuffer;
        mySerialPort->waitForReadyRead(10);
        qApp->processEvents();
        packet.Reset();
    }
//    while (isSerialSendEnable){
//        //qDebug()<<"1";
//        packet.dspInst->id=ID_DSP;
//        packet.dspInst->instruction=INST_INCLINOMETER_REQUIRED;
//        packet.dspInst->length=2;
//        packet.ConstructPacket();
//        serialPortBuffer = packet.GetByteArray();
//        mySerialPort->write(serialPortBuffer);
//        //qDebug()<<serialPortBuffer;
//        mySerialPort->waitForReadyRead(10);
//        qApp->processEvents();
//        packet.Reset();

//    }

    /*******测试代码↑********/

    // 发送数据
    // mySerialPort->write(serialPortBuffer);
}


void MainWindow::on_btnCancelSerialPortCon_clicked()
{
    if(ui->btnStopGetData->isEnabled()){
        QMessageBox::warning(this, "警告", "请先停止获取串口数据再断开串口连接！");
        return;
    }
    mySerialPort->close();
    ui->checkBoxSerial->setChecked(false);
    ui->checkBoxSerial->setText("串口已连接");
    ui->btnCancelSerialPortCon->setEnabled(false);
}


void MainWindow::on_openFile_triggered()
{
    // 打开矫正步态文件
    QString curPath=QDir::currentPath();//获取系统当前目录
    QString dlgTitle="打开一个矫正步态文件"; //对话框标题
    QString filter="矫正步态文件(*.cgait)"; //文件过滤器
    QString aFileName=QFileDialog::getOpenFileName(this,dlgTitle,curPath,filter);

    if (aFileName.isEmpty())
        return;
    QFile   aFile(aFileName);


    if (!aFile.exists()) //文件不存在
        return;

    if (!aFile.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    ui->plainTextEditGaitData->setPlainText(aFile.readAll());

    aFile.close();
}


void MainWindow::on_saveFile_triggered()
{
    QString curPath=QDir::currentPath();//获取系统当前目录
    QString dlgTitle="另存为一个矫正步态文件"; //对话框标题
    QString filter="矫正步态文件(*.cgait)"; //文件过滤器
    QString aFileName=QFileDialog::getSaveFileName(this,dlgTitle,curPath,filter);

    if (aFileName.isEmpty())
        return;
    QFile   aFile(aFileName);

    if (!aFile.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    QString str=ui->plainTextEditGaitData->toPlainText();//整个内容作为字符串

    QByteArray  strBytes=str.toUtf8();//转换为字节数组

    aFile.write(strBytes,strBytes.length());  //写入文件

    aFile.close();
    return;
}


void MainWindow::on_btnfilePathSensorDynaZMP_clicked()
{
    QString curPath=QDir::currentPath();//获取系统当前目录
    QString dlgTitle="选择存储文件路径"; //对话框标题
    QString filter="矫正步态文件(*.sdz)"; //文件过滤器
    QString aFileName=QFileDialog::getSaveFileName(this,dlgTitle,curPath,filter);

    if (aFileName.isEmpty())
        return;
    QFile   aFile(aFileName);
    if (!aFile.open(QIODevice::WriteOnly | QIODevice::Text))
        return;
    aFile.close();
    ui->lineEditFilePathSdz->setText(aFileName);

}

void MainWindow::onConnectNetwork(QString IP, quint16 port)
{
    tcpClient->connectToHost(IP, port);
}

void MainWindow::onDisConnectNetwork()
{
    tcpClient->disconnectFromHost();
    //dlgNetwork->clearLocalIP();
    dlgNetwork->clearLocalPort();
}

void MainWindow::onNetworkConnected()
{
    labelSeverIP->setText("服务端IP："+tcpClient->peerAddress().toString());
    dlgNetwork->getLocalIPv4();
    dlgNetwork->setLocaPort(tcpClient->localPort());
    ui->checkBoxNetwork->setChecked(true);
    ui->btnCancelNetCon->setEnabled(true);
}

void MainWindow::onNetworkDisconnected()
{
    labelSeverIP->setText("服务端IP：");
    if (tcpClient->state()==QAbstractSocket::ConnectedState)
        tcpClient->disconnectFromHost();
    dlgNetwork->clearLocalIP();
    dlgNetwork->clearLocalPort();
    ui->checkBoxNetwork->setChecked(false);
}

void MainWindow::onSocketStateChange(QAbstractSocket::SocketState socketState)
{
    // 显示Socket状态的变化
    switch(socketState)
    {
    case QAbstractSocket::UnconnectedState:
        labelSocketState->setText("scoket状态：UnconnectedState");
        break;
    case QAbstractSocket::HostLookupState:
        labelSocketState->setText("scoket状态：HostLookupState");
        break;
    case QAbstractSocket::ConnectingState:
        labelSocketState->setText("scoket状态：ConnectingState");
        break;

    case QAbstractSocket::ConnectedState:
        labelSocketState->setText("scoket状态：ConnectedState");
        break;

    case QAbstractSocket::BoundState:
        labelSocketState->setText("scoket状态：BoundState");
        break;

    case QAbstractSocket::ClosingState:
        labelSocketState->setText("scoket状态：ClosingState");
        break;

    case QAbstractSocket::ListeningState:
        labelSocketState->setText("scoket状态：ListeningState");
    default:
        labelSocketState->setText("scoket状态：");
    }
}

void MainWindow::onSocketReadyRead()
{
    networkBuffer = tcpClient->readAll();


    // 暂时借用plainTextEditGaitData测试显示
    ui->plainTextEditGaitData->appendPlainText(networkBuffer);

}


void MainWindow::on_btnCancelNetCon_clicked()
{
    onDisConnectNetwork();
    ui->btnCancelNetCon->setEnabled(false);
}


void MainWindow::on_btnSelSdzFilePath_clicked()
{
    QString curPath=QDir::currentPath();//获取系统当前目录
    QString dlgTitle="选择存储文件路径"; //对话框标题
    QString filter="矫正步态文件(*.sdz)"; //文件过滤器
    QString aFileName=QFileDialog::getOpenFileName(this,dlgTitle,curPath,filter);

    if (aFileName.isEmpty())
        return;
    QFile   aFile(aFileName);
    if (!aFile.open(QIODevice::WriteOnly | QIODevice::Text))
        return;
    aFile.close();
    ui->lineEditSelSdzFilePath->setText(aFileName);
}


void MainWindow::on_sshConnect_triggered()
{
    if (sshConWin == NULL)
        sshConWin = new sshConnectWindow(this);
    sshConWin->setWindowTitle("ssh连接");
    // 功能细节
    connect(sshConWin,SIGNAL(sigConnectStateChanged(bool,QString,int)),
            sshConWin,SLOT(slotConnectStateChanged(bool,QString,int)));
    connect(sshConWin,SIGNAL(sigDataArrived(QString ,QString , int )),
            sshConWin,SLOT(slotDataArrived(QString ,QString , int )));
    connect(sshConWin,SIGNAL(sigSend(QString)),sshConWin,SLOT(slotSend(QString)));
    connect(sshConWin,SIGNAL(sigDisconnected()),sshConWin,SLOT(slotDisconnected()));
    sshConWin->show();
}


void MainWindow::on_btnStartGetData_clicked()
{
    /*******测试代码↓********/
    ui->btnStopGetData->setEnabled(true);
    ui->btnStartGetData->setEnabled(false);
    if(ui->checkBoxSerial->isChecked()){
        isSerialSendEnable = true;
        sendSerialPortData();
    }
    /*******测试代码↑********/
}


void MainWindow::on_pushButtonClearText_clicked()
{
    ui->plainTextEditGaitData->clear();
}


void MainWindow::on_btnStopGetData_clicked()
{
    ui->btnStopGetData->setEnabled(false);
    ui->btnStartGetData->setEnabled(true);
    isSerialSendEnable = false;
}

