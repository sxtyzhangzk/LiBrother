#include "signupmain.h"
#include "ui_signupmain.h"

signupmain::signupmain(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::signupmain)
{
    ui->setupUi(this);
}

signupmain::~signupmain()
{
    delete ui;
}

void signupmain::on_pushButton_3_clicked()
{
    close();
}

void signupmain::on_pushButton_2_clicked()
{

}
