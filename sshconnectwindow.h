#ifndef SSHCONNECTWINDOW_H
#define SSHCONNECTWINDOW_H

#include <QMainWindow>
#include <sshconnection.h>
#include <sshremoteprocess.h>
#include <sftpchannel.h>
#include <QTimer>
#include <QHostAddress>
#include <QThread>

#define RECONNET_SPAN_TIME (1000*10)  //连接状态心跳


namespace Ui {
class sshConnectWindow;
}

class sshConnectWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit sshConnectWindow(QWidget *parent = nullptr);
    void initSshParm(QString strIp,int nPort = 22,QString strPwd = "123456",QString strUser = "root");
    void init();
    void unInit();
    ~sshConnectWindow();

private:
    Ui::sshConnectWindow *ui;

private:
    QThread *m_pThread = nullptr;
    bool m_sshConnectState = false;
    bool m_bConnected = false;
    bool m_bIsFirstConnect = true;
    bool m_bSendAble = false;

    QTimer *m_pTimer;

    QString m_strIp = "";
    int m_nPort = -1;
    QString m_strUser;
    QString m_strPwd;
    QString m_strIpPort;

    QSsh::SshConnectionParameters m_argParameters;
    QSsh::SshConnection *m_pSshSocket = nullptr;
    QSharedPointer<QSsh::SshRemoteProcess> m_shell;
signals:
    void sigInitForClild();
    void sigConnectStateChanged(bool bState,QString strIp,int nPort);
    void sigDataArrived(QString strMsg,QString strIp, int nPort);
private:
    int send(QString strMessage);
    QString getIpPort(){return m_strIp + ":" + QString::number(m_nPort);}
public slots:
    void slotResetConnection(QString strIpPort);
    void slotSend(QString strIpPort,QString strMessage);
    void slotSend(QString strMsg);
    void slotSendByQByteArray(QString strIpPort,QByteArray arrMsg);
    void slotDisconnected();
    void slotDataReceived();
private slots:
    void slotInitForClild();
    void slotCreateConnection();
    void slotConnected();

    void slotThreadFinished();

    void slotSshConnectError(QSsh::SshError sshError);
    void slotShellStart();
    void slotShellError();
// UI界面相关
private:
    void setConnectState(bool bState);
    void createConnection();
private slots:
    void slotConnectStateChanged(bool bState,QString strIp,int nPort);
    void slotDataArrived(QString strMsg,QString strIp, int nPort);
    void on_btnConnect_clicked();

    void on_btnSendCmd_clicked();

    void on_pushButtonClearContent_clicked();


signals:
    void sigSend(QString strMsg);
    void sigDisconnected();


};

#endif // SSHCONNECTWINDOW_H
