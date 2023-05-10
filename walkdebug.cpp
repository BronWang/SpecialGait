#include "walkdebug.h"
#include "ui_walkdebug.h"

WalkDebug::WalkDebug(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::WalkDebug)
{
    ui->setupUi(this);
    ui->pushButtonForward->setEnabled(false);
    ui->pushButtonLeft->setEnabled(false);
    ui->pushButtonRight->setEnabled(false);
    ui->pushButtonStop->setEnabled(false);
}

void WalkDebug::closeEvent(QCloseEvent *e)
{
    emit cmd_walk_stop();
    ui->pushButtonForward->setEnabled(false);
    ui->pushButtonLeft->setEnabled(false);
    ui->pushButtonRight->setEnabled(false);
    ui->pushButtonStop->setEnabled(false);
}

WalkDebug::~WalkDebug()
{
    delete ui;
}

void WalkDebug::on_pushButtonStartWalk_clicked()
{
    emit cmd_walk_start_walk();
    ui->pushButtonForward->setEnabled(true);
    ui->pushButtonLeft->setEnabled(true);
    ui->pushButtonRight->setEnabled(true);
    ui->pushButtonStop->setEnabled(true);
    ui->pushButtonStartWalk->setEnabled(false);
    ui->pushButtonEndWalk->setEnabled(true);
}


void WalkDebug::on_pushButtonForward_clicked()
{
    emit cmd_walk_forward();
}


void WalkDebug::on_pushButtonLeft_clicked()
{
    emit cmd_walk_left();
}


void WalkDebug::on_pushButtonRight_clicked()
{
    emit cmd_walk_right();
}


void WalkDebug::on_pushButtonStop_clicked()
{
    emit cmd_walk_stop();
}


void WalkDebug::on_pushButtonEndWalk_clicked()
{
    emit cmd_walk_end_walk();
    emit cmd_walk_stop();
    ui->pushButtonForward->setEnabled(false);
    ui->pushButtonLeft->setEnabled(false);
    ui->pushButtonRight->setEnabled(false);
    ui->pushButtonStop->setEnabled(false);
    ui->pushButtonStartWalk->setEnabled(true);
    ui->pushButtonEndWalk->setEnabled(false);
}

