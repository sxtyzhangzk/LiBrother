#include "login.h"
#include "ui_login.h"
#include "usermain.h"
#include "mainwindow.h"
#include "qmessagebox.h"
#include "QMessageBox"
#include "QString"
#include "client_interfaces.h"

login::login(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::login)
{
    ui->setupUi(this);
}

login::~login()
{
    delete ui;
}


void login::on_pushButton_clicked()
{
    IClassFactoryClient *factory1;
    getClassFactory(&factory1);
    IAuthManager *iUser;
    factory1->getAuthManager(&iUser);

    QString uName = ui->lineEdit->text();//得到界面上的的用户名和密码
    QString uPassword = ui->lineEdit_2->text();
    std::string uName1 = uName.toStdString();
    std::string uPassword1 = uPassword.toStdString();

    bool Lpd = iUser->Login(uName1.c_str(),uPassword1.c_str());//登陆判断
    if(Lpd)
    {
        usermain user1;
        close();
        user1.exec();
    }
    else
    {
        QMessageBox::information(this,"Title","用户名或密码有误");
    }

    factory1->Release();
}

