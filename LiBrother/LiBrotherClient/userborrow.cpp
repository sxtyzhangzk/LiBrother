#include "userborrow.h"
#include "ui_userborrow.h"
#include "Qstring"
#include "Qstringlist"
#include "QStringListModel"

userborrow::userborrow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::userborrow)
{

}

userborrow::~userborrow()
{
    delete ui;
}



void userborrow::on_pushButton_clicked()
{
    QStringList strList;
        strList.append("string1");
        strList.append("string2");
        strList.append("string3");
        strList.append("string4");
        strList.append("string5");
        strList.append("string6");
        strList.append("string7");
        strList << "string8";
        strList += "string9";

   QStringListModel *model = new QStringListModel(strList);

   ui->listView->setModel(model);
}
