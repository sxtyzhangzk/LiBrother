#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "login.h"
#include "signup.h"
#include "client_interfaces.h"
#include "qlistwidget.h"
#include "QMessageBox"
#include "bookdata.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    login login1;
    login1.exec();
}

void MainWindow::on_pushButton_2_clicked()
{
    signup signup1;
    signup1.exec();
}

void MainWindow::on_pushButton_4_clicked()
{
        ui->listWidget->clear();
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
                    for(i=1;i<=bNum;i++)
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

            factory1->Release();
            library1->Release();
            iBook1->Release();
}




void MainWindow::on_pushButton_5_clicked()//主界面中搜索出来的书目，跳转到该书的信息窗口
{
    QListWidgetItem *item = ui->listWidget->currentItem();
    int bID = item->data(Qt::UserRole).toInt();
    bookdata bookdata1;
    bookdata1.setBookID(bID);
    bookdata1.exec();


}
