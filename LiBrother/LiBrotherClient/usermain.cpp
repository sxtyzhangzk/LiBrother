#include "usermain.h"
#include "ui_usermain.h"
#include "mainwindow.h"
#include "userborrow.h"

usermain::usermain(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::usermain)
{
    ui->setupUi(this);
}

usermain::~usermain()
{
    delete ui;
}





void usermain::on_pushButton_2_clicked()
{
    userborrow userborrow1;
    userborrow1.exec();
}
