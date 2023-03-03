#include "gaitupload.h"
#include "ui_gaitupload.h"

GaitUpload::GaitUpload(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::GaitUpload)
{
    ui->setupUi(this);
}

GaitUpload::~GaitUpload()
{
    delete ui;
}

void GaitUpload::displayProcessBar(int cur, int total)
{

    ui->progressBar->setMaximum(total);
    ui->progressBar->setValue(cur);
    if(cur == total){
        ui->pushButton->setEnabled(true);
    }else {
        ui->pushButton->setEnabled(false);
    }

}

void GaitUpload::on_pushButton_clicked()
{

    emit startFileUpload();

}

void GaitUpload::clearProcessBar()
{
    ui->progressBar->setValue(0);
    ui->pushButton->setEnabled(true);
}

