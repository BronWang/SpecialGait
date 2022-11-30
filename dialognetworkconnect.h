#ifndef DIALOGNETWORKCONNECT_H
#define DIALOGNETWORKCONNECT_H

#include <QDialog>
#include <QTcpServer>
#include <QtNetwork>


namespace Ui {
class DialogNetworkConnect;
}

class DialogNetworkConnect : public QDialog
{
    Q_OBJECT

public:
    explicit DialogNetworkConnect(QWidget *parent = nullptr);
    ~DialogNetworkConnect();

    QString getLocalIPv4();  // 获取本机的IPv4地址
    void setLocaPort(quint16);  // 获取本机的port
    void clearLocalIP();  // 清空IPv4地址
    void clearLocalPort();  // 清空port


private:
    Ui::DialogNetworkConnect *ui;

signals:
    void connectNetwork(QString IP, quint16 port);
    void disConnectNetwork();
private slots:
    void on_btnConnectTarNet_clicked();
    void on_btnCancelCon_clicked();
};

#endif // DIALOGNETWORKCONNECT_H
