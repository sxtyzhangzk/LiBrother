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

    QString passOld= ui->lineEdit->text();
    QString passNew = ui->lineEdit_2->text();
    QString passNew2 = ui->lineEdit_3->text();
    std::string passOld1 = passOld.toStdString();
    std::string passNew1 = passNew.toStdString();
    std::string passNew12 = passNew2.toStdString();
    if(passNew1 == passNew12)
    {
        if(iUser->changePassword(passOld1.c_str(),passNew1.c_str()))
        {
            close();
        }
    else{QMessageBox::information(this,"Warning","修改密码失败");}
    }
    else{QMessageBox::information(this,"Warning","两次密码输入不一致");}

    factory1->Release();
    iUser->Release();
}
