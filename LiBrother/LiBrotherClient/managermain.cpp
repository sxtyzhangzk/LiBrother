#include "managermain.h"
#include "ui_managermain.h"
#include "qstring.h"
#include "userborrow.h"
#include "newbookedit.h"

managermain::managermain(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::managermain)
{
    ui->setupUi(this);
}

managermain::~managermain()
{
    delete ui;
}

void managermain::on_pushButton_clicked()
{
    userborrow userborrow1;
    userborrow1.exec();
}
void managermain::on_pushButton_2_clicked()
{
    newbookedit newbookedit1;
    newbookedit1.exec();
}


void managermain::on_pushButton_4_clicked()
{
    close();
}
