#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDir>
#include <QFileDialog>
#define FixedColumnCount 20
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

    labelSocketState = new QLabel("Socket状态：");
    labelSocketState->setMinimumWidth(250);
    labelSeverIP = new QLabel("服务端IP：");
    labelSeverIP->setMinimumWidth(250);
    ui->statusbar->addWidget(labelSocketState);
    ui->statusbar->addWidget(labelSeverIP);

    //tableview部分
    theModel = new QStandardItemModel(5, FixedColumnCount, this);
    theSelection = new QItemSelectionModel(theModel);
    ui->tableView->setModel(theModel);
    ui->tableView->setSelectionModel(theSelection);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tableView->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    connect(theSelection,SIGNAL(currentChanged(QModelIndex,QModelIndex)),this,SLOT(on_tableCurrentChanged(QModelIndex,QModelIndex)));
    connect(theSelection,SIGNAL(selectionChanged(QItemSelection,QItemSelection)),this,SLOT(on_tableSelectionChanged(QItemSelection,QItemSelection)));
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
}


void MainWindow::on_saveFile_triggered()
{
    QString aFileName=currentFileName;

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
    str = tempstr+"\n"+ ui->lineEditGaitID->text();
    stream << str << "\n";
    ui->plainTextEditGaitData->appendPlainText(str);
    tempstr = "GaitDescription";
    str = tempstr+"\n"+ui->textEditGaitDesc->toPlainText();
    stream << str << "\n";
    ui->plainTextEditGaitData->appendPlainText(str);
    tempstr = "zero_point";
    QString temp_zero_point = "0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0";
    str = tempstr+"\n"+temp_zero_point;
    stream << str << "\n";
    ui->plainTextEditGaitData->appendPlainText(str);
    str = "Gait_Frame";
    stream << str << "\n";
    ui->plainTextEditGaitData->appendPlainText(str);
    for(int i = 0;i<theModel->rowCount();i++){
        str = "";
        int j = 0;
        for(;j<theModel->columnCount()-1;j++){
            item = theModel->item(i,j);
            str = str + item->text()+",";
        }
        item = theModel->item(i,j);
        str = str + item->text();
        stream << str << "\n";
        ui->plainTextEditGaitData->appendPlainText(str);
    }

    aFile.close();
    return;
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
    if(currentFileName != ""){  // 加载文件后才使能相应按钮
        ui->btnDelCurFrame->setEnabled(true);
        ui->btnAppendFrame->setEnabled(true);
        ui->btnInsertFrame->setEnabled(true);
        ui->btnRecordCurFrame->setEnabled(true);
        ui->btnExecLaterFrame->setEnabled(true);
        ui->btnExecPreFrame->setEnabled(true);
        ui->btnResetFrame->setEnabled(true);
        ui->scrollArea->setEnabled(true);
        ui->btnReturnCentralValue->setEnabled(true);
        // 把滑块数值和表格数值相对应
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
    }
}

void MainWindow::iniModelFromStringList(QStringList &fileContent)
{
    theModel->setRowCount(fileContent.count()-7);
    QString temp;
    temp = fileContent.at(0);
    if(temp != "GaitID"){
        QMessageBox::warning(this,"警告","文件格式不正确！");
        return;
    }
    QString gaitID = fileContent.at(1);
    ui->lineEditGaitID->setText(gaitID);
    temp = fileContent.at(2);
    if(temp != "GaitDescription"){
        QMessageBox::warning(this,"警告","文件格式不正确！");
        return;
    }
    QString gaitDesc = fileContent.at(3);
    ui->textEditGaitDesc->setText(gaitDesc);
    temp = fileContent.at(4);
    if(temp != "zero_point"){
        QMessageBox::warning(this,"警告","文件格式不正确！");
        return;
    }
    QString zero_point = fileContent.at(5);
    QStringList zero_point_list = zero_point.split(",",QString::SkipEmptyParts);
    temp = fileContent.at(6);
    if(temp != "Gait_Frame"){
        QMessageBox::warning(this,"警告","文件格式不正确！");
        return;
    }
    for(int i = 7;i < fileContent.count();i++){
        QString gait_frame = fileContent.at(i);
        QStringList gait_frame_list = gait_frame.split(",",QString::SkipEmptyParts);
        for(int j = 0; j < FixedColumnCount;j++){
            QStandardItem *item = new QStandardItem(gait_frame_list.at(j));
            theModel->setItem(i-7,j,item);
        }
    }

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

    QTextStream stream(&aFile);
    stream.setCodec("utf-8");
    QString str;
    QStandardItem *item;
    ui->plainTextEditGaitData->clear();
    QString tempstr;
    tempstr = "GaitID";
    str = tempstr+"\n"+ ui->lineEditGaitID->text();
    stream << str << "\n";
    ui->plainTextEditGaitData->appendPlainText(str);
    tempstr = "GaitDescription";
    str = tempstr+"\n"+ui->textEditGaitDesc->toPlainText();
    stream << str << "\n";
    ui->plainTextEditGaitData->appendPlainText(str);
    tempstr = "zero_point";
    QString temp_zero_point = "0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0";
    str = tempstr+"\n"+temp_zero_point;
    stream << str << "\n";
    ui->plainTextEditGaitData->appendPlainText(str);
    str = "Gait_Frame";
    stream << str << "\n";
    ui->plainTextEditGaitData->appendPlainText(str);
    for(int i = 0;i<theModel->rowCount();i++){
        str = "";
        int j = 0;
        for(;j<theModel->columnCount()-1;j++){
            item = theModel->item(i,j);
            str = str + item->text()+",";
        }
        item = theModel->item(i,j);
        str = str + item->text();
        stream << str << "\n";
        ui->plainTextEditGaitData->appendPlainText(str);
    }

    aFile.close();
    return;
}


