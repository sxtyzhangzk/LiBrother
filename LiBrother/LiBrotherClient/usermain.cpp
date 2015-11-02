#include "usermain.h"
#include "ui_usermain.h"
#include "mainwindow.h"

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

void usermain::on_label_3_linkActivated(const QString &link)
{

}
