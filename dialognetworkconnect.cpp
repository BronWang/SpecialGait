#include "dialognetworkconnect.h"
#include "ui_dialognetworkconnect.h"

DialogNetworkConnect::DialogNetworkConnect(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogNetworkConnect)
{
    ui->setupUi(this);
}

DialogNetworkConnect::~DialogNetworkConnect()
{
    delete ui;
}

QString DialogNetworkConnect::getLocalIPv4()
{
    QString hostName = QHostInfo::localHostName();
    QHostInfo hostInfo = QHostInfo::fromName(hostName);

    QString localIPv4 = "";
    QList<QHostAddress>  addrList = hostInfo.addresses();
    if(addrList.isEmpty()) return localIPv4;
    foreach (auto addrItem, addrList) {
        if (QAbstractSocket::IPv4Protocol == addrItem.protocol()){
            localIPv4 = addrItem.toString();
            break;
        }
    }
    ui->lineEditLocalIP->setText(localIPv4);
    return localIPv4;

}

void DialogNetworkConnect::setLocaPort(quint16 port)
{
    ui->lineEditLocalPort->setText(QString::asprintf("%d",port));
}

void DialogNetworkConnect::clearLocalIP()
{
    ui->lineEditLocalIP->clear();
}

void DialogNetworkConnect::clearLocalPort()
{
    ui->lineEditLocalPort->clear();
}

void DialogNetworkConnect::on_btnConnectTarNet_clicked()
{
    QString IP = ui->lineEditTargetIP->text();
    uint port = ui->lineEditTargetPort->text().toUInt();
    emit connectNetwork(IP,port);
}


void DialogNetworkConnect::on_btnCancelCon_clicked()
{
    emit disConnectNetwork();
}

