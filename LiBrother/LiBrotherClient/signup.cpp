#include "signup.h"
#include "ui_signup.h"
#include "mainwindow.h"
#include "signupmain.h"
signup::signup(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::signup)
{
    ui->setupUi(this);

}

signup::~signup()
{
    delete ui;
}

void signup::on_pushButton_2_clicked()
{
    close();
}

void signup::on_pushButton_clicked()
{
    signupmain sigmain;
    sigmain.exec();
    close();
}
