#include "useredit.h"
#include "ui_useredit.h"
#include "client_interfaces.h"
#include "QMessageBox"

useredit::useredit(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::useredit)
{
    ui->setupUi(this);
}

useredit::~useredit()
{
    delete ui;
}

void useredit::on_pushButton_3_clicked()
{
    IClassFactoryClient *factory1;
    getClassFactory(&factory1);
    IUserManager *IUManager;
    factory1->getUserManager(&IUManager);
    IUser *iUser;

    uName = ui->label_5->text();
    std::string uName1 = uName.toStdString();
    if(IUManager->getUserByName(uName1.c_str(),&iUser))
    {
        TUserBasicInfo basic1;
        if(iUser->getBasicInfo(basic1))
        {
            QString uMail =QString::fromStdString(basic1.email);
            int uLevel = iUser->getAuthLevel();

            ui->lineEdit_3->setText(uMail);
            ui->lineEdit_4->setText(QString::number(uLevel));
        }
        else{QMessageBox::information(this,"Warning","用户获取信息错误");}
    }
    else{QMessageBox::information(this,"Warning","查找用户错误");}
    factory1->Release();
    IUManager->Release();
    iUser->Release();

}

void useredit::on_pushButton_clicked()
{
    IClassFactoryClient *factory1;
    getClassFactory(&factory1);
    IUserManager *IUManager;
    factory1->getUserManager(&IUManager);
    IUser *iUser;

    std::string uName1 = uName.toStdString();
    if(IUManager->getUserByName(uName1.c_str(),&iUser))
    {
        TUserBasicInfo basic1;
        QString uMail1 = ui->lineEdit_3->text();
        QString uLevel1 = ui->lineEdit_4->text();
        int uNLevel = uLevel1.toInt();

        basic1.email = uMail1.toStdString();
        basic1.name = uName.toStdString();
        iUser->setAuthLevel(uNLevel);
        iUser->setBasicInfo(basic1);

        QString nPassword = ui->lineEdit->text();
        QString n2Password = ui->lineEdit_2->text();
        std::string nPassword1 = nPassword.toStdString();
        std::string n2Password1 = n2Password.toStdString();
        if(nPassword1 != "")
        {
            if(nPassword1 == n2Password1)
            {
                iUser->setPassword(nPassword1.c_str());
            }
            else{QMessageBox::information(this,"Warning","两次密码输入不同");}
        }
    }
    else{QMessageBox::information(this,"Warning","查找用户错误");}


    factory1->Release();
    IUManager->Release();
    iUser->Release();
}

void useredit::on_pushButton_2_clicked()
{
    close();
}
