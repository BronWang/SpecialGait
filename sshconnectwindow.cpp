#include "sshconnectwindow.h"
#include "ui_sshconnectwindow.h"



sshConnectWindow::sshConnectWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::sshConnectWindow)
{
    ui->setupUi(this);
    connect(ui->lineEditCmd, SIGNAL(returnPressed()), ui->btnSendCmd, SLOT(click()), Qt::UniqueConnection);
}

void sshConnectWindow::initSshParm(QString strIp, int nPort, QString strPwd, QString strUser)
{
    m_strIp = strIp;
    m_nPort = nPort;
    m_strUser = strUser;
    m_strPwd = strPwd;
    m_strIpPort = m_strIp + ":" + QString::number(m_nPort);

}

void sshConnectWindow::init()
{
//    m_pThread = new QThread();
//    connect(m_pThread,SIGNAL(finished()),this,SLOT(slotThreadFinished()));
//    this->moveToThread(m_pThread);
//    m_pThread->start();

    connect(this,SIGNAL(sigInitForClild()),this,SLOT(slotInitForClild()));
    emit sigInitForClild();
}

void sshConnectWindow::unInit()
{
    //m_pThread->quit();
}

sshConnectWindow::~sshConnectWindow()
{
    delete ui;
    if(nullptr != m_pSshSocket){
        delete m_pSshSocket;
        m_pSshSocket = nullptr;
    }
}

int sshConnectWindow::send(QString strMessage)
{
    qDebug()<<"CConnectionForSshClient ssh send "<<strMessage;

    int nSize = 0;
    if(m_bConnected && m_bSendAble){
       nSize = m_shell->write(strMessage.toLatin1().data());
    }else{
       qDebug()<<"CConnectionForSshClient::send() ssh未连接 或 shell未连接:"<<getIpPort();
    }

    return nSize;

}

void sshConnectWindow::slotResetConnection(QString strIpPort)
{
    if(this->getIpPort() == strIpPort){
        this->slotDisconnected();
    }
}

void sshConnectWindow::slotSend(QString strIpPort, QString strMessage)
{
    if(0 != m_strIpPort.compare(strIpPort)){
        return;
    }

    send(strMessage);
}

void sshConnectWindow::slotSend(QString strMsg)
{
    send(strMsg);
}

void sshConnectWindow::slotSendByQByteArray(QString strIpPort, QByteArray arrMsg)
{
    if(0 != m_strIpPort.compare(strIpPort)){
        return;
    }

    if(m_bConnected){
       m_shell->write(arrMsg);
    }else{
       qDebug()<<"CConnectionForSshClient::send(QString strMessage) 发送失败 未建立连接:"<<getIpPort();
    }
}

void sshConnectWindow::slotDisconnected()
{
    m_pSshSocket->disconnectFromHost();

    // 添加修改
    m_bConnected = false;
    emit sigConnectStateChanged(m_bConnected,m_strIp,m_nPort);
}

void sshConnectWindow::slotDataReceived()
{
    QByteArray byteRecv = m_shell->readAllStandardOutput();
    QString strRecv = QString::fromUtf8(byteRecv);


    if(!strRecv.isEmpty())
        emit sigDataArrived(strRecv, m_strIp, m_nPort);
}

void sshConnectWindow::slotInitForClild()
{
    m_argParameters.setPort(m_nPort);
    m_argParameters.setUserName(m_strUser);
    m_argParameters.setPassword(m_strPwd);
    m_argParameters.setHost(m_strIp);
    m_argParameters.timeout = 10;
    m_argParameters.authenticationType =QSsh::SshConnectionParameters::AuthenticationTypePassword; //密码方式连接

    slotCreateConnection(); //连接

//    m_pTimer = new QTimer(this);
//    m_pTimer->setInterval(RECONNET_SPAN_TIME);
//    connect(m_pTimer,SIGNAL(timeout()),this,SLOT(slotCreateConnection()));
//    m_pTimer->start();//启动心跳定时器，每隔一段时间进入slotCreateConnection判断是否需要重连

}

void sshConnectWindow::slotCreateConnection()
{
    qDebug()<<"CConnectionForSshClient::slotCreateConnection检查连接 ";

    if(true == m_bConnected)
        return;

    if(nullptr == m_pSshSocket){
        m_pSshSocket = new QSsh::SshConnection(m_argParameters);
        connect(m_pSshSocket,SIGNAL(connected()),SLOT(slotConnected()));
        connect(m_pSshSocket,SIGNAL(error(QSsh::SshError)),SLOT(slotSshConnectError(QSsh::SshError)));
    }
    m_pSshSocket->connectToHost();
    qDebug()<<"CConnectionForSshClient::slotCreateConnection() 以ssh方式 尝试连接:"<<getIpPort();

}

