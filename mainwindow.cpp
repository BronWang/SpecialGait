#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDir>
#include <QFileDialog>
#define FixedColumnCount 25
#define FrameRateColumn 25
#define PI 3.1415926


#pragma execution_character_set("utf-8")

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("SpecialGait");
    ui->checkBoxSerial->setEnabled(false);
    ui->checkBoxNetwork->setEnabled(false);
    ui->btnCancelSerialPortCon->setEnabled(false);
    ui->btnCancelNetCon->setEnabled(false);
    ui->btnDelCurFrame->setEnabled(false);
    ui->btnAppendFrame->setEnabled(false);
    ui->btnInsertFrame->setEnabled(false);
    ui->btnRecordCurFrame->setEnabled(false);
    ui->btnExecLaterFrame->setEnabled(false);
    ui->btnExecPreFrame->setEnabled(false);
    ui->btnResetFrame->setEnabled(false);
    ui->btnExecList->setEnabled(false);
    ui->anotherSaveFile->setEnabled(false);
    ui->scrollArea->setEnabled(false);
    ui->btnReturnCentralValue->setEnabled(false);
    ui->rbtnAdjustGait->setChecked(true);
    ui->rbtnAdjustGait->setEnabled(false);
    ui->rbtnAdjustZero->setEnabled(false);

    labelSocketState = new QLabel("Socket status：");
    labelSocketState->setMinimumWidth(250);
    labelSeverIP = new QLabel("Server IP：");
    labelSeverIP->setMinimumWidth(250);
    ui->statusbar->addWidget(labelSocketState);
    ui->statusbar->addWidget(labelSeverIP);

    //tableview部分
    theModel = new QStandardItemModel(5, FixedColumnCount+1, this);
    theModel->setHeaderData(FrameRateColumn,Qt::Horizontal,"帧率");  // 对帧数列单独处理
    theSelection = new QItemSelectionModel(theModel);
    ui->tableView->setModel(theModel);
    ui->tableView->setSelectionModel(theSelection);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tableView->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    connect(theSelection,SIGNAL(currentChanged(QModelIndex,QModelIndex)),this,SLOT(on_tableCurrentChanged(QModelIndex,QModelIndex)));
    connect(theSelection,SIGNAL(selectionChanged(QItemSelection,QItemSelection)),this,SLOT(on_tableSelectionChanged(QItemSelection,QItemSelection)));

    //输入格式限制
    ui->lineEditGaitID->setValidator(new QIntValidator(0,20000,ui->lineEditGaitID));
    ui->lineEditGaitRate->setValidator(new QIntValidator(0,300,ui->lineEditGaitRate));

    //关闭所有用不到的关节
    ui->horizontalSlider->setEnabled(false);
    ui->horizontalSlider_7->setEnabled(false);
    ui->horizontalSlider_11->setEnabled(false);
    ui->horizontalSlider_18->setEnabled(false);
    ui->horizontalSlider_25->setEnabled(false);
    ui->spinBox->setEnabled(false);
    ui->spinBox_7->setEnabled(false);
    ui->spinBox_11->setEnabled(false);
    ui->spinBox_18->setEnabled(false);
    ui->spinBox_25->setEnabled(false);
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

void MainWindow::on_tableCurrentChanged(const QModelIndex &current, const QModelIndex &previous)
{
    if(!current.isValid()){

        return;
    }

}

void MainWindow::on_tableSelectionChanged(const QItemSelection &current, const QItemSelection &previous)
{
    if(current.indexes().empty()){
        ui->btnDelCurFrame->setEnabled(false);
        ui->btnAppendFrame->setEnabled(false);
        ui->btnInsertFrame->setEnabled(false);
        ui->btnRecordCurFrame->setEnabled(false);
        ui->btnExecLaterFrame->setEnabled(false);
        ui->btnExecPreFrame->setEnabled(false);
        ui->btnResetFrame->setEnabled(false);
        ui->scrollArea->setEnabled(false);
        ui->btnReturnCentralValue->setEnabled(false);
        return;
    }
    if(ui->rbtnAdjustZero->isChecked()){
        ui->btnDelCurFrame->setEnabled(false);
        ui->btnAppendFrame->setEnabled(false);
        ui->btnInsertFrame->setEnabled(false);
        ui->btnExecLaterFrame->setEnabled(false);
        ui->btnExecPreFrame->setEnabled(false);
        ui->btnResetFrame->setEnabled(false);
        return;
    }
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
    mySerialPort->close();
    ui->checkBoxSerial->setChecked(false);
    ui->checkBoxSerial->setText("串口已连接");
    ui->btnCancelSerialPortCon->setEnabled(false);
}


