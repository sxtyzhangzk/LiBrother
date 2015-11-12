#include "bookdata.h"
#include "ui_bookdata.h"

bookdata::bookdata(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::bookdata)
{
    ui->setupUi(this);
}

bookdata::~bookdata()
{
    delete ui;
}
