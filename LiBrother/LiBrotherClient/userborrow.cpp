#include "userborrow.h"
#include "ui_userborrow.h"
#include "QStringListModel"
#include "QString"
#include "bookdata.h"
#include "client_interfaces.h"
#include "QMessageBox"
#include "qmessagebox.h"
#include <QLineEdit>

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
    bookdata bookdata1;
    bookdata1.setBookID(bID);

    bookdata1.exec();
}






void userborrow::on_pushButton_4_clicked()    /*搜索按键*/
{
      QString ISBN;
      ISBN = ui->lineEdit->text();
    IClassFactoryClient *pFactory;
    getClassFactory(&pFactory);
    ILibrary *pLibrary;
    pFactory->getLibrary(&pLibrary);
    IBook *pBook;
    pLibrary->queryByISBN(ISBN.toStdString().c_str(),&pBook);
    TBookBasicInfo info;
      pBook->getBasicInfo(info);
    std::string name = info.name;
    QString name1 = QString::fromStdString(name);
    QLabel *qlabel;
    qlabel->setText(name1);
    pBook->Release();
    pLibrary->Release();
    pFactory->Release();
}


void userborrow::on_pushButton_4_clicked()
{

    //清空listwidge
        IClassFactoryClient *factory1;
        getClassFactory(&factory1);
        ILibrary *library1;
        factory1->getLibrary(&library1);

        QString bSearch = ui->lineEdit->text();
        QString bOption = ui->comboBox->currentText();
        std::string bSearch1 = bSearch.toStdString();
        std::string bOption1 = bOption.toStdString();
        TBookBasicInfo basic1;
        IBook *iBook1;


        QString bName1;
            if(bOption1 == "ISBN")//按照ISBN号找到唯一的一本
            {
            bool bPd = library1->queryByISBN(bSearch1.c_str(), &iBook1);
            if(bPd)
            {
            iBook1->getBasicInfo(basic1);
            bName1 = QString::fromStdString(basic1.name);

            QListWidgetItem *item = new QListWidgetItem;
            item->setText(bName1);
            item->setData(Qt::UserRole,basic1.id);//data中存了之后要用到的书本ID号
            ui->listWidget->addItem(item);
            }
            else{QMessageBox::information(this,"Title","操作错误");}
            }

            else//按照书名就行搜索
            {
            IFvector vBooks;
            int bNum = library1->queryByName(bSearch1.c_str(),vBooks,INT_MAX,1);
            if(bNum > 0)
            {
            int i;
            for(i=0;i<bNum;i++)
               {


                 ((IBook*)vBooks[i])->getBasicInfo(basic1);
                bName1 = QString::fromStdString(basic1.name);

                QListWidgetItem *item = new QListWidgetItem;
                item->setText(bName1);
                item->setData(Qt::UserRole,basic1.id);
                ui->listWidget->addItem(item);


                }
            }
            else{QMessageBox::information(this,"Title","没有找到相关书本");}
            }

}



