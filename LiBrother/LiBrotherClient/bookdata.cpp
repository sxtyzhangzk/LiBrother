#include "bookdata.h"
#include "ui_bookdata.h"
#include "QString"

bookdata::bookdata(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::bookdata)
{
    ui->setupUi(this);
    ui->lcdNumber->display(23);
    ui->lcdNumber->setPalette(Qt::red);
    ui->label->setText("大拇指的弟弟");
    QString lala,lala2;
    lala = "作者:";
    lala2 = "hqwxhqjxjshqlx";
    lala = lala + lala2;
    ui->label_4->setText(lala);
    ui->label_5->setText("编号：2713712637");

    ui->textEdit->setText("cdhjcbwhjqbcjkhbqhcbdbqchbwhcbwjhqbcjhwbchjbdjcbwqcbhjwdbcjbqwjcbhwqbcjdbhqcbdjbqj");
}

bookdata::~bookdata()
{
    delete ui;
}
