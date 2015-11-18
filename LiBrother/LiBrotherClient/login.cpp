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
<<<<<<< HEAD
        QMessageBox::information(this,"Title","用户名或密码输入错误！");
=======
        QMessageBox::information(this,"Title","助教我们用户名是lalala，你们很聪明的，我们不是故意的");
>>>>>>> 11fd821d7be4093ee3f7b6985d1db12642c78d15
    }
}