void MainWindow::on_btnAppendFrame_clicked()
{
    QList<QStandardItem*> itemList;
    QStandardItem *item;
    QStandardItem *newItem;
    for (int i = 0; i < FixedColumnCount; ++i) {
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
    for (int i = 0; i < FixedColumnCount; ++i) {
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
        QString ready_send_msg = "special_giat_data";
        ready_send_msg.append("\n");
        ready_send_msg += QString::asprintf("%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f",
                                            ui->horizontalSlider->value()/180.0*PI,
                                            ui->horizontalSlider_2->value()/180.0*PI,
                                            ui->horizontalSlider_3->value()/180.0*PI,
                                            ui->horizontalSlider_4->value()/180.0*PI,
                                            ui->horizontalSlider_5->value()/180.0*PI,
                                            ui->horizontalSlider_6->value()/180.0*PI,
                                            ui->horizontalSlider_7->value()/180.0*PI,
                                            ui->horizontalSlider_8->value()/180.0*PI,
                                            ui->horizontalSlider_9->value()/180.0*PI,
                                            ui->horizontalSlider_10->value()/180.0*PI,
                                            ui->horizontalSlider_11->value()/180.0*PI,
                                            ui->horizontalSlider_12->value()/180.0*PI,
                                            ui->horizontalSlider_13->value()/180.0*PI,
                                            ui->horizontalSlider_14->value()/180.0*PI,
                                            ui->horizontalSlider_15->value()/180.0*PI,
                                            ui->horizontalSlider_16->value()/180.0*PI,
                                            ui->horizontalSlider_17->value()/180.0*PI,
                                            ui->horizontalSlider_18->value()/180.0*PI,
                                            ui->horizontalSlider_19->value()/180.0*PI,
                                            ui->horizontalSlider_20->value()/180.0*PI);
        ui->plainTextEditGaitData->appendPlainText("[out] "+ready_send_msg);
        QByteArray  send_msg=ready_send_msg.toUtf8();
        tcpClient->write(send_msg);
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
}


void MainWindow::on_btnExecPreFrame_clicked()
{
    if(ui->checkBoxNetwork->isChecked()){
        QModelIndex index = theSelection->currentIndex();
        if(index.row() == 0){
            QMessageBox::warning(this,"警告","当前帧为第一帧！无法执行前一帧");
            return;
        }
        QString ready_send_msg = "special_giat_data";
        ready_send_msg.append("\n");
        for(int i = 0; i < FixedColumnCount-1; i++){
            ready_send_msg += QString::asprintf("%f,",theModel->item(index.row()-1,i)->text().toInt()/180.0*PI);
        }
        ready_send_msg += QString::asprintf("%f", theModel->item(index.row()-1,FixedColumnCount-1)->text().toInt()/180.0*PI);
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
        QString ready_send_msg = "special_giat_data";
        ready_send_msg.append("\n");
        for(int i = 0; i < FixedColumnCount-1; i++){
            ready_send_msg += QString::asprintf("%f,",theModel->item(index.row()+1,i)->text().toInt()/180.0*PI);
        }
        ready_send_msg += QString::asprintf("%f", theModel->item(index.row()+1,FixedColumnCount-1)->text().toInt()/180.0*PI);
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
        QString ready_send_msg = "special_giat_data";
        ready_send_msg.append("\n");
        for(int i = 0; i < FixedColumnCount-1; i++){
            ready_send_msg += QString::asprintf("%f,",theModel->item(0,i)->text().toInt()/180.0*PI);
        }
        ready_send_msg += QString::asprintf("%f", theModel->item(0,FixedColumnCount-1)->text().toInt()/180.0*PI);
        ui->plainTextEditGaitData->appendPlainText("[out] "+ready_send_msg);
        QByteArray  send_msg=ready_send_msg.toUtf8();
        tcpClient->write(send_msg);
        theSelection->clearSelection();
        theSelection->setCurrentIndex(theModel->index(0,0),QItemSelectionModel::Select);
    }
}


void MainWindow::on_btnExecList_clicked()
{
    if(ui->checkBoxNetwork->isChecked()){
        for (int i = 0; i < theModel->rowCount(); ++i) {
            QString ready_send_msg = "special_giat_data";
            ready_send_msg.append("\n");
            theSelection->clearSelection();
            theSelection->setCurrentIndex(theModel->index(i,0),QItemSelectionModel::Select);
            for(int j = 0; j < FixedColumnCount-1; j++){
                ready_send_msg += QString::asprintf("%f,",theModel->item(i,j)->text().toInt()/180.0*PI);
            }
            ready_send_msg += QString::asprintf("%f", theModel->item(i,FixedColumnCount-1)->text().toInt()/180.0*PI);
            ui->plainTextEditGaitData->appendPlainText("[out] "+ready_send_msg);
            QByteArray  send_msg=ready_send_msg.toUtf8();
            tcpClient->write(send_msg);
            tcpClient->waitForReadyRead(1000);
            qApp->processEvents();
        }
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
    tempstr = "GaitDescription";
    str = tempstr+"\n";
    str.append(" ");
    stream << str << "\n";
    ui->plainTextEditGaitData->appendPlainText(str);
    tempstr = "zero_point";
    QString temp_zero_point = "0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0";
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

