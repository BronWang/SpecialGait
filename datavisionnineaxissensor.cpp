#include "datavisionnineaxissensor.h"
#include "ui_datavisionnineaxissensor.h"

DataVisionNineAxisSensor::DataVisionNineAxisSensor(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::DataVisionNineAxisSensor)
{
    ui->setupUi(this);
}

DataVisionNineAxisSensor::~DataVisionNineAxisSensor()
{
    delete ui;
}