void MainWindow::on_openFile_triggered()
{
    // 打开步态序列文件
    QString curPath=QDir::currentPath();//获取系统当前目录
    QString dlgTitle="打开一个步态序列文件"; //对话框标题
    QString filter="步态序列文件(*.txt)"; //文件过滤器
    QString aFileName=QFileDialog::getOpenFileName(this,dlgTitle,curPath,filter);
    currentFileName = aFileName;
    if (aFileName.isEmpty())
        return;
    QFile   aFile(aFileName);


    if (!aFile.exists()) //文件不存在
        return;

    if (!aFile.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    this->setWindowTitle("SpecialGait  "+aFileName);
    QStringList fileContent;
    QTextStream stream(&aFile);
    stream.setCodec("utf-8");
    ui->plainTextEditGaitData->clear();
    while(!stream.atEnd()){
        QString str = stream.readLine();
        fileContent.append(str);
        ui->plainTextEditGaitData->appendPlainText(str);
    }
    aFile.close();
    if(fileContent.isEmpty()){
        QMessageBox::warning(this,"警告","文件为空！");
        return;
    }
    iniModelFromStringList(fileContent);
    ui->anotherSaveFile->setEnabled(true);
    ui->btnExecList->setEnabled(true);
    ui->rbtnAdjustGait->setEnabled(true);
    ui->rbtnAdjustZero->setEnabled(true);
}


void MainWindow::on_saveFile_triggered()
{
    QString aFileName=currentFileName;
    if (aFileName.isEmpty()) return;
    QFile  aFile(aFileName);
    if (!aFile.exists()) //文件不存在
        return;
    if (!aFile.open(QIODevice::WriteOnly | QIODevice::Text))
        return;
    QTextStream stream(&aFile);
    stream.setCodec("utf-8");
    QString str;
    ui->plainTextEditGaitData->clear();
    QString tempstr;
    if(ui->rbtnAdjustZero->isChecked()){
        tempstr = "GaitID";
        //ID不能为空
        QString idstr = ui->lineEditGaitID->text();
        if(idstr.isEmpty()) idstr = "0";
        str = tempstr+"\n"+ idstr;
        stream << str << "\n";
        ui->plainTextEditGaitData->appendPlainText(str);
        tempstr = "GaitRate";
        //帧率不能为空
        QString ratestr = ui->lineEditGaitRate->text();
        if(ratestr.isEmpty()) ratestr = "30";
        currentGaitRate = ratestr.toInt();
        str = tempstr+"\n"+ratestr;
        stream << str << "\n";
        ui->plainTextEditGaitData->appendPlainText(str);
        tempstr = "GaitDescription";
        str = tempstr+"\n"+ui->textEditGaitDesc->toPlainText().replace("\n","");
        stream << str << "\n";
        ui->plainTextEditGaitData->appendPlainText(str);
        tempstr = "zero_point";
        QString temp_zero_point = "";
        //读取表中新的零点数据
        for(int i = 0; i < FixedColumnCount-1; i++){
            temp_zero_point += theModel->item(0,i)->text() + ",";
        }
        temp_zero_point += theModel->item(0,FixedColumnCount-1)->text();
        currentZeroPoint = temp_zero_point;
        str = tempstr+"\n"+temp_zero_point;
        stream << str << "\n";
        ui->plainTextEditGaitData->appendPlainText(str);
        str = "Gait_Frame";
        stream << str << "\n";
        ui->plainTextEditGaitData->appendPlainText(str);
        for(int i = 0;i<currentGaitFrames.count();i++){
            str = currentGaitFrames.at(i);
            stream << str << "\n";
            ui->plainTextEditGaitData->appendPlainText(str);
        }
        aFile.close();
    }else if(ui->rbtnAdjustGait->isChecked()){
        QStandardItem *item;
        tempstr = "GaitID";
        //ID不能为空
        QString idstr = ui->lineEditGaitID->text();
        if(idstr.isEmpty()) idstr = "0";
        str = tempstr+"\n"+ idstr;
        stream << str << "\n";
        ui->plainTextEditGaitData->appendPlainText(str);
        tempstr = "GaitRate";
        //帧率不能为空
        QString ratestr = ui->lineEditGaitRate->text();
        if(ratestr.isEmpty()) ratestr = "30";
        currentGaitRate = ratestr.toInt();
        str = tempstr+"\n"+ratestr;
        stream << str << "\n";
        ui->plainTextEditGaitData->appendPlainText(str);
        tempstr = "GaitDescription";
        str = tempstr+"\n"+ui->textEditGaitDesc->toPlainText().replace("\n","");
        stream << str << "\n";
        ui->plainTextEditGaitData->appendPlainText(str);
        tempstr = "zero_point";
        QString temp_zero_point = currentZeroPoint;
        str = tempstr+"\n"+temp_zero_point;
        stream << str << "\n";
        ui->plainTextEditGaitData->appendPlainText(str);
        str = "Gait_Frame";
        stream << str << "\n";
        ui->plainTextEditGaitData->appendPlainText(str);
        currentGaitFrames.clear();
        for(int i = 0;i<theModel->rowCount();i++){
            str = "";
            int j = 0;
            for(;j<theModel->columnCount()-1;j++){
                item = theModel->item(i,j);
                str = str + item->text()+",";
            }
            item = theModel->item(i,j);
            str = str + item->text();
            currentGaitFrames.append(str);
            stream << str << "\n";
            ui->plainTextEditGaitData->appendPlainText(str);
        }

        aFile.close();
    }

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


void MainWindow::on_sshConnect_triggered()
{
    if (sshConWin == NULL)
        sshConWin = new sshConnectWindow(this);
    sshConWin->setWindowTitle("ssh连接");
    // 功能细节
    connect(sshConWin,SIGNAL(sigConnectStateChanged(bool,QString,int)),
            sshConWin,SLOT(slotConnectStateChanged(bool,QString,int)));
    connect(sshConWin,SIGNAL(sigDataArrived(QString,QString,int)),
            sshConWin,SLOT(slotDataArrived(QString,QString,int)));
    connect(sshConWin,SIGNAL(sigSend(QString)),sshConWin,SLOT(slotSend(QString)));
    connect(sshConWin,SIGNAL(sigDisconnected()),sshConWin,SLOT(slotDisconnected()));
    sshConWin->show();
}


void MainWindow::on_pushButtonClearText_clicked()
{
    ui->plainTextEditGaitData->clear();
}



void MainWindow::on_tableView_clicked(const QModelIndex &index)
{
    if(!index.isValid()){
        ui->btnDelCurFrame->setEnabled(false);
        ui->btnAppendFrame->setEnabled(false);
        ui->btnInsertFrame->setEnabled(false);
        ui->btnRecordCurFrame->setEnabled(false);
        ui->btnExecLaterFrame->setEnabled(false);
        ui->btnExecPreFrame->setEnabled(false);
        ui->btnResetFrame->setEnabled(false);
        ui->scrollArea->setEnabled(false);
        ui->btnReturnCentralValue->setEnabled(false);
        return;
    }
    if(currentFileName != "" && ui->rbtnAdjustGait->isChecked()){  // 加载文件后才使能相应按钮
        ui->btnDelCurFrame->setEnabled(true);
        ui->btnAppendFrame->setEnabled(true);
        ui->btnInsertFrame->setEnabled(true);
        ui->btnRecordCurFrame->setEnabled(true);
        ui->btnExecLaterFrame->setEnabled(true);
        ui->btnExecPreFrame->setEnabled(true);
        ui->btnResetFrame->setEnabled(true);
        ui->scrollArea->setEnabled(true);
        ui->btnReturnCentralValue->setEnabled(true);

    }else if(currentFileName != "" && ui->rbtnAdjustZero->isChecked()){
        ui->btnRecordCurFrame->setEnabled(true);
        ui->scrollArea->setEnabled(true);
        ui->btnReturnCentralValue->setEnabled(true);
    }
    if(currentFileName != ""){
        // 把滑块数值和表格数值相对应
        const QSignalBlocker blocker(ui->horizontalSlider);
        const QSignalBlocker blocker2(ui->horizontalSlider_2);
        const QSignalBlocker blocker3(ui->horizontalSlider_3);
        const QSignalBlocker blocker4(ui->horizontalSlider_4);
        const QSignalBlocker blocker5(ui->horizontalSlider_5);
        const QSignalBlocker blocker6(ui->horizontalSlider_6);
        const QSignalBlocker blocker7(ui->horizontalSlider_7);
        const QSignalBlocker blocker8(ui->horizontalSlider_8);
        const QSignalBlocker blocker9(ui->horizontalSlider_9);
        const QSignalBlocker blocker10(ui->horizontalSlider_10);
        const QSignalBlocker blocker11(ui->horizontalSlider_11);
        const QSignalBlocker blocker12(ui->horizontalSlider_12);
        const QSignalBlocker blocker13(ui->horizontalSlider_13);
        const QSignalBlocker blocker14(ui->horizontalSlider_14);
        const QSignalBlocker blocker15(ui->horizontalSlider_15);
        const QSignalBlocker blocker16(ui->horizontalSlider_16);
        const QSignalBlocker blocker17(ui->horizontalSlider_17);
        const QSignalBlocker blocker18(ui->horizontalSlider_18);
        const QSignalBlocker blocker19(ui->horizontalSlider_19);
        const QSignalBlocker blocker20(ui->horizontalSlider_20);
        const QSignalBlocker blocker21(ui->horizontalSlider_21);
        const QSignalBlocker blocker22(ui->horizontalSlider_22);
        const QSignalBlocker blocker23(ui->horizontalSlider_23);
        const QSignalBlocker blocker24(ui->horizontalSlider_24);
        const QSignalBlocker blocker25(ui->horizontalSlider_25);

        ExecCurrentFrame();
        ui->horizontalSlider->setValue(theModel->item(index.row(),0)->text().toInt());
        ui->horizontalSlider_2->setValue(theModel->item(index.row(),1)->text().toInt());
        ui->horizontalSlider_3->setValue(theModel->item(index.row(),2)->text().toInt());
        ui->horizontalSlider_4->setValue(theModel->item(index.row(),3)->text().toInt());
        ui->horizontalSlider_5->setValue(theModel->item(index.row(),4)->text().toInt());
        ui->horizontalSlider_6->setValue(theModel->item(index.row(),5)->text().toInt());
        ui->horizontalSlider_7->setValue(theModel->item(index.row(),6)->text().toInt());
        ui->horizontalSlider_8->setValue(theModel->item(index.row(),7)->text().toInt());
        ui->horizontalSlider_9->setValue(theModel->item(index.row(),8)->text().toInt());
        ui->horizontalSlider_10->setValue(theModel->item(index.row(),9)->text().toInt());
        ui->horizontalSlider_11->setValue(theModel->item(index.row(),10)->text().toInt());
        ui->horizontalSlider_12->setValue(theModel->item(index.row(),11)->text().toInt());
        ui->horizontalSlider_13->setValue(theModel->item(index.row(),12)->text().toInt());
        ui->horizontalSlider_14->setValue(theModel->item(index.row(),13)->text().toInt());
        ui->horizontalSlider_15->setValue(theModel->item(index.row(),14)->text().toInt());
        ui->horizontalSlider_16->setValue(theModel->item(index.row(),15)->text().toInt());
        ui->horizontalSlider_17->setValue(theModel->item(index.row(),16)->text().toInt());
        ui->horizontalSlider_18->setValue(theModel->item(index.row(),17)->text().toInt());
        ui->horizontalSlider_19->setValue(theModel->item(index.row(),18)->text().toInt());
        ui->horizontalSlider_20->setValue(theModel->item(index.row(),19)->text().toInt());
        ui->horizontalSlider_21->setValue(theModel->item(index.row(),20)->text().toInt());
        ui->horizontalSlider_22->setValue(theModel->item(index.row(),21)->text().toInt());
        ui->horizontalSlider_23->setValue(theModel->item(index.row(),22)->text().toInt());
        ui->horizontalSlider_24->setValue(theModel->item(index.row(),23)->text().toInt());
        ui->horizontalSlider_25->setValue(theModel->item(index.row(),24)->text().toInt());
        ui->horizontalSlider_26->setValue(theModel->item(index.row(),25)->text().toInt());
        // spinbox数值和表格数值相对应
        ui->spinBox->setValue(theModel->item(index.row(),0)->text().toInt());
        ui->spinBox_2->setValue(theModel->item(index.row(),1)->text().toInt());
        ui->spinBox_3->setValue(theModel->item(index.row(),2)->text().toInt());
        ui->spinBox_4->setValue(theModel->item(index.row(),3)->text().toInt());
        ui->spinBox_5->setValue(theModel->item(index.row(),4)->text().toInt());
        ui->spinBox_6->setValue(theModel->item(index.row(),5)->text().toInt());
        ui->spinBox_7->setValue(theModel->item(index.row(),6)->text().toInt());
        ui->spinBox_8->setValue(theModel->item(index.row(),7)->text().toInt());
        ui->spinBox_9->setValue(theModel->item(index.row(),8)->text().toInt());
        ui->spinBox_10->setValue(theModel->item(index.row(),9)->text().toInt());
        ui->spinBox_11->setValue(theModel->item(index.row(),10)->text().toInt());
        ui->spinBox_12->setValue(theModel->item(index.row(),11)->text().toInt());
        ui->spinBox_13->setValue(theModel->item(index.row(),12)->text().toInt());
        ui->spinBox_14->setValue(theModel->item(index.row(),13)->text().toInt());
        ui->spinBox_15->setValue(theModel->item(index.row(),14)->text().toInt());
        ui->spinBox_16->setValue(theModel->item(index.row(),15)->text().toInt());
        ui->spinBox_17->setValue(theModel->item(index.row(),16)->text().toInt());
        ui->spinBox_18->setValue(theModel->item(index.row(),17)->text().toInt());
        ui->spinBox_19->setValue(theModel->item(index.row(),18)->text().toInt());
        ui->spinBox_20->setValue(theModel->item(index.row(),19)->text().toInt());
        ui->spinBox_21->setValue(theModel->item(index.row(),20)->text().toInt());
        ui->spinBox_22->setValue(theModel->item(index.row(),21)->text().toInt());
        ui->spinBox_23->setValue(theModel->item(index.row(),22)->text().toInt());
        ui->spinBox_24->setValue(theModel->item(index.row(),23)->text().toInt());
        ui->spinBox_25->setValue(theModel->item(index.row(),24)->text().toInt());
        ui->spinBox_26->setValue(theModel->item(index.row(),25)->text().toInt());
    }
}

void MainWindow::iniModelFromStringList(QStringList &fileContent)
{
    theModel->setRowCount(fileContent.count()-9);
    QString temp;
    temp = fileContent.at(0);
    if(temp != "GaitID"){
        QMessageBox::warning(this,"警告","文件格式不正确！");
        return;
    }
    QString gaitID = fileContent.at(1);
    ui->lineEditGaitID->setText(gaitID);
    temp = fileContent.at(2);
    if(temp != "GaitRate"){
        QMessageBox::warning(this,"警告","文件格式不正确！");
        return;
    }
    QString gaitRate = fileContent.at(3);
    ui->lineEditGaitRate->setText(gaitRate);
    currentGaitRate = gaitRate.toInt();
    temp = fileContent.at(4);
    if(temp != "GaitDescription"){
        QMessageBox::warning(this,"警告","文件格式不正确！");
        return;
    }
    QString gaitDesc = fileContent.at(5);
    ui->textEditGaitDesc->setText(gaitDesc);
    temp = fileContent.at(6);
    if(temp != "zero_point"){
        QMessageBox::warning(this,"警告","文件格式不正确！");
        return;
    }
    QString zero_point = fileContent.at(7);
    currentZeroPoint = zero_point;
//    QStringList zero_point_list = zero_point.split(",",QString::SkipEmptyParts);
    temp = fileContent.at(8);
    if(temp != "Gait_Frame"){
        QMessageBox::warning(this,"警告","文件格式不正确！");
        return;
    }
    currentGaitFrames.clear();
    for(int i = 9;i < fileContent.count();i++){
        QString gait_frame = fileContent.at(i);
        currentGaitFrames.append(gait_frame);
        QStringList gait_frame_list = gait_frame.split(",",QString::SkipEmptyParts);
        for(int j = 0; j < FixedColumnCount;j++){
            QStandardItem *item = new QStandardItem(gait_frame_list.at(j));
            theModel->setItem(i-9,j,item);
        }
        // 单独处理步态帧列
        QStandardItem *frameItem = new QStandardItem(gait_frame_list.at(FrameRateColumn));
        theModel->setItem(i-9,FrameRateColumn,frameItem);
    }

}

void MainWindow::iniModelFromStringList_zeroPoint(QStringList &fileContent)
{
    theModel->setRowCount(1);
    QString temp;
    temp = fileContent.at(0);
    if(temp != "GaitID"){
        QMessageBox::warning(this,"警告","文件格式不正确！");
        return;
    }
    QString gaitID = fileContent.at(1);
    ui->lineEditGaitID->setText(gaitID);
    temp = fileContent.at(2);
    if(temp != "GaitRate"){
        QMessageBox::warning(this,"警告","文件格式不正确！");
        return;
    }
    QString gaitRate = fileContent.at(3);
    ui->lineEditGaitRate->setText(gaitRate);
    currentGaitRate = gaitRate.toInt();
    temp = fileContent.at(4);
    if(temp != "GaitDescription"){
        QMessageBox::warning(this,"警告","文件格式不正确！");
        return;
    }
    QString gaitDesc = fileContent.at(5);
    ui->textEditGaitDesc->setText(gaitDesc);
    temp = fileContent.at(6);
    if(temp != "zero_point"){
        QMessageBox::warning(this,"警告","文件格式不正确！");
        return;
    }
    QString zero_point = fileContent.at(7);
    currentZeroPoint = zero_point;
    QStringList zero_point_list = zero_point.split(",",QString::SkipEmptyParts);

    for(int i = 0; i < zero_point_list.count(); i++){
        QStandardItem *item = new QStandardItem(zero_point_list.at(i));
        theModel->setItem(0,i,item);
    }
    QStandardItem *item = new QStandardItem("1"); // 单独添加帧数列，防止报错
    theModel->setItem(0,FrameRateColumn,item);
}


void MainWindow::on_anotherSaveFile_triggered()
{
    QString curPath=QDir::currentPath();//获取系统当前目录
    QString dlgTitle="另存为一个步态文件"; //对话框标题
    QString filter="步态文件(*.txt)"; //文件过滤器
    QString aFileName=QFileDialog::getSaveFileName(this,dlgTitle,curPath,filter);

    if (aFileName.isEmpty())
        return;
    QFile   aFile(aFileName);

    if (!aFile.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    if(ui->rbtnAdjustZero->isChecked()){
        QTextStream stream(&aFile);
        stream.setCodec("utf-8");
        QString str;
        ui->plainTextEditGaitData->clear();
        QString tempstr;
        tempstr = "GaitID";
        //ID不能为空
        QString idstr = ui->lineEditGaitID->text();
        if(idstr.isEmpty()) idstr = "0";
        str = tempstr+"\n"+ idstr;
        stream << str << "\n";
        ui->plainTextEditGaitData->appendPlainText(str);
        tempstr = "GaitRate";
        //帧率不能为空
        QString ratestr = ui->lineEditGaitRate->text();
        if(ratestr.isEmpty()) ratestr = "30";
        str = tempstr+"\n"+ratestr;
        stream << str << "\n";
        ui->plainTextEditGaitData->appendPlainText(str);
        tempstr = "GaitDescription";
        str = tempstr+"\n"+ui->textEditGaitDesc->toPlainText().replace("\n","");
        stream << str << "\n";
        ui->plainTextEditGaitData->appendPlainText(str);
        tempstr = "zero_point";
        QString temp_zero_point = "";
        //读取表中新的零点数据
        for(int i = 0; i < FixedColumnCount-1; i++){
            temp_zero_point += theModel->item(0,i)->text() + ",";
        }
        temp_zero_point += theModel->item(0,FixedColumnCount-1)->text();
        currentZeroPoint = temp_zero_point;
        str = tempstr+"\n"+temp_zero_point;
        stream << str << "\n";
        ui->plainTextEditGaitData->appendPlainText(str);
        str = "Gait_Frame";
        stream << str << "\n";
        ui->plainTextEditGaitData->appendPlainText(str);
        for(int i = 0;i<currentGaitFrames.count();i++){
            str = currentGaitFrames.at(i);
            stream << str << "\n";
            ui->plainTextEditGaitData->appendPlainText(str);
        }
        aFile.close();
    }else if(ui->rbtnAdjustGait->isChecked()){
        QTextStream stream(&aFile);
        stream.setCodec("utf-8");
        QString str;
        QStandardItem *item;
        ui->plainTextEditGaitData->clear();
        QString tempstr;
        tempstr = "GaitID";
        //ID不能为空
        QString idstr = ui->lineEditGaitID->text();
        if(idstr.isEmpty()) idstr = "0";
        str = tempstr+"\n"+ idstr;
        stream << str << "\n";
        ui->plainTextEditGaitData->appendPlainText(str);
        tempstr = "GaitRate";
        //帧率不能为空
        QString ratestr = ui->lineEditGaitRate->text();
        if(ratestr.isEmpty()) ratestr = "30";
        str = tempstr+"\n"+ratestr;
        stream << str << "\n";
        ui->plainTextEditGaitData->appendPlainText(str);
        tempstr = "GaitDescription";
        str = tempstr+"\n"+ui->textEditGaitDesc->toPlainText().replace("\n","");
        stream << str << "\n";
        ui->plainTextEditGaitData->appendPlainText(str);
        tempstr = "zero_point";
        QString temp_zero_point = currentZeroPoint;
        str = tempstr+"\n"+temp_zero_point;
        stream << str << "\n";
        ui->plainTextEditGaitData->appendPlainText(str);
        str = "Gait_Frame";
        stream << str << "\n";
        ui->plainTextEditGaitData->appendPlainText(str);
        currentGaitFrames.clear();
        for(int i = 0;i<theModel->rowCount();i++){
            str = "";
            int j = 0;
            for(;j<theModel->columnCount()-1;j++){
                item = theModel->item(i,j);
                str = str + item->text()+",";
            }
            item = theModel->item(i,j);
            str = str + item->text();
            currentGaitFrames.append(str);
            stream << str << "\n";
            ui->plainTextEditGaitData->appendPlainText(str);
        }

        aFile.close();
    }
}


void MainWindow::on_btnAppendFrame_clicked()
{
    QList<QStandardItem*> itemList;
    QStandardItem *item;
    QStandardItem *newItem;
    for (int i = 0; i < theModel->columnCount(); ++i) { // 加1是因为还有帧率列
        item = theModel->item(theModel->rowCount()-1,i);
        newItem = new QStandardItem(item->text());
        itemList<<newItem;
    }
    theModel->appendRow(itemList);
    theSelection->clearSelection();
    QModelIndex index = theModel->index(theModel->rowCount()-1,0);
    theSelection->setCurrentIndex(index,QItemSelectionModel::Select);
}


void MainWindow::on_btnInsertFrame_clicked()
{
    QList<QStandardItem*> itemList;
    QStandardItem *item;
    QStandardItem *newItem;
    for (int i = 0; i < theModel->columnCount(); ++i) {
        item = theModel->item(theSelection->currentIndex().row(),i);
        newItem = new QStandardItem(item->text());
        itemList<<newItem;
    }
    int curRow = theSelection->currentIndex().row();
    theModel->insertRow(curRow+1,itemList);
    QModelIndex index = theModel->index(curRow+1,0);
    theSelection->clearSelection();
    theSelection->setCurrentIndex(index,QItemSelectionModel::Select);
}


void MainWindow::on_btnDelCurFrame_clicked()
{
    QModelIndex index = theSelection->currentIndex();
    if(index.row() != theModel->rowCount()-1){
        theModel->removeRow(index.row());
        theSelection->setCurrentIndex(index,QItemSelectionModel::Select);
    }else {
        theModel->removeRow(index.row());
    }

}

void MainWindow::send_horizontalSlider_Data()
{
    if(ui->checkBoxNetwork->isChecked()){
        if(ui->rbtnAdjustGait->isChecked()){
            QStringList currentZeroPointList = currentZeroPoint.split(",",QString::SkipEmptyParts);
            QList<int> zeroData;
            for (int i = 0; i < currentZeroPointList.count(); ++i) {
                QString str = currentZeroPointList.at(i);
                zeroData.append(str.toInt());
            }
            QString ready_send_msg = "special_gait_data";
            ready_send_msg.append("\n");
            ready_send_msg += QString::asprintf("%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f",
                                                (ui->horizontalSlider->value()+zeroData.at(0))/180.0*PI,
                                                (ui->horizontalSlider_2->value()+zeroData.at(1))/180.0*PI,
                                                (ui->horizontalSlider_3->value()+zeroData.at(2))/180.0*PI,
                                                (ui->horizontalSlider_4->value()+zeroData.at(3))/180.0*PI,
                                                (ui->horizontalSlider_5->value()+zeroData.at(4))/180.0*PI,
                                                (ui->horizontalSlider_6->value()+zeroData.at(5))/180.0*PI,
                                                (ui->horizontalSlider_7->value()+zeroData.at(6))/180.0*PI,
                                                (ui->horizontalSlider_8->value()+zeroData.at(7))/180.0*PI,
                                                (ui->horizontalSlider_9->value()+zeroData.at(8))/180.0*PI,
                                                (ui->horizontalSlider_10->value()+zeroData.at(9))/180.0*PI,
                                                (ui->horizontalSlider_11->value()+zeroData.at(10))/180.0*PI,
                                                (ui->horizontalSlider_12->value()+zeroData.at(11))/180.0*PI,
                                                (ui->horizontalSlider_13->value()+zeroData.at(12))/180.0*PI,
                                                (ui->horizontalSlider_14->value()+zeroData.at(13))/180.0*PI,
                                                (ui->horizontalSlider_15->value()+zeroData.at(14))/180.0*PI,
                                                (ui->horizontalSlider_16->value()+zeroData.at(15))/180.0*PI,
                                                (ui->horizontalSlider_17->value()+zeroData.at(16))/180.0*PI,
                                                (ui->horizontalSlider_18->value()+zeroData.at(17))/180.0*PI,
                                                (ui->horizontalSlider_19->value()+zeroData.at(18))/180.0*PI,
                                                (ui->horizontalSlider_20->value()+zeroData.at(19))/180.0*PI,
                                                (ui->horizontalSlider_21->value()+zeroData.at(20))/180.0*PI,
                                                (ui->horizontalSlider_22->value()+zeroData.at(21))/180.0*PI,
                                                (ui->horizontalSlider_23->value()+zeroData.at(22))/180.0*PI,
                                                (ui->horizontalSlider_24->value()+zeroData.at(23))/180.0*PI,
                                                (ui->horizontalSlider_25->value()+zeroData.at(24))/180.0*PI
                                                );
            ui->plainTextEditGaitData->appendPlainText("[out] "+ready_send_msg);
            QByteArray  send_msg=ready_send_msg.toUtf8();
            tcpClient->write(send_msg);
            tcpClient->waitForBytesWritten(5);
        }else if(ui->rbtnAdjustZero->isChecked()){
            QString ready_send_msg = "special_gait_data";
            ready_send_msg.append("\n");
            ready_send_msg += QString::asprintf("%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f",
                                                (ui->horizontalSlider->value())/180.0*PI,
                                                (ui->horizontalSlider_2->value())/180.0*PI,
                                                (ui->horizontalSlider_3->value())/180.0*PI,
                                                (ui->horizontalSlider_4->value())/180.0*PI,
                                                (ui->horizontalSlider_5->value())/180.0*PI,
                                                (ui->horizontalSlider_6->value())/180.0*PI,
                                                (ui->horizontalSlider_7->value())/180.0*PI,
                                                (ui->horizontalSlider_8->value())/180.0*PI,
                                                (ui->horizontalSlider_9->value())/180.0*PI,
                                                (ui->horizontalSlider_10->value())/180.0*PI,
                                                (ui->horizontalSlider_11->value())/180.0*PI,
                                                (ui->horizontalSlider_12->value())/180.0*PI,
                                                (ui->horizontalSlider_13->value())/180.0*PI,
                                                (ui->horizontalSlider_14->value())/180.0*PI,
                                                (ui->horizontalSlider_15->value())/180.0*PI,
                                                (ui->horizontalSlider_16->value())/180.0*PI,
                                                (ui->horizontalSlider_17->value())/180.0*PI,
                                                (ui->horizontalSlider_18->value())/180.0*PI,
                                                (ui->horizontalSlider_19->value())/180.0*PI,
                                                (ui->horizontalSlider_20->value())/180.0*PI,
                                                (ui->horizontalSlider_21->value())/180.0*PI,
                                                (ui->horizontalSlider_22->value())/180.0*PI,
                                                (ui->horizontalSlider_23->value())/180.0*PI,
                                                (ui->horizontalSlider_24->value())/180.0*PI,
                                                (ui->horizontalSlider_25->value())/180.0*PI
                                                );
            ui->plainTextEditGaitData->appendPlainText("[out] "+ready_send_msg);
            QByteArray  send_msg=ready_send_msg.toUtf8();
            tcpClient->write(send_msg);
            tcpClient->waitForBytesWritten(5);
        }
    }
}


void MainWindow::on_horizontalSlider_valueChanged(int value)
{
    QModelIndex index =  theSelection->currentIndex();
    ui->spinBox->setValue(value);
    send_horizontalSlider_Data();
}


void MainWindow::on_horizontalSlider_2_valueChanged(int value)
{
    QModelIndex index =  theSelection->currentIndex();
    ui->spinBox_2->setValue(value);
    send_horizontalSlider_Data();
}


void MainWindow::on_horizontalSlider_3_valueChanged(int value)
{
    QModelIndex index =  theSelection->currentIndex();
    ui->spinBox_3->setValue(value);
    send_horizontalSlider_Data();
}


void MainWindow::on_horizontalSlider_4_valueChanged(int value)
{
    QModelIndex index =  theSelection->currentIndex();
    ui->spinBox_4->setValue(value);
    send_horizontalSlider_Data();
}


void MainWindow::on_horizontalSlider_5_valueChanged(int value)
{
    QModelIndex index =  theSelection->currentIndex();
    ui->spinBox_5->setValue(value);
    send_horizontalSlider_Data();
}


void MainWindow::on_horizontalSlider_6_valueChanged(int value)
{
    QModelIndex index =  theSelection->currentIndex();
    ui->spinBox_6->setValue(value);
    send_horizontalSlider_Data();
}


void MainWindow::on_horizontalSlider_7_valueChanged(int value)
{
    QModelIndex index =  theSelection->currentIndex();
    ui->spinBox_7->setValue(value);
    send_horizontalSlider_Data();
}


void MainWindow::on_horizontalSlider_8_valueChanged(int value)
{
    QModelIndex index =  theSelection->currentIndex();
    ui->spinBox_8->setValue(value);
    send_horizontalSlider_Data();
}


void MainWindow::on_horizontalSlider_9_valueChanged(int value)
{
    QModelIndex index =  theSelection->currentIndex();
    ui->spinBox_9->setValue(value);
    send_horizontalSlider_Data();
}


void MainWindow::on_horizontalSlider_10_valueChanged(int value)
{
    QModelIndex index =  theSelection->currentIndex();
    ui->spinBox_10->setValue(value);
    send_horizontalSlider_Data();
}


void MainWindow::on_horizontalSlider_11_valueChanged(int value)
{
    QModelIndex index =  theSelection->currentIndex();
    ui->spinBox_11->setValue(value);
    send_horizontalSlider_Data();
}


void MainWindow::on_horizontalSlider_12_valueChanged(int value)
{
    QModelIndex index =  theSelection->currentIndex();
    ui->spinBox_12->setValue(value);
    send_horizontalSlider_Data();
}


void MainWindow::on_horizontalSlider_13_valueChanged(int value)
{
    QModelIndex index =  theSelection->currentIndex();
    ui->spinBox_13->setValue(value);
    send_horizontalSlider_Data();
}


void MainWindow::on_horizontalSlider_14_valueChanged(int value)
{
    QModelIndex index =  theSelection->currentIndex();
    ui->spinBox_14->setValue(value);
    send_horizontalSlider_Data();
}


void MainWindow::on_horizontalSlider_15_valueChanged(int value)
{
    QModelIndex index =  theSelection->currentIndex();
    ui->spinBox_15->setValue(value);
    send_horizontalSlider_Data();
}


void MainWindow::on_horizontalSlider_16_valueChanged(int value)
{
    QModelIndex index =  theSelection->currentIndex();
    ui->spinBox_16->setValue(value);
    send_horizontalSlider_Data();
}


void MainWindow::on_horizontalSlider_17_valueChanged(int value)
{
    QModelIndex index =  theSelection->currentIndex();
    ui->spinBox_17->setValue(value);
    send_horizontalSlider_Data();
}


void MainWindow::on_horizontalSlider_18_valueChanged(int value)
{
    QModelIndex index =  theSelection->currentIndex();
    ui->spinBox_18->setValue(value);
    send_horizontalSlider_Data();
}


void MainWindow::on_horizontalSlider_19_valueChanged(int value)
{
    QModelIndex index =  theSelection->currentIndex();
    ui->spinBox_19->setValue(value);
    send_horizontalSlider_Data();
}


void MainWindow::on_horizontalSlider_20_valueChanged(int value)
{
    QModelIndex index =  theSelection->currentIndex();
    ui->spinBox_20->setValue(value);
    send_horizontalSlider_Data();
}


void MainWindow::on_horizontalSlider_21_valueChanged(int value)
{
    QModelIndex index =  theSelection->currentIndex();
    ui->spinBox_21->setValue(value);
    send_horizontalSlider_Data();
}


void MainWindow::on_horizontalSlider_22_valueChanged(int value)
{
    QModelIndex index =  theSelection->currentIndex();
    ui->spinBox_22->setValue(value);
    send_horizontalSlider_Data();
}


void MainWindow::on_horizontalSlider_23_valueChanged(int value)
{
    QModelIndex index =  theSelection->currentIndex();
    ui->spinBox_23->setValue(value);
    send_horizontalSlider_Data();
}


void MainWindow::on_horizontalSlider_24_valueChanged(int value)
{
    QModelIndex index =  theSelection->currentIndex();
    ui->spinBox_24->setValue(value);
    send_horizontalSlider_Data();
}


void MainWindow::on_horizontalSlider_25_valueChanged(int value)
{
    QModelIndex index =  theSelection->currentIndex();
    ui->spinBox_25->setValue(value);
    send_horizontalSlider_Data();
}

void MainWindow::on_horizontalSlider_26_valueChanged(int value)
{
    QModelIndex index =  theSelection->currentIndex();
    ui->spinBox_26->setValue(value);
}

void MainWindow::on_spinBox_valueChanged(int arg1)
{
    QModelIndex index =  theSelection->currentIndex();
    ui->horizontalSlider->setValue(arg1);
}


void MainWindow::on_spinBox_2_valueChanged(int arg1)
{
    QModelIndex index =  theSelection->currentIndex();
    ui->horizontalSlider_2->setValue(arg1);
}


void MainWindow::on_spinBox_3_valueChanged(int arg1)
{
    QModelIndex index =  theSelection->currentIndex();
    ui->horizontalSlider_3->setValue(arg1);
}


void MainWindow::on_spinBox_4_valueChanged(int arg1)
{
    QModelIndex index =  theSelection->currentIndex();
    ui->horizontalSlider_4->setValue(arg1);
}


void MainWindow::on_spinBox_5_valueChanged(int arg1)
{
    QModelIndex index =  theSelection->currentIndex();
    ui->horizontalSlider_5->setValue(arg1);
}


void MainWindow::on_spinBox_6_valueChanged(int arg1)
{
    QModelIndex index =  theSelection->currentIndex();
    ui->horizontalSlider_6->setValue(arg1);
}


void MainWindow::on_spinBox_7_valueChanged(int arg1)
{
    QModelIndex index =  theSelection->currentIndex();
    ui->horizontalSlider_7->setValue(arg1);
}


void MainWindow::on_spinBox_8_valueChanged(int arg1)
{
    QModelIndex index =  theSelection->currentIndex();
    ui->horizontalSlider_8->setValue(arg1);
}


void MainWindow::on_spinBox_9_valueChanged(int arg1)
{
    QModelIndex index =  theSelection->currentIndex();
    ui->horizontalSlider_9->setValue(arg1);
}


void MainWindow::on_spinBox_10_valueChanged(int arg1)
{
    QModelIndex index =  theSelection->currentIndex();
    ui->horizontalSlider_10->setValue(arg1);
}


void MainWindow::on_spinBox_11_valueChanged(int arg1)
{
    QModelIndex index =  theSelection->currentIndex();
    ui->horizontalSlider_11->setValue(arg1);
}


void MainWindow::on_spinBox_12_valueChanged(int arg1)
{
    QModelIndex index =  theSelection->currentIndex();
    ui->horizontalSlider_12->setValue(arg1);
}


void MainWindow::on_spinBox_13_valueChanged(int arg1)
{
    QModelIndex index =  theSelection->currentIndex();
    ui->horizontalSlider_13->setValue(arg1);
}


void MainWindow::on_spinBox_14_valueChanged(int arg1)
{
    QModelIndex index =  theSelection->currentIndex();
    ui->horizontalSlider_14->setValue(arg1);
}


void MainWindow::on_spinBox_15_valueChanged(int arg1)
{
    QModelIndex index =  theSelection->currentIndex();
    ui->horizontalSlider_15->setValue(arg1);
}


void MainWindow::on_spinBox_16_valueChanged(int arg1)
{
    QModelIndex index =  theSelection->currentIndex();
    ui->horizontalSlider_16->setValue(arg1);
}


void MainWindow::on_spinBox_17_valueChanged(int arg1)
{
    QModelIndex index =  theSelection->currentIndex();
    ui->horizontalSlider_17->setValue(arg1);
}


void MainWindow::on_spinBox_18_valueChanged(int arg1)
{
    QModelIndex index =  theSelection->currentIndex();
    ui->horizontalSlider_18->setValue(arg1);
}


void MainWindow::on_spinBox_19_valueChanged(int arg1)
{
    QModelIndex index =  theSelection->currentIndex();
    ui->horizontalSlider_19->setValue(arg1);
}


void MainWindow::on_spinBox_20_valueChanged(int arg1)
{
    QModelIndex index =  theSelection->currentIndex();
    ui->horizontalSlider_20->setValue(arg1);
}


void MainWindow::on_spinBox_21_valueChanged(int arg1)
{
    QModelIndex index =  theSelection->currentIndex();
    ui->horizontalSlider_21->setValue(arg1);
}


void MainWindow::on_spinBox_22_valueChanged(int arg1)
{
    QModelIndex index =  theSelection->currentIndex();
    ui->horizontalSlider_22->setValue(arg1);
}


void MainWindow::on_spinBox_23_valueChanged(int arg1)
{
    QModelIndex index =  theSelection->currentIndex();
    ui->horizontalSlider_23->setValue(arg1);
}


void MainWindow::on_spinBox_24_valueChanged(int arg1)
{
    QModelIndex index =  theSelection->currentIndex();
    ui->horizontalSlider_24->setValue(arg1);
}


void MainWindow::on_spinBox_25_valueChanged(int arg1)
{
    QModelIndex index =  theSelection->currentIndex();
    ui->horizontalSlider_25->setValue(arg1);
}

void MainWindow::on_spinBox_26_valueChanged(int arg1)
{
    QModelIndex index =  theSelection->currentIndex();
    ui->horizontalSlider_26->setValue(arg1);
}


void MainWindow::on_btnReturnCentralValue_clicked()
{
    int value = QMessageBox::information(this,"提示","确定回答中央值吗？",QMessageBox::Yes,QMessageBox::No);
    if(value == QMessageBox::Yes){
        ui->horizontalSlider->setValue(0);
        ui->horizontalSlider_2->setValue(0);
        ui->horizontalSlider_3->setValue(0);
        ui->horizontalSlider_4->setValue(0);
        ui->horizontalSlider_5->setValue(0);
        ui->horizontalSlider_6->setValue(0);
        ui->horizontalSlider_7->setValue(0);
        ui->horizontalSlider_8->setValue(0);
        ui->horizontalSlider_9->setValue(0);
        ui->horizontalSlider_10->setValue(0);
        ui->horizontalSlider_11->setValue(0);
        ui->horizontalSlider_12->setValue(0);
        ui->horizontalSlider_13->setValue(0);
        ui->horizontalSlider_14->setValue(0);
        ui->horizontalSlider_15->setValue(0);
        ui->horizontalSlider_16->setValue(0);
        ui->horizontalSlider_17->setValue(0);
        ui->horizontalSlider_18->setValue(0);
        ui->horizontalSlider_19->setValue(0);
        ui->horizontalSlider_20->setValue(0);
        ui->horizontalSlider_21->setValue(0);
        ui->horizontalSlider_22->setValue(0);
        ui->horizontalSlider_23->setValue(0);
        ui->horizontalSlider_24->setValue(0);
        ui->horizontalSlider_25->setValue(0);
    }else {
        return;
    }

}


void MainWindow::on_btnRecordCurFrame_clicked()
{
    QModelIndex index =  theSelection->currentIndex();
    theModel->item(index.row(),0)->setData(ui->horizontalSlider->value(),Qt::DisplayRole);
    theModel->item(index.row(),1)->setData(ui->horizontalSlider_2->value(),Qt::DisplayRole);
    theModel->item(index.row(),2)->setData(ui->horizontalSlider_3->value(),Qt::DisplayRole);
    theModel->item(index.row(),3)->setData(ui->horizontalSlider_4->value(),Qt::DisplayRole);
    theModel->item(index.row(),4)->setData(ui->horizontalSlider_5->value(),Qt::DisplayRole);
    theModel->item(index.row(),5)->setData(ui->horizontalSlider_6->value(),Qt::DisplayRole);
    theModel->item(index.row(),6)->setData(ui->horizontalSlider_7->value(),Qt::DisplayRole);
    theModel->item(index.row(),7)->setData(ui->horizontalSlider_8->value(),Qt::DisplayRole);
    theModel->item(index.row(),8)->setData(ui->horizontalSlider_9->value(),Qt::DisplayRole);
    theModel->item(index.row(),9)->setData(ui->horizontalSlider_10->value(),Qt::DisplayRole);
    theModel->item(index.row(),10)->setData(ui->horizontalSlider_11->value(),Qt::DisplayRole);
    theModel->item(index.row(),11)->setData(ui->horizontalSlider_12->value(),Qt::DisplayRole);
    theModel->item(index.row(),12)->setData(ui->horizontalSlider_13->value(),Qt::DisplayRole);
    theModel->item(index.row(),13)->setData(ui->horizontalSlider_14->value(),Qt::DisplayRole);
    theModel->item(index.row(),14)->setData(ui->horizontalSlider_15->value(),Qt::DisplayRole);
    theModel->item(index.row(),15)->setData(ui->horizontalSlider_16->value(),Qt::DisplayRole);
    theModel->item(index.row(),16)->setData(ui->horizontalSlider_17->value(),Qt::DisplayRole);
    theModel->item(index.row(),17)->setData(ui->horizontalSlider_18->value(),Qt::DisplayRole);
    theModel->item(index.row(),18)->setData(ui->horizontalSlider_19->value(),Qt::DisplayRole);
    theModel->item(index.row(),19)->setData(ui->horizontalSlider_20->value(),Qt::DisplayRole);
    theModel->item(index.row(),20)->setData(ui->horizontalSlider_21->value(),Qt::DisplayRole);
    theModel->item(index.row(),21)->setData(ui->horizontalSlider_22->value(),Qt::DisplayRole);
    theModel->item(index.row(),22)->setData(ui->horizontalSlider_23->value(),Qt::DisplayRole);
    theModel->item(index.row(),23)->setData(ui->horizontalSlider_24->value(),Qt::DisplayRole);
    theModel->item(index.row(),24)->setData(ui->horizontalSlider_25->value(),Qt::DisplayRole);
    theModel->item(index.row(),25)->setData(ui->horizontalSlider_26->value(),Qt::DisplayRole);
}

void MainWindow::ExecCurrentFrame()
{
    if(ui->checkBoxNetwork->isChecked()){
        QModelIndex index = theSelection->currentIndex();
        QStringList currentZeroPointList = currentZeroPoint.split(",",QString::SkipEmptyParts);
        QList<int> zeroData;
        for (int i = 0; i < currentZeroPointList.count(); ++i) {
            QString str = currentZeroPointList.at(i);
            zeroData.append(str.toInt());
        }
        QString ready_send_msg = "special_gait_data";
        ready_send_msg.append("\n");
        if(ui->rbtnAdjustZero->isChecked()){
            for(int i = 0; i < FixedColumnCount-1; i++){
                ready_send_msg += QString::asprintf("%f,",(theModel->item(index.row(),i)->text().toInt())/180.0*PI);
            }
            ready_send_msg += QString::asprintf("%f", (theModel->item(index.row(),FixedColumnCount-1)->text().toInt())/180.0*PI);
        }else {
            for(int i = 0; i < FixedColumnCount-1; i++){
                ready_send_msg += QString::asprintf("%f,",(theModel->item(index.row(),i)->text().toInt()+zeroData.at(i))/180.0*PI);
            }
            ready_send_msg += QString::asprintf("%f", (theModel->item(index.row(),FixedColumnCount-1)->text().toInt()+zeroData.at(FixedColumnCount-1))/180.0*PI);
        }
        ui->plainTextEditGaitData->appendPlainText("[out] "+ready_send_msg);
        QByteArray  send_msg=ready_send_msg.toUtf8();
        tcpClient->write(send_msg);
    }
}


void MainWindow::on_btnExecPreFrame_clicked()
{
    if(ui->checkBoxNetwork->isChecked()){
        QModelIndex index = theSelection->currentIndex();
        if(index.row() == 0){
            QMessageBox::warning(this,"警告","当前帧为第一帧！无法执行前一帧");
            return;
        }
        QStringList currentZeroPointList = currentZeroPoint.split(",",QString::SkipEmptyParts);
        QList<int> zeroData;
        for (int i = 0; i < currentZeroPointList.count(); ++i) {
            QString str = currentZeroPointList.at(i);
            zeroData.append(str.toInt());
        }
        QString ready_send_msg = "special_gait_data";
        ready_send_msg.append("\n");
        for(int i = 0; i < FixedColumnCount-1; i++){
            ready_send_msg += QString::asprintf("%f,",(theModel->item(index.row()-1,i)->text().toInt()+zeroData.at(i))/180.0*PI);
        }
        ready_send_msg += QString::asprintf("%f", (theModel->item(index.row()-1,FixedColumnCount-1)->text().toInt()+zeroData.at(FixedColumnCount-1))/180.0*PI);
        ui->plainTextEditGaitData->appendPlainText("[out] "+ready_send_msg);
        QByteArray  send_msg=ready_send_msg.toUtf8();
        tcpClient->write(send_msg);
        theSelection->clearSelection();
        theSelection->setCurrentIndex(theModel->index(index.row()-1,0),QItemSelectionModel::Select);
    }else{
        QMessageBox::information(this,"提示","请先连接网络！");
    }
}


void MainWindow::on_btnExecLaterFrame_clicked()
{
    if(ui->checkBoxNetwork->isChecked()){
        QModelIndex index = theSelection->currentIndex();
        if(index.row() == theModel->rowCount()-1){
            QMessageBox::warning(this,"警告","当前帧为最后一帧！无法执行后一帧");
            return;
        }
        QStringList currentZeroPointList = currentZeroPoint.split(",",QString::SkipEmptyParts);
        QList<int> zeroData;
        for (int i = 0; i < currentZeroPointList.count(); ++i) {
            QString str = currentZeroPointList.at(i);
            zeroData.append(str.toInt());
        }
        QString ready_send_msg = "special_gait_data";
        ready_send_msg.append("\n");
        for(int i = 0; i < FixedColumnCount-1; i++){
            ready_send_msg += QString::asprintf("%f,",(theModel->item(index.row()+1,i)->text().toInt()+zeroData.at(i))/180.0*PI);
        }
        ready_send_msg += QString::asprintf("%f", (theModel->item(index.row()+1,FixedColumnCount-1)->text().toInt()+zeroData.at(FixedColumnCount-1))/180.0*PI);
        ui->plainTextEditGaitData->appendPlainText("[out] "+ready_send_msg);
        QByteArray  send_msg=ready_send_msg.toUtf8();
        tcpClient->write(send_msg);
        theSelection->clearSelection();
        theSelection->setCurrentIndex(theModel->index(index.row()+1,0),QItemSelectionModel::Select);
    }else{
        QMessageBox::information(this,"提示","请先连接网络！");
    }
}


void MainWindow::on_btnResetFrame_clicked()
{
    if(ui->checkBoxNetwork->isChecked()){
        QStringList currentZeroPointList = currentZeroPoint.split(",",QString::SkipEmptyParts);
        QList<int> zeroData;
        for (int i = 0; i < currentZeroPointList.count(); ++i) {
            QString str = currentZeroPointList.at(i);
            zeroData.append(str.toInt());
        }
        QString ready_send_msg = "special_gait_data";
        ready_send_msg.append("\n");
        for(int i = 0; i < FixedColumnCount-1; i++){
            ready_send_msg += QString::asprintf("%f,",(theModel->item(0,i)->text().toInt()+zeroData.at(i))/180.0*PI);
        }
        ready_send_msg += QString::asprintf("%f", (theModel->item(0,FixedColumnCount-1)->text().toInt()+zeroData.at(FixedColumnCount-1))/180.0*PI);
        ui->plainTextEditGaitData->appendPlainText("[out] "+ready_send_msg);
        QByteArray  send_msg=ready_send_msg.toUtf8();
        tcpClient->write(send_msg);
        theSelection->clearSelection();
        theSelection->setCurrentIndex(theModel->index(0,0),QItemSelectionModel::Select);
    }else {
        QMessageBox::information(this,"提示","请先连接网络！");
    }
}


void MainWindow::on_btnExecList_clicked()
{
    if(ui->checkBoxNetwork->isChecked()){
        //关键帧之间均值插值
        ui->btnExecList->setEnabled(false);
        QStringList currentZeroPointList = currentZeroPoint.split(",",QString::SkipEmptyParts);
        QList<int> zeroData;
        for (int i = 0; i < currentZeroPointList.count(); ++i) {
            QString str = currentZeroPointList.at(i);
            zeroData.append(str.toInt());
        }
        for (int i = 1; i < theModel->rowCount(); ++i) {
            theSelection->clearSelection();
            theSelection->setCurrentIndex(theModel->index(i,0),QItemSelectionModel::Select);
            QList<int> preFrameDataList;
            QList<int> laterFrameDataList;
            for(int j = 0; j < FixedColumnCount; j++){
                preFrameDataList.append(theModel->item(i-1,j)->text().toInt());
                laterFrameDataList.append(theModel->item(i,j)->text().toInt());
            }
            int tempRate = theModel->item(i,FrameRateColumn)->text().toInt();  //当前帧的帧率
            for (int k = 1; k <= tempRate; ++k) {
                QString ready_send_msg = "special_gait_data";
                ready_send_msg.append("\n");
                for(int j = 0; j < FixedColumnCount-1; j++){
                    ready_send_msg += QString::asprintf("%f,",((preFrameDataList.at(j)+((laterFrameDataList.at(j)-preFrameDataList.at(j))/(tempRate*1.0)*k))+zeroData.at(j))/180.0*PI);
                }
                ready_send_msg += QString::asprintf("%f", ((preFrameDataList.at(FixedColumnCount-1)+((laterFrameDataList.at(FixedColumnCount-1)-preFrameDataList.at(FixedColumnCount-1))/(tempRate*1.0)*k))+zeroData.at(FixedColumnCount-1))/180.0*PI);
                ui->plainTextEditGaitData->appendPlainText("[out] "+ready_send_msg);
                QByteArray  send_msg=ready_send_msg.toUtf8();
                tcpClient->write(send_msg);
                tcpClient->waitForReadyRead(20);
                qApp->processEvents();
            }
        }
        theSelection->clearSelection();
        theSelection->setCurrentIndex(theModel->index(theModel->rowCount()-1,0),QItemSelectionModel::Select);
        ui->btnExecList->setEnabled(true);
    }else{
        QMessageBox::information(this,"提示","请先连接网络！");
    }
}


void MainWindow::on_newFile_triggered()
{
    QString curPath=QDir::currentPath();//获取系统当前目录
    QString dlgTitle="选择存储文件路径"; //对话框标题
    QString filter="步态文件(*.txt)"; //文件过滤器
    QString aFileName=QFileDialog::getSaveFileName(this,dlgTitle,curPath,filter);

    if (aFileName.isEmpty())
        return;
    QFile   aFile(aFileName);
    if (!aFile.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    QTextStream stream(&aFile);
    stream.setCodec("utf-8");
    QString str;
    QStandardItem *item;
    ui->plainTextEditGaitData->clear();
    QString tempstr;
    tempstr = "GaitID";
    str = tempstr+"\n";
    str.append("0");
    stream << str << "\n";
    ui->plainTextEditGaitData->appendPlainText(str);
    tempstr = "GaitRate";
    str = tempstr+"\n";
    str.append("30");
    stream << str << "\n";
    ui->plainTextEditGaitData->appendPlainText(str);
    tempstr = "GaitDescription";
    str = tempstr+"\n";
    str.append(" ");
    stream << str << "\n";
    ui->plainTextEditGaitData->appendPlainText(str);
    tempstr = "zero_point";
    QString temp_zero_point = "0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0";
    str = tempstr+"\n"+temp_zero_point;
    stream << str << "\n";
    ui->plainTextEditGaitData->appendPlainText(str);
    str = "Gait_Frame";
    stream << str << "\n";
    ui->plainTextEditGaitData->appendPlainText(str);
    str = "";
    for(int i = 0;i<FixedColumnCount-1;i++){
        str = str + "0,";
    }
    str.append("0");
    str.append(",30"); //单独处理步态帧列
    stream << str << "\n";
    ui->plainTextEditGaitData->appendPlainText(str);

    aFile.close();

    // 打开新建文件
    currentFileName = aFileName;
    if (aFileName.isEmpty())
        return;
    aFile.setFileName(aFileName);
    if (!aFile.exists()) //文件不存在
        return;
    if (!aFile.open(QIODevice::ReadOnly | QIODevice::Text))
        return;
    this->setWindowTitle("SpecialGait  "+aFileName);
    QStringList fileContent;
    QTextStream newStream(&aFile);
    newStream.setCodec("utf-8");
    ui->plainTextEditGaitData->clear();
    while(!newStream.atEnd()){
        QString str = newStream.readLine();
        fileContent.append(str);
        ui->plainTextEditGaitData->appendPlainText(str);
    }
    aFile.close();
    if(fileContent.isEmpty()){
        QMessageBox::warning(this,"警告","文件为空！");
        return;
    }
    iniModelFromStringList(fileContent);
    ui->anotherSaveFile->setEnabled(true);
    ui->btnExecList->setEnabled(true);
    ui->rbtnAdjustGait->setEnabled(true);
    ui->rbtnAdjustZero->setEnabled(true);
}


void MainWindow::on_rbtnAdjustZero_clicked()
{
    QString aFileName = currentFileName;
    if (aFileName.isEmpty())
        return;
    QFile   aFile(aFileName);
    aFile.setFileName(aFileName);
    if (!aFile.exists()) //文件不存在
        return;
    if (!aFile.open(QIODevice::ReadOnly | QIODevice::Text))
        return;
    QStringList fileContent;
    QTextStream newStream(&aFile);
    newStream.setCodec("utf-8");
    ui->plainTextEditGaitData->clear();
    while(!newStream.atEnd()){
        QString str = newStream.readLine();
        fileContent.append(str);
        ui->plainTextEditGaitData->appendPlainText(str);
    }
    aFile.close();
    if(fileContent.isEmpty()){
        QMessageBox::warning(this,"警告","文件为空！");
        return;
    }
    iniModelFromStringList_zeroPoint(fileContent);
    ui->anotherSaveFile->setEnabled(true);
    ui->btnExecList->setEnabled(true);
    ui->btnDelCurFrame->setEnabled(false);
    ui->btnAppendFrame->setEnabled(false);
    ui->btnInsertFrame->setEnabled(false);
    ui->btnExecLaterFrame->setEnabled(false);
    ui->btnExecPreFrame->setEnabled(false);
    ui->btnResetFrame->setEnabled(false);
}


void MainWindow::on_rbtnAdjustGait_clicked()
{
    QString aFileName = currentFileName;
    if (aFileName.isEmpty())
        return;
    QFile   aFile(aFileName);
    aFile.setFileName(aFileName);
    if (!aFile.exists()) //文件不存在
        return;
    if (!aFile.open(QIODevice::ReadOnly | QIODevice::Text))
        return;
    QStringList fileContent;
    QTextStream newStream(&aFile);
    newStream.setCodec("utf-8");
    ui->plainTextEditGaitData->clear();
    while(!newStream.atEnd()){
        QString str = newStream.readLine();
        fileContent.append(str);
        ui->plainTextEditGaitData->appendPlainText(str);
    }
    aFile.close();
    if(fileContent.isEmpty()){
        QMessageBox::warning(this,"警告","文件为空！");
        return;
    }
    iniModelFromStringList(fileContent);
    ui->anotherSaveFile->setEnabled(true);
    ui->btnExecList->setEnabled(true);

}



void MainWindow::on_IkidRobotImage_triggered()
{
    if (ikidImage == NULL)
        ikidImage = new IkidRobotImage(this);
    ikidImage->setWindowTitle("IkidRobot");

    ikidImage->show();
}


void MainWindow::on_gaitUpload_triggered()
{
    if (gaitUpload == NULL){
        gaitUpload = new GaitUpload(this);;
        gaitUpload->setWindowTitle("文件上传");
        // 功能细节
        connect(gaitUpload, SIGNAL(startFileUpload()), this, SLOT(onStartFileUpload()));
        connect(this, SIGNAL(gaitDataUploadProcess(int,int)), gaitUpload, SLOT(displayProcessBar(int, int)));
        connect(this, SIGNAL(clearGaitDataUploadProcessBar()), gaitUpload, SLOT(clearProcessBar()));
    }
    gaitUpload->show();
    emit clearGaitDataUploadProcessBar();
}

void MainWindow::onStartFileUpload()
{
    if(ui->checkBoxNetwork->isChecked()){
        on_saveFile_triggered(); // 先保存当前文件
        QString ready_send_msg = "start_write_gait_txt";
        ready_send_msg.append("\n");
        if(ui->lineEditGaitID->text() == "") return;
        if(currentFileName.isEmpty()) return;
        ready_send_msg += ui->lineEditGaitID->text();
        ready_send_msg.append("\n");
        QByteArray  send_msg=ready_send_msg.toUtf8();
        int ret = -1;
        ret = tcpClient->write(send_msg);
        if(ret == -1){
            QMessageBox::warning(this,"失败","文件已上传失败，客户端报错或网络连接已断开!");
            emit clearGaitDataUploadProcessBar();
            return;
        }
        bool readret = false;
        readret = tcpClient->waitForReadyRead();

        qApp->processEvents();
        if(!readret){
            QMessageBox::warning(this,"失败","文件已上传超时!");
            emit clearGaitDataUploadProcessBar();
            return;
        }

        QString aFileName = currentFileName;
        if (aFileName.isEmpty())
            return;
        QFile   aFile(aFileName);
        aFile.setFileName(aFileName);
        if (!aFile.exists()) //文件不存在
            return;
        if (!aFile.open(QIODevice::ReadOnly | QIODevice::Text))
            return;
        QTextStream newStream(&aFile);
        newStream.setCodec("utf-8");
        int count = 0;
        while(!newStream.atEnd()){
            QString str = newStream.readLine();
            count++;
            ready_send_msg = "gait_txt_data";
            ready_send_msg.append("\n");
            ready_send_msg.append(str);
            ready_send_msg.append("\n");
            send_msg = ready_send_msg.toUtf8();
            ret = tcpClient->write(send_msg);
            if(ret == -1){
                QMessageBox::warning(this,"失败","文件已上传失败，客户端报错或网络连接已断开!请重新启动客户端节点!");
                emit clearGaitDataUploadProcessBar();
                return;
            }
            readret = tcpClient->waitForReadyRead();
            qApp->processEvents();
            if(!readret){
                QMessageBox::warning(this,"失败","文件已上传超时!");
                emit clearGaitDataUploadProcessBar();
                return;
            }
            emit gaitDataUploadProcess(count, theModel->rowCount()+9);
        }
        aFile.close();
        QMessageBox::information(this,"成功","文件已上传完毕");


    }else {
        QMessageBox::information(this,"提示","请先连接网络！");
    }
}



void MainWindow::on_walkDebug_triggered()
{
    if (walkDebug == NULL){
        walkDebug = new WalkDebug(this);
        walkDebug->setWindowTitle("walk debug");
        // 功能细节
        connect(walkDebug, SIGNAL(cmd_walk_start_walk()), this, SLOT(onCmd_walk_start_walk()));
        connect(walkDebug, SIGNAL(cmd_walk_forward()), this, SLOT(onCmd_walk_forward()));
        connect(walkDebug, SIGNAL(cmd_walk_left()), this, SLOT(onCmd_walk_left()));
        connect(walkDebug, SIGNAL(cmd_walk_right()), this, SLOT(onCmd_walk_right()));
        connect(walkDebug, SIGNAL(cmd_walk_stop()), this, SLOT(onCmd_walk_stop()));
        connect(walkDebug, SIGNAL(cmd_walk_end_walk()), this, SLOT(onCmd_walk_end_walk()));
    }
    walkDebug->setWindowModality(Qt::ApplicationModal);
    walkDebug->show();

}

void MainWindow::onCmd_walk_start_walk()
{
    if(ui->checkBoxNetwork->isChecked()){
        QString ready_send_msg = "cmd_walk_start_walk";
        ready_send_msg.append("\n");
        QByteArray  send_msg=ready_send_msg.toUtf8();
        int ret = -1;
        ret = tcpClient->write(send_msg);
        if(ret == -1){
            QMessageBox::warning(this,"失败","start_walk 启动失败，客户端报错或网络连接已断开!");
            return;
        }
    }else {
        QMessageBox::information(this,"提示","请先连接网络！");
    }
}

void MainWindow::onCmd_walk_forward()
{
    if(ui->checkBoxNetwork->isChecked()){
        QString ready_send_msg = "cmd_walk_forward";
        ready_send_msg.append("\n");
        QByteArray  send_msg=ready_send_msg.toUtf8();
        int ret = -1;
        ret = tcpClient->write(send_msg);
        if(ret == -1){
            QMessageBox::warning(this,"失败","walk_forward 启动失败，客户端报错或网络连接已断开!");
            return;
        }
    }else {
        QMessageBox::information(this,"提示","请先连接网络！");
    }
}

void MainWindow::onCmd_walk_left()
{
    if(ui->checkBoxNetwork->isChecked()){
        QString ready_send_msg = "cmd_walk_left";
        ready_send_msg.append("\n");
        QByteArray  send_msg=ready_send_msg.toUtf8();
        int ret = -1;
        ret = tcpClient->write(send_msg);
        if(ret == -1){
            QMessageBox::warning(this,"失败","walk_left 启动失败，客户端报错或网络连接已断开!");
            return;
        }
    }else {
        QMessageBox::information(this,"提示","请先连接网络！");
    }
}

void MainWindow::onCmd_walk_right()
{
    if(ui->checkBoxNetwork->isChecked()){
        QString ready_send_msg = "cmd_walk_right";
        ready_send_msg.append("\n");
        QByteArray  send_msg=ready_send_msg.toUtf8();
        int ret = -1;
        ret = tcpClient->write(send_msg);
        if(ret == -1){
            QMessageBox::warning(this,"失败","walk_right 启动失败，客户端报错或网络连接已断开!");
            return;
        }
    }else {
        QMessageBox::information(this,"提示","请先连接网络！");
    }
}

void MainWindow::onCmd_walk_stop()
{
    if(ui->checkBoxNetwork->isChecked()){
        QString ready_send_msg = "cmd_walk_stop";
        ready_send_msg.append("\n");
        QByteArray  send_msg=ready_send_msg.toUtf8();
        int ret = -1;
        ret = tcpClient->write(send_msg);
        if(ret == -1){
            QMessageBox::warning(this,"失败","walk_stop 启动失败，客户端报错或网络连接已断开!");
            return;
        }
    }else {
        QMessageBox::information(this,"提示","请先连接网络！");
    }
}

void MainWindow::onCmd_walk_end_walk()
{
    if(ui->checkBoxNetwork->isChecked()){
        return;
    }else {
        QMessageBox::information(this,"提示","请先连接网络！");
    }
}

