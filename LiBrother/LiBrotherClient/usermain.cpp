#include "usermain.h"
#include "ui_usermain.h"
#include "mainwindow.h"
#include "userborrow.h"
#include "bookdata.h"
#include "client_interfaces.h"
#include "QMessageBox"
#include "changepassword.h"
#include "managermain.h"
#include "useredit.h"
#include <set>

usermain::usermain(QWidget *parent) ://开场直接显示所有用户已借的书本
    QDialog(parent),
    ui(new Ui::usermain)
{
    ui->setupUi(this);

    ui->listWidget->clear();
    getClassFactory(&factory1);

    auto_iface<IAuthManager> iUser;//构建iauth为了之后使用getcurrentuser
    factory1->getAuthManager(&iUser);
    auto_iface<IUser> iUser1;
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
                auto_iface<ILibrary> library2;
                factory1->getLibrary(&library2);
                int i;int j;
                std::multiset<int> TBorrow;
                for(i=0;i<basic2.size();i++)//在widget依次显示已借的书本
                {
                            //bool pd = true;
                            if(basic2[i].flag == 1)
                            {
                                auto iter = TBorrow.lower_bound(basic2[i].bookID);
								if(*iter == basic2[i].bookID)
									TBorrow.erase(iter);
                                //pd = false;break;
                            }
                            else{TBorrow.insert(basic2[i].bookID);}
                }
                for(int bookID : TBorrow)
                {
                    auto_iface<IBook> iBook2;

                    if(!library2->queryById(bookID,&iBook2)){QMessageBox::information(this,"Warning",u8"系统错误");return;}
                    TBookBasicInfo basic4;
                    if(iBook2->getBasicInfo(basic4))
                    {
                        QString bName2 = QString::fromStdString(basic4.name);

                        QListWidgetItem *item = new QListWidgetItem;
                        item->setText(bName2);
                        item->setData(Qt::UserRole,basic4.id);//随带保存书本ID便于之后归还
                        ui->listWidget->addItem(item);
                    }
                    else{QMessageBox::information(this,"Warning",u8"系统错误");}
                }
            }
            else{QMessageBox::information(this,"Warning",u8"无法获取用户已借的书目信息");}

        }
        else{QMessageBox::information(this,"Warning",u8"当前用户信息错误");}
    }
    else{QMessageBox::information(this,"Warning",u8"请先登录");}


}

usermain::~usermain()
{
    delete ui;
}





void usermain::on_pushButton_2_clicked()//继续借书模块，直接进入书目搜索。（todo：增加对用户已借书目数量的检测）
{
    userborrow userborrow1;
    userborrow1.exec();
}

void usermain::on_pushButton_3_clicked()//还书操作
{
    QListWidgetItem *item = ui->listWidget->currentItem();
	if(!item)
		return;
    int bId = item->data(Qt::UserRole).toInt();

    auto_iface<ILibrary> library3;
    factory1->getLibrary(&library3);
    auto_iface<IBook> iBook1;
    if(library3->queryById(bId,&iBook1)){}//通过预先保存的书本ID号确定所要归还的ibook
    else{QMessageBox::information(this,"Warning",u8"无法找寻到该书本");return;}


    auto_iface<IAuthManager> iUser;
    factory1->getAuthManager(&iUser);
    auto_iface<IUser> iUser1;
    if(iUser->getCurrentUser(&iUser1))//确定当前要还书的用户
    {
        if(iUser1->returnBook(iBook1)){QMessageBox::information(this,"Welcome",u8"还书成功");}//正式还书
        else{QMessageBox::information(this,"Warning",u8"归还失败");}
    }
    else{QMessageBox::information(this,"Warning",u8"用户信息错误");}


    ui->listWidget->takeItem(ui->listWidget->currentRow());//删除已还的书

}



void usermain::on_pushButton_5_clicked()
{
    ChangePassword changePassword1;//进入修改密码界面
    changePassword1.exec();
}

void usermain::on_pushButton_4_clicked()//进入初等管理员（AuthLevel＝1）操作选择菜单
{
    auto_iface<IClassFactoryClient> factory3;
    getClassFactory(&factory3);
    auto_iface<IAuthManager> iUser;
    factory3->getAuthManager(&iUser);
    if(iUser->getAuthLevel() >= 1)
    {
        managermain managermain1;
        managermain1.exec();
    }
    else{QMessageBox::information(this,"Warning",u8"对不起，您没有权限");}

}

void usermain::on_pushButton_6_clicked()//只有authLevel＝2的高级管理员才能进入用户信息修改界面
{
    auto_iface<IClassFactoryClient> factory3;
    getClassFactory(&factory3);
    auto_iface<IAuthManager> iUser;
    factory3->getAuthManager(&iUser);
    if(iUser->getAuthLevel() >= 2)
    {
        useredit useredit1;
        useredit1.exec();
    }
    else{QMessageBox::information(this,"Warning",u8"对不起，您没有权限");}

}

void usermain::on_pushButton_7_clicked()//刷新操作，对当前用户的所借的书本做一个重新显示
{
    ui->listWidget->clear();
    getClassFactory(&factory1);

    auto_iface<IAuthManager> iUser;//构建iauth为了之后使用getcurrentuser
    factory1->getAuthManager(&iUser);
    auto_iface<IUser> iUser1;
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
                auto_iface<ILibrary> library2;
                factory1->getLibrary(&library2);
                int i;int j;
                std::multiset<int> TBorrow;
                for(i=0;i<basic2.size();i++)//在widget依次显示已借的书本
                {
                            //bool pd = true;
                            if(basic2[i].flag == 1)
                            {
								auto iter = TBorrow.lower_bound(basic2[i].bookID);
								if(*iter == basic2[i].bookID)
									TBorrow.erase(iter);
                                //pd = false;break;
                            }
							else{TBorrow.insert(basic2[i].bookID);}
                }
                for(int bookID : TBorrow)
                {
                    auto_iface<IBook> iBook2;

                    if(!library2->queryById(bookID,&iBook2)){QMessageBox::information(this,"Warning",u8"系统错误");return;}
                    TBookBasicInfo basic4;
                    if(iBook2->getBasicInfo(basic4))
                    {
                        QString bName2 = QString::fromStdString(basic4.name);

                        QListWidgetItem *item = new QListWidgetItem;
                        item->setText(bName2);
                        item->setData(Qt::UserRole,basic4.id);//随带保存书本ID便于之后归还
                        ui->listWidget->addItem(item);
                    }
                    else{QMessageBox::information(this,"Warning",u8"系统错误");}
                }
            }
            else{QMessageBox::information(this,"Warning",u8"无法获取用户已借的书目信息");}

        }
        else{QMessageBox::information(this,"Warning",u8"当前用户信息错误");}
    }
    else{QMessageBox::information(this,"Warning",u8"请先登录");}

}

void usermain::on_pushButton_clicked()
{
    close();
}
