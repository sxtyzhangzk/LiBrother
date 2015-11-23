#include "bookdata.h"
#include "ui_bookdata.h"
#include "QString"
#include "client_interfaces.h"
#include "qmessagebox.h"
#include "QMessageBox"


bookdata::bookdata(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::bookdata)
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
        iBook1->setBasicInfo(basic1);
        iBook1->getDescription(bDescription);


        QString bName1 = QString::fromStdString(basic1.name);
        int bCount = basic1.count;

        QString bAuthor1 = QString::fromStdString(basic1.author);
        bAuthor1 = QString(u8"作者：") + bAuthor1;

        QString bIsbn1 = QString::fromStdString(basic1.isbn);
        bIsbn1 = QString(u8"ISBN号：") + bIsbn1;

        QString bPublisher1 = QString::fromStdString(basic1.publisher);
        bPublisher1 = QString(u8"出版社：") + bPublisher1;

        QString bDescription1 = QString::fromStdString(bDescription);

        ui->label_4->setText(bAuthor1);
        ui->lcdNumber->display(bCount);
        ui->lcdNumber->setPalette(Qt::red);
        ui->label->setText(bName1);
        ui->label_5->setText(bIsbn1);
        ui->label_6->setText(bPublisher1);
        ui->textEdit->setText(bDescription1);

    }
    else{QMessageBox::information(this,"Warning",u8"ID号有误，无法通过ID查询到相关书目");}
    factory1->Release();
    library1->Release();
    iBook1->Release();
}

bookdata::~bookdata()
{
    delete ui;
}

void bookdata::setBookID(const int& bID1)//传入书本信息的ID号
{
    m_strBookID = bID1;
}

void bookdata::on_pushButton_clicked()
{
    IClassFactoryClient *factory1;
    getClassFactory(&factory1);
    ILibrary *library1;
    factory1->getLibrary(&library1);
    IAuthManager *IAManager;
    factory1->getAuthManager(&IAManager);
    IUser *iUser1;
    IBook *iBook1;
    if(IAManager->getCurrentUser(&iUser1))//如果当前没有用户登录，会自动得到false的输出值，会提示需要先登录
    {
        if(library1->queryById(m_strBookID,&iBook1))
        {
            if(iUser1->borrowBook(iBook1)){}//正式借书操作
            else{QMessageBox::information(this,"Warning","借书失败");}
        }
        else{QMessageBox::information(this,"Warning",u8"请先选择所要借阅的书籍");}
    }
    else{QMessageBox::information(this,"Warning",u8"请先登录");}

    factory1->Release();
    library1->Release();
    IAManager->Release();
    iUser1->Release();
    iBook1->Release();
}

void bookdata::on_pushButton_2_clicked()
{
    close();
}
