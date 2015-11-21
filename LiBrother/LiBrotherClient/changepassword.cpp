#include "changepassword.h"
#include "ui_changepassword.h"
#include "qstring.h"
#include "login.h"
#include "login.cpp"
#include "client_interfaces.h"
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

void ChangePassword::on_pushButton_clicked()
{
    QString name1;
    QString newpassword;
    QString newpasswordagain;
    name1 = ui->lineEdit->text();
    newpassword = ui->lineEdit_2->text();
    newpasswordagain = ui->lineEdit_3->text();
    IClassFactoryClient *pFactory;
    getClassFactory(&pFactory);
    ILibrary *pLibrary;
    pFactory->getLibrary(&pLibrary);
    IUser *pUser;
    TUserBasicInfo info;
    pUser->getBasicInfo(info);
    std::string name = info.name;
    if(name1.toStdString() != name){
       QMessageBox::information(this,"Title","用户名错误");
    }
    if(newpassword != newpasswordagain){
       QMessageBox::information(this,"Title","请确认新密码");
    }
    if(name1.toStdString() == name && newpassword == newpasswordagain){
        pUser->setPassword(newpassword.toStdString().c_str());
    }
    pUser->Release();
    pLibrary->Release();
    pFactory->Release();
}