void sshConnectWindow::slotConnected()
{
    qDebug()<<"CConnectionForSshClient::slotConnected ssh已连接到:"<<getIpPort();
    //m_pTimer->stop();

    m_shell = m_pSshSocket->createRemoteShell();
    connect(m_shell.data(), SIGNAL(started()), SLOT(slotShellStart()));
    connect(m_shell.data(), SIGNAL(readyReadStandardOutput()), SLOT(slotDataReceived()));
    connect(m_shell.data(), SIGNAL(readyReadStandardError()), SLOT(slotShellError()));
    m_shell.data()->start();

    m_bConnected = true;
    emit sigConnectStateChanged(m_bConnected,m_strIp,m_nPort);
}

void sshConnectWindow::slotThreadFinished()
{
    m_pThread->deleteLater();
    this->deleteLater();
}

void sshConnectWindow::slotSshConnectError(QSsh::SshError sshError)
{
    m_bSendAble = false;
    m_bConnected = false;
    emit sigConnectStateChanged(m_bConnected,m_strIp,m_nPort);

    //m_pTimer->start();

    switch(sshError){
    case QSsh::SshNoError:
        qDebug()<<"slotSshConnectError SshNoError"<<getIpPort();
        break;
    case QSsh::SshSocketError:
        qDebug()<<"slotSshConnectError SshSocketError"<<getIpPort(); //拔掉网线是这种错误
        break;
    case QSsh::SshTimeoutError:
        qDebug()<<"slotSshConnectError SshTimeoutError"<<getIpPort();
        break;
    case QSsh::SshProtocolError:
        qDebug()<<"slotSshConnectError SshProtocolError"<<getIpPort();
        break;
    case QSsh::SshHostKeyError:
        qDebug()<<"slotSshConnectError SshHostKeyError"<<getIpPort();
        break;
    case QSsh::SshKeyFileError:
        qDebug()<<"slotSshConnectError SshKeyFileError"<<getIpPort();
        break;
    case QSsh::SshAuthenticationError:
        qDebug()<<"slotSshConnectError SshAuthenticationError"<<getIpPort();
        break;
    case QSsh::SshClosedByServerError:
        qDebug()<<"slotSshConnectError SshClosedByServerError"<<getIpPort();
        break;
    case QSsh::SshInternalError:
        qDebug()<<"slotSshConnectError SshInternalError"<<getIpPort();
        break;
    default:
        break;
    }

}

void sshConnectWindow::slotShellStart()
{
    m_bSendAble = true;
    qDebug()<<"CConnectionForSshClient::slotShellStart Shell已连接:"<<getIpPort();
}

void sshConnectWindow::slotShellError()
{
    qDebug()<<"CConnectionForSshClient::slotShellError Shell发生错误:"<<getIpPort();
}

void sshConnectWindow::setConnectState(bool bState)
{
    if(!bState)
        ui->labelConState->setStyleSheet("QLabel{background-color:#ff0000;border-radius:5px;}");
    else
        ui->labelConState->setStyleSheet("QLabel{background-color:#00ff00;border-radius:5px;}");
}

void sshConnectWindow::createConnection()
{
    QString strIp = ui->lineEditTarIP->text();
    QString strUser = ui->lineEditUserName->text();
    QString strPwd = ui->lineEditPwd->text();
    initSshParm(strIp,22,strPwd,strUser);
    init();

}

void sshConnectWindow::slotConnectStateChanged(bool bState, QString strIp, int nPort)
{
    Q_UNUSED(strIp)
    Q_UNUSED(nPort)

    m_sshConnectState = bState;
    setConnectState(m_sshConnectState);
    if(m_sshConnectState)
        ui->btnConnect->setText("断开");
    else
        ui->btnConnect->setText("连接");
}

void sshConnectWindow::slotDataArrived(QString strMsg, QString strIp, int nPort)
{
    Q_UNUSED(strIp)
    Q_UNUSED(nPort)

    ui->plainTextEditWindow->appendPlainText(strMsg);
    ui->plainTextEditWindow->moveCursor(QTextCursor::End);
}

void sshConnectWindow::on_btnConnect_clicked()
{
    if(!m_sshConnectState){
        ui->btnConnect->setText("连接中...");
        createConnection();  //发起连接
    }else{
        ui->btnConnect->setText("连接");
        m_sshConnectState = false;
        emit sigDisconnected();//断开连接
        setConnectState(m_sshConnectState);
    }

}


void sshConnectWindow::on_btnSendCmd_clicked()
{
    if(m_sshConnectState){
        QString strCmd = ui->lineEditCmd->text();
        strCmd += "\n"; //添加回车
        emit sigSend(strCmd);
    }
    ui->lineEditCmd->clear();

}


void sshConnectWindow::on_pushButtonClearContent_clicked()
{
    ui->plainTextEditWindow->clear();
}




