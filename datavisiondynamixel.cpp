#include "datavisiondynamixel.h"
#include "ui_datavisiondynamixel.h"

DataVisionDynamixel::DataVisionDynamixel(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::DataVisionDynamixel)
{
    ui->setupUi(this);
}

DataVisionDynamixel::~DataVisionDynamixel()
{
    delete ui;
}
