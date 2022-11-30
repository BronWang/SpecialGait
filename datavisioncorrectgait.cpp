#include "datavisioncorrectgait.h"
#include "ui_datavisioncorrectgait.h"

DataVisionCorrectGait::DataVisionCorrectGait(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::DataVisionCorrectGait)
{
    ui->setupUi(this);
}

DataVisionCorrectGait::~DataVisionCorrectGait()
{
    delete ui;
}
