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
    QString name;
    QString password;
    name = ui->lineEdit->text();
    password = ui->lineEdit_2->text();
    if(name == "lalala")
    {
        close();
        usermain user1;
        user1.exec();
    }
    else
    {
        QMessageBox::information(this,"Title","用户名或密码输入错误！");
    }
}

