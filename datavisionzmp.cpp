#include "datavisionzmp.h"
#include "ui_datavisionzmp.h"

DataVisionZmp::DataVisionZmp(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::DataVisionZmp)
{
    ui->setupUi(this);
}

DataVisionZmp::~DataVisionZmp()
{
    delete ui;
}
