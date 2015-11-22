#include "managermain.h"
#include "ui_managermain.h"
#include "qstring.h"
#include "userborrow.h"
#include "newbookedit.h"

managermain::managermain(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::managermain)
{
    ui->setupUi(this);
}

managermain::~managermain()
{
    delete ui;
}

void managermain::on_pushButton_clicked()
{
    userborrow userborrow1;//管理员在进行书目编辑的时候是先进行搜索的，ISBN与书名都支持，在选中进行修改的
                            //考虑到管理员自己查找ID号再去查找书本的操作属于重复工作，所以这样设定，与用户公用一个搜索界面
    userborrow1.exec();
}
void managermain::on_pushButton_2_clicked()
{
    newbookedit newbookedit1;//管理员添加新书
    newbookedit1.exec();
}


void managermain::on_pushButton_4_clicked()
{
    close();
}
