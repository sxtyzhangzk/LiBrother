#include "userborrow.h"
#include "ui_userborrow.h"
#include "QStringListModel"
#include "QString"
#include "bookdata.h"
#include "client_interfaces.h"
#include "QMessageBox"
#include "qmessagebox.h"
#include <QLineEdit>
#include "bookdataedit.h"

userborrow::userborrow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::userborrow)
{
    ui->setupUi(this);

}

userborrow::~userborrow()
{
    delete ui;
}






void userborrow::on_pushButton_clicked()//选择一本书本，进入书本具体信息界面
{
    QListWidgetItem *item = ui->listWidget->currentItem();
    int bID = item->data(Qt::UserRole).toInt();
    bookdata bookdata1(bID);

    bookdata1.exec();
}



void userborrow::on_pushButton_4_clicked()
{
        ui->listWidget->clear();//先对widget进行清空，可能会出问题，后期可能会需要修改
        auto_iface<IClassFactoryClient> factory1;
        getClassFactory(&factory1);
        auto_iface<ILibrary> library1;
        factory1->getLibrary(&library1);

        QString bSearch = ui->lineEdit->text();
        QString bOption = ui->comboBox->currentText();//查看用户选择的搜索模式
        std::string bSearch1 = bSearch.toStdString();
        std::string bOption1 = bOption.toStdString();
        TBookBasicInfo basic1;
        auto_iface<IBook> iBook1;


        QString bName1;
            if(bOption1 == "ISBN")//按照ISBN号找到唯一的一本
            {
                bool bPd = library1->queryByISBN(bSearch1.c_str(), &iBook1);
                if(bPd)
                {
                   if(!iBook1->getBasicInfo(basic1)){close();QMessageBox::information(this,"Warning",u8"系统错误");return;}
                    bName1 = QString::fromStdString(basic1.name);

                    QListWidgetItem *item = new QListWidgetItem;
                    item->setText(bName1);
                    item->setData(Qt::UserRole,basic1.id);//data中存了之后要用到的书本ID号
                    ui->listWidget->addItem(item);
                }
                else{QMessageBox::information(this,"Warning",u8"无法通过ISBN号查询到此书");}
            }

            else//按照书名就行搜索
            {
                IFvector vBooks;
                int bNum = library1->queryByName(bSearch1.c_str(),vBooks,INT_MAX,1);
                if(bNum > 0)//查看是否查询到相关书本
                {
                int i;
                for(i=0;i<bNum;i++)
                   {


                     if(!((IBook*)vBooks[i])->getBasicInfo(basic1)){close();QMessageBox::information(this,"Warning",u8"系统错误");return;}
                    bName1 = QString::fromStdString(basic1.name);

                    QListWidgetItem *item = new QListWidgetItem;
                    item->setText(bName1);
                    item->setData(Qt::UserRole,basic1.id);
                    ui->listWidget->addItem(item);


                    }
                }
                else{QMessageBox::information(this,"Warning",u8"无法找到相关书本");}
            }

}




void userborrow::on_pushButton_2_clicked()
{
    auto_iface<IClassFactoryClient> factory3;
    getClassFactory(&factory3);
    auto_iface<IAuthManager> iUser;
    factory3->getAuthManager(&iUser);
    if(iUser->getAuthLevel() >= 1)
    {
        QListWidgetItem *item = ui->listWidget->currentItem();//获取选中的书本信息，并通过setBookID函数输入书本的ID号
        int bID = item->data(Qt::UserRole).toInt();
        bookdataedit bookdataedit1(bID);
        //bookdataedit1.setBookID(bID);
        bookdataedit1.exec();
    }
    else{QMessageBox::information(this,"Warning",u8"对不起，您没有权限");}

}
