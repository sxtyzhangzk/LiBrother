#include "userborrow.h"
#include "ui_userborrow.h"
#include "QStringListModel"
#include "QString"
#include "bookdata.h"
userborrow::userborrow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::userborrow)
{
    ui->setupUi(this);
    ui->listWidget->addItem("现藏岳阳");
    ui->listWidget->addItem("yueyangzangxian");
    ui->listWidget->addItem("楼上是傻逼");
    for(int i = 0; i <= 5; i++)
    {
        ui->listWidget->addItem(QString::number(i) + "lalala123");
    }

    //model = new QStringListModel(strList);
}

userborrow::~userborrow()
{
    delete ui;
    //delete model;
}



//void userborrow::on_pushButton_4_clicked()    /*搜索按键*/
//{
    /*QString name[100];
    int i;
    for(i=1;i<=4;i++)
    {
        name[i] = this->ui->lineEdit->text();
        //this->ui->pushButton_4->clicked();
        //this->ui->lineEdit->clear();
    }*/
    //QStringList strList;
  //      strList.append(ui->lineEdit->text());
  //      i++;
  //      ui->lineEdit->setPlaceholderText(tr("输入你所需要查找的书名"));
        //strList.append(name[2]);
        //strList.append(name[3]);
        //strList.append(name[4]);
  //      if(i == 4)
  //      {model = new QStringListModel(strList);

  //          ui->listView->setModel(model);

    //    }


  // QStringListModel *model = new QStringListModel(strList);

  // ui->listView->setModel(model);
   //delete model;

//}


void userborrow::on_pushButton_clicked()
{
    QListWidgetItem *item = ui->listWidget->currentItem();
    bookdata bookdata1;
    bookdata1.exec();
    item->setTextColor(Qt::blue);
}


