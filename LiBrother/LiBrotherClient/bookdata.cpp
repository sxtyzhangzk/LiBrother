#include "bookdata.h"
#include "ui_bookdata.h"
#include "QString"

bookdata::bookdata(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::bookdata)
{
    ui->setupUi(this);
    ui->lcdNumber->display(8);
    ui->lcdNumber->setPalette(Qt::red);
    ui->label->setText("大拇指的弟弟");
    QString lala,lala2;
    lala = "作者：";
    lala2 = "冼臧越洋";
    lala = lala + lala2;
    ui->label_4->setText(lala);
    ui->label_5->setText("编号：2713712637");
    ui->label_6->setText("此书已被您借阅");
    ui->textEdit->setText("很早很早以前，有一只小朋友，他的哥哥叫大拇指，于是它就成为了大拇指的弟弟");
}

bookdata::~bookdata()
{
    delete ui;
}

void bookdata::setBookName(const QString& name)
{
    m_strBookName = name;
    ui->label->setText(m_strBookName);
}
