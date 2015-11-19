#include "manager.h"
#include "ui_manager.h"
#include "qmessagebox.h"
#include "managermain.h"

manager::manager(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::manager)
{
    ui->setupUi(this);
}

manager::~manager()
{
    delete ui;
}
void manager::on_pushButton_6_clicked()
{
   close();
}

void manager::on_pushButton_clicked()
{
    QString name1;
    QString password1;
    name1 = ui->lineEdit->text();
    password1 = ui->lineEdit_2->text();
    if(name1 == "zjh" && password1 == "hehehe")
    {
        close();
        managermain managermain1;
        managermain1.exec();
    }
    else
    {
        QMessageBox::information(this,"Title","账号或密码输入错误！");
    }
}

