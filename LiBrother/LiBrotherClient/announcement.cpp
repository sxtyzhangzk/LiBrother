#include "announcement.h"
#include "ui_announcement.h"
#include "client_interfaces.h"

announcement::announcement(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::announcement)
{
    ui->setupUi(this);
}

announcement::~announcement()
{
    delete ui;
}

void announcement::on_pushButton_5_clicked()
{

   announcement announcement1;
   announcement1.exec();
}
