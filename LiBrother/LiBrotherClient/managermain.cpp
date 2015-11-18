#include "managermain.h"
#include "ui_managermain.h"
#include "announcement.h"
#include "qstring.h"

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

}
void managermain::on_pushButton_2_clicked()
{

}

void managermain::on_pushButton_3_clicked()
{
    QString content;
    announcement announcement1;
    announcement1.exec();
}





