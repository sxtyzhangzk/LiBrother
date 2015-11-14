#include "userborrow.h"
#include "ui_userborrow.h"
#include "QStringListModel"
#include "QString"

userborrow::userborrow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::userborrow)
{
    ui->setupUi(this);




    //model = new QStringListModel(strList);
}

userborrow::~userborrow()
{
    delete ui;
    //delete model;
}



void userborrow::on_pushButton_4_clicked()    /*搜索按键*/
{
    /*QString name[100];
    int i;
    for(i=1;i<=4;i++)
    {
        name[i] = this->ui->lineEdit->text();
        //this->ui->pushButton_4->clicked();
        //this->ui->lineEdit->clear();
    }*/
    //QStringList strList;
        strList.append(ui->lineEdit->text());
        i++;
        //strList.append(name[2]);
        //strList.append(name[3]);
        //strList.append(name[4]);
        if(i == 4)
        {model = new QStringListModel(strList);

            ui->listView->setModel(model);

        }


  // QStringListModel *model = new QStringListModel(strList);

  // ui->listView->setModel(model);
   //delete model;

}

void userborrow::on_listView_doubleClicked(const QModelIndex &index)  /*书目表*/
{

}

void userborrow::on_pushButton_clicked()
{
    delete model;
    close();
}
