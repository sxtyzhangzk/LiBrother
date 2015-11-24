#include "signupmain.h"
#include "ui_signupmain.h"
#include "client_interfaces.h"
#include "QMessageBox"

signupmain::signupmain(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::signupmain)
{
    ui->setupUi(this);
}

signupmain::~signupmain()
{
    delete ui;
}

void signupmain::on_pushButton_3_clicked()
{
    close();
}

void signupmain::on_pushButton_2_clicked()
{
    auto_iface<IClassFactoryClient> factory1;
    getClassFactory(&factory1);

    auto_iface<IAuthManager> iAUser;
    factory1->getAuthManager(&iAUser);

    auto_iface<IUser> iUser;
    factory1->createEmptyUser(&iUser);

    TUserBasicInfo basic1;
    QString uName = ui->lineEdit->text();//读入用户所有添加的信息，并register
    QString uPassword = ui->lineEdit_2->text();
    QString u2Password = ui->lineEdit_3->text();
    QString uTName = ui->lineEdit_4->text();
    QString uMail = ui->lineEdit_6->text();
    std::string uPass = uPassword.toStdString();
    std::string u2Pass = u2Password.toStdString();



    basic1.name = uName.toStdString();
    basic1.email = uMail.toStdString();

    if(uPass.length()>=6)
    {
    if(uPass ==u2Pass)
    {
        if(!iUser->setBasicInfo(basic1)){close();QMessageBox::information(this,"Warning",u8"系统错误");return;}
        if(!iUser->setPassword(uPass.c_str())){close();QMessageBox::information(this,"Warning",u8"系统错误");return;}

        if(!iAUser->Register(iUser)){close();QMessageBox::information(this,"Warning",u8"系统错误");return;}
        QMessageBox::information(this,"Welcome",u8"注册成功");
        close();
    }
    else{QMessageBox::information(this,"Warning",u8"两次密码输入不一致");}
    }
    else{QMessageBox::information(this,"Warning",u8"用户密码必须不短于6位");}
}


