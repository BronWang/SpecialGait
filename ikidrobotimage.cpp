#include "ikidrobotimage.h"
#include "ui_ikidrobotimage.h"

IkidRobotImage::IkidRobotImage(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::IkidRobotImage)
{
    ui->setupUi(this);
    QPixmap pixmap(":/images/images/ikid_robot.jpg");
    pixmap.scaled(pixmap.size(),Qt::KeepAspectRatio);
    ui->label->setScaledContents(true);
    ui->label->setPixmap(pixmap);
}

IkidRobotImage::~IkidRobotImage()
{
    delete ui;
}
