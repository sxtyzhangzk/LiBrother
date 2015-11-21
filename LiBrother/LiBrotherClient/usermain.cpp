#include "usermain.h"
#include "ui_usermain.h"
#include "mainwindow.h"
#include "userborrow.h"
#include "bookdata.h"
#include "client_interfaces.h"
#include "QMessageBox"
#include "changepassword.h"

usermain::usermain(QWidget *parent) ://开场直接显示所有用户已借的书本
    QDialog(parent),
    ui(new Ui::usermain)
{
    ui->setupUi(this);

    getClassFactory(&factory1);


    IAuthManager *iUser;//构建iauth为了之后使用getcurrentuser
    factory1->getAuthManager(&iUser);
    IUser *iUser1;
    bool uPd = iUser->getCurrentUser(&iUser1);//获取当前登陆的用户
    if(uPd)
    {
        TUserBasicInfo basic1;
        if(iUser1->getBasicInfo(basic1))//获取当前用户信息
           {
            QString uName1 = QString::fromStdString(basic1.name);
            ui->label_2->setText(uName1);//两个显示
            ui->label_5->setText(QString::number(basic1.id));
            std::vector<TBorrowInfo> basic2;//已经借了的书目
            if(iUser1->getBorrowedBooks(basic2))
            {
                int i;

                ILibrary *library2;
                factory1->getLibrary(&library2);

                for(i=0;i<basic2.size();i++)//在widget依次显示已借的书本
                {
                    IBook *iBook1;
                    int bID1 = basic2[i].bookID;
                    if(library2->queryById(bID1,&iBook1))
                    {
                        TBookBasicInfo basic3;
                        if(iBook1->getBasicInfo(basic3))
                        {
                            QString bName2 = QString::fromStdString(basic3.name);

                            QListWidgetItem *item = new QListWidgetItem;
                            item->setText(bName2);
                            item->setData(Qt::UserRole,basic3.id);//随带保存书本ID便于之后归还
                            ui->listWidget->addItem(item);
                        }
                        else{QMessageBox::information(this,"title","无法获取用户所借的书目");}
                    }
                    else{QMessageBox::information(this,"title","无法获取用户所借的书目");}
                }
            }
            else{QMessageBox::information(this,"title","无法获取用户所借的书目");}


          /*  QListWidgetItem *item = ui->listWidget->currentItem();
            int bID = item->data(Qt::UserRole).toInt();
            bookdata bookdata1;
            bookdata1.setBookID(bID);
            bookdata1.exec();*/

           }
        else{QMessageBox::information(this,"title","获取用户信息错误");}
    }
    else{QMessageBox::information(this,"title","获取用户信息错误");}



}

usermain::~usermain()
{
    delete ui;
}

void usermain::on_pushButton_4_clicked()
{
    ChangePassword ChangePassword1;
    ChangePassword1.exec();
}





void usermain::on_pushButton_2_clicked()
{
    userborrow userborrow1;
    userborrow1.exec();
}

void usermain::on_pushButton_3_clicked()//还书操作
{
    QListWidgetItem *item = ui->listWidget->currentItem();
    int bId = item->data(Qt::UserRole).toInt();

    ILibrary *library3;
    factory1->getLibrary(&library3);
    IBook *iBook1;
    if(library3->queryById(bId,&iBook1)){}//通过预先保存的书本ID号确定所要归还的ibook
    else{QMessageBox::information(this,"title","无法归还书本");return;}


    IAuthManager *iUser;
    factory1->getAuthManager(&iUser);
    IUser *iUser1;
    if(iUser->getCurrentUser(&iUser1))//确定当前要还书的用户
    {
        if(iUser1->returnBook(iBook1)){}//正式还书
        else{QMessageBox::information(this,"title","无法归还书本");}
    }
    else{QMessageBox::information(this,"title","无法归还书本");}


    ui->listWidget->takeItem(ui->listWidget->currentRow());//删除已还的书

}


