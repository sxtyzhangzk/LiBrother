#include "bookdataedit.h"
#include "ui_bookdataedit.h"
#include "client_interfaces.h"
#include "QMessageBox"

bookdataedit::bookdataedit(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::bookdataedit)
{
    ui->setupUi(this);

    IClassFactoryClient *factory1;
    getClassFactory(&factory1);
    ILibrary *library1;
    factory1->getLibrary(&library1);


    std::string bDescription;
    TBookBasicInfo basic1;
    IBook *iBook1;

    bool fPd = library1->queryById(m_strBookID,&iBook1);
    if(fPd)
    {
        iBook1->setBasicInfo(basic1);//找到书本的信息并显示在窗口中，便于用户进行更改操作
        iBook1->getDescription(bDescription);


        QString bName1 = QString::fromStdString(basic1.name);

        int bCount = basic1.count;

        QString bAuthor1 = QString::fromStdString(basic1.author);

        QString bIsbn1 = QString::fromStdString(basic1.isbn);

        QString bPublisher1 = QString::fromStdString(basic1.publisher);

        QString bDescription1 = QString::fromStdString(bDescription);

        ui->lineEdit->setText(bName1);
        ui->lineEdit_2->setText(QString::number(bCount));
        ui->lineEdit_3->setText(bAuthor1);
        ui->lineEdit_4->setText(bPublisher1);
        ui->lineEdit_5->setText(bIsbn1);
        ui->textEdit->setText(bDescription1);

    }
    else{QMessageBox::information(this,"Warning",u8"无法获取该书的基本信息");}

    factory1->Release();
    iBook1->Release();
    library1->Release();
}

bookdataedit::~bookdataedit()
{
    delete ui;
}

void bookdataedit::setBookID(const int& bID1)//传入书本信息的ID号
{
    m_strBookID = bID1;
}

void bookdataedit::on_pushButton_clicked()
{
    IClassFactoryClient *factory2;
    getClassFactory(&factory2);
    ILibrary *library2;
    factory2->getLibrary(&library2);

    std::string bDescription;
    TBookBasicInfo basic2;
    IBook *iBook2;

    bool fPd = library2->queryById(m_strBookID,&iBook2);

    if(fPd)
    {

        QString nbName1 = ui->lineEdit->text();//依次读入书本的新信息

        QString nCount = ui->lineEdit_2->text();
        int nbCount = nCount.toInt();

        QString nbAuthor1 = ui->lineEdit_3->text();

        QString nbIsbn1 = ui->lineEdit_5->text();

        QString nbPublisher1 = ui->lineEdit_4->text();

        QString nbDescription1 = ui->textEdit->toPlainText();

        basic2.count = nbCount;
        basic2.name = nbName1.toStdString();
        basic2.author = nbAuthor1.toStdString();
        basic2.isbn = nbIsbn1.toStdString();
        basic2.publisher = nbPublisher1.toStdString();
        std::string nbDescription01 = nbDescription1.toStdString();

        if(iBook2->setBasicInfo(basic2)){}else{QMessageBox::information(this,"Warning",u8"基础信息录入失败，请检查输入格式");}
        if(iBook2->setDescription(nbDescription01.c_str())){}else{QMessageBox::information(this,"Warning",u8"简洁录入失败");}

    }
    else{QMessageBox::information(this,"Warning",u8"无法对当前书本信息进行修改");}

    factory2->Release();
    iBook2->Release();
    library2->Release();

}

void bookdataedit::on_pushButton_2_clicked()
{
    close();
}
