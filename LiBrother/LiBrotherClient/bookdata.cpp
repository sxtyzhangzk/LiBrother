#include "bookdata.h"
#include "ui_bookdata.h"
#include "QString"
#include "client_interfaces.h"


bookdata::bookdata(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::bookdata)
{
    ui->setupUi(this);

    IClassFactoryClient * factory1;
    getClassFactory(&factory1);
    ILibrary * library1;
    factory1->getLibrary(&library1);
    IFvector vBooks;

    std::string m_strBookName1 = m_strBookName.toStdString();
    library1->queryByName(m_strBookName1.c_str(),vBooks,INT_MAX,1);

    TBookBasicInfo basic1;
    ((IBook*)vBooks[0])->getBasicInfo(basic1);

    QString bName1 = QString::fromStdString(basic1.name);
    int bCount = basic1.count;

    QString bAuthor1 = QString::fromStdString(basic1.author);
    bAuthor1 = QString("作者：") + bAuthor1;

    QString bIsbn1 = QString::fromStdString(basic1.isbn);
    bIsbn1 = QString("ISBN号：") + bIsbn1;

    QString bPublisher1 = QString::fromStdString(basic1.publisher);
    bPublisher1 = QString("出版社：") + bPublisher1;

    std::string bDescription;
    ((IBook*)vBooks[0])->getDescription(bDescription);
    QString bDescription1 = QString::fromStdString(bDescription);

    ui->label_4->setText(bAuthor1);
    ui->lcdNumber->display(bCount);
    ui->lcdNumber->setPalette(Qt::red);
    ui->label->setText(bName1);
    ui->label_5->setText(bIsbn1);
    ui->label_6->setText(bPublisher1);
    ui->textEdit->setText(bDescription1);



    //ui->lcdNumber->display(8);
    //ui->lcdNumber->setPalette(Qt::red);
    //ui->label->setText("大拇指的弟弟");
    //QString lala,lala2;lala = "作者：";lala2 = "冼臧越洋";lala = lala + lala2;
    //ui->label_4->setText(lala);
    //ui->label_5->setText("编号：2713712637");
    //ui->label_6->setText("此书已被您借阅");
    //ui->textEdit->setText("很早很早以前，有一只小朋友，他的哥哥叫大拇指，于是它就成为了大拇指的弟弟");
}

bookdata::~bookdata()
{
    delete ui;
}

void bookdata::setBookName(const QString& name)
{
    m_strBookName = name;
}
