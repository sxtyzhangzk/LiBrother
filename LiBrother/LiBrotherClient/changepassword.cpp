#include "changepassword.h"
#include "ui_changepassword.h"
#include "client_interfaces.h"
#include "QMessageBox"

ChangePassword::ChangePassword(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChangePassword)
{
    ui->setupUi(this);
}

ChangePassword::~ChangePassword()
{
    delete ui;
}

void ChangePassword::on_pushButton_2_clicked()
{
    close();
}

void ChangePassword::on_pushButton_clicked()
{
    IClassFactoryClient *factory1;
    getClassFactory(&factory1);

    IAuthManager *iUser;
    factory1->getAuthManager(&iUser);

    QString passOld= ui->lineEdit->text();//依次读入新老密码
    QString passNew = ui->lineEdit_2->text();
    QString passNew2 = ui->lineEdit_3->text();
    std::string passOld1 = passOld.toStdString();
    std::string passNew1 = passNew.toStdString();
    std::string passNew12 = passNew2.toStdString();//重复输入老密码
    if(passNew1 == passNew12)//两次密码输入比对
    {
        if(iUser->changePassword(passOld1.c_str(),passNew1.c_str()))
        {
            close();//成功更改密码之后自动退出
        }
        else{QMessageBox::information(this,"Warning","修改密码失败");}
    }
    else{QMessageBox::information(this,"Warning","两次密码输入不一致");}

    factory1->Release();
    iUser->Release();
}
