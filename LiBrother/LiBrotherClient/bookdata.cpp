#include "bookdata.h"
#include "ui_bookdata.h"
#include "QString"
#include "client_interfaces.h"
#include "qmessagebox.h"
#include "QMessageBox"


bookdata::bookdata(int nID, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::bookdata),
    m_nBookID(nID)
{
    ui->setupUi(this);

    auto_iface<IClassFactoryClient> factory1;
    getClassFactory(&factory1);
    auto_iface<ILibrary> library1;
    factory1->getLibrary(&library1);

    std::string bDescription;
    TBookBasicInfo basic1;
    auto_iface<IBook>iBook1;


    bool fPd = library1->queryById(nID,&iBook1);

    if(fPd)
    {
        if(!iBook1->getBasicInfo(basic1)){close();QMessageBox::information(this,"Warning",u8"系统错误");return;}

        if(!iBook1->getDescription(bDescription)){close();QMessageBox::information(this,"Warning",u8"系统错误");return;}

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

}

bookdata::~bookdata()
{
    delete ui;
}

void bookdata::on_pushButton_clicked()
{
    auto_iface<IClassFactoryClient> factory1;
    getClassFactory(&factory1);
    auto_iface<ILibrary> library1;
    factory1->getLibrary(&library1);
    auto_iface<IAuthManager> IAManager;
    factory1->getAuthManager(&IAManager);
    auto_iface<IUser> iUser1;
    auto_iface<IBook> iBook1;
    if(IAManager->getCurrentUser(&iUser1))//如果当前没有用户登录，会自动得到false的输出值，会提示需要先登录
    {
        if(library1->queryById(m_nBookID,&iBook1))
        {
            if(iUser1->borrowBook(iBook1)){}//正式借书操作
            else{QMessageBox::information(this,"Warning","借书失败");}
        }
        else{QMessageBox::information(this,"Warning",u8"请先选择所要借阅的书籍");}
    }
    else{QMessageBox::information(this,"Warning",u8"请先登录");}

}

void bookdata::on_pushButton_2_clicked()
{
    close();
}
