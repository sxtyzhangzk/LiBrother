#include "usermain.h"
#include "ui_usermain.h"
#include "mainwindow.h"
#include "userborrow.h"
#include "bookdata.h"

usermain::usermain(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::usermain)
{
    ui->setupUi(this);
    ui->label_2->setText("王辰");
    int i;
    for(i=1;i<=6;i++)
    {
    QListWidgetItem *item = new QListWidgetItem;
    item->setText(QString("大拇指的哥哥")+QString::number(i));
    item->setData(Qt::UserRole,i);
    ui->listWidget->addItem(item);
    }
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

void usermain::on_pushButton_3_clicked()
{
    QListWidgetItem *item = ui->listWidget->currentItem();
    int a = item->data(Qt::UserRole).toInt();
    ui->label_3->setText(QString::number(a));
    ui->listWidget->takeItem(ui->listWidget->currentRow());

    //item->setTextColor(Qt::blue);
}
