#include "userborrow.h"
#include "ui_userborrow.h"

userborrow::userborrow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::userborrow)
{
    ui->setupUi(this);
}

userborrow::~userborrow()
{
    delete ui;
}
