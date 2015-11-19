#include "addbook.h"
#include "ui_addbook.h"
#include "client_interfaces.h"

AddBook::AddBook(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddBook)
{
    ui->setupUi(this);
}

AddBook::~AddBook()
{
    delete ui;
}

void AddBook::on_pushButton_clicked()
{
    QString bookname;
    QString bookISBN;
    QString bookauther;
    QString bookpublisher;
    int bookID;
    bookname = ui->lineEdit->text();
    bookISBN = ui->lineEdit_2->text();
    bookauther = ui->lineEdit_3->text();
    bookpublisher = ui->lineEdit_4->text();
    bookID = ui->lineEdit_5->text().toInt();
    IClassFactoryClient *pFactory;
    getClassFactory(&pFactory);
    ILibrary *pLibrary;
    pFactory->getLibrary(&pLibrary);
    IBook *pbook;
    TBookBasicInfo info;
    info.name = bookname.toStdString();
    info.id = bookID;
    info.author = bookauther.toStdString();
    info.isbn = bookISBN.toStdString();
    info.publisher = bookpublisher.toStdString();
    info.count++;
    pbook->setBasicInfo(info);
    pLibrary->insertBook(pbook);
    pbook->Release();
    pLibrary->Release();
    pFactory->Release();
    close();
}
