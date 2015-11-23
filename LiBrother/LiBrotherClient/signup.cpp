#include "signup.h"
#include "ui_signup.h"
#include "mainwindow.h"
#include "signupmain.h"
#include "client_interfaces.h"
#include "QMessageBox"
signup::signup(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::signup)
{
    ui->setupUi(this);
    auto_iface<IClassFactoryClient> factory1;
    getClassFactory(&factory1);
    auto_iface<IAuthManager> IAManager;
    factory1->getAuthManager(&IAManager);
    std::string LicenseStr;
    if(IAManager->getLicense(LicenseStr))
    {
        QString LicenseStr1 = QString::fromStdString(LicenseStr);
        ui->textEdit->setText(LicenseStr1);
    }
    else{QMessageBox::information(this,"Warning",u8"获取申明失败");}


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
