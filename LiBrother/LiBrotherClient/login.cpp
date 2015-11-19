#include "login.h"
#include "ui_login.h"
#include "usermain.h"
#include "mainwindow.h"
#include "qmessagebox.h"
#include "QMessageBox"
#include "QString"

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
        QMessageBox::information(this,"Title","助教我们用户名是lalala，你们很聪明的，我们不是故意的");
    }
}

