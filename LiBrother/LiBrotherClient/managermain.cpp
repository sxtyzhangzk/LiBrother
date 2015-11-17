#include "managermain.h"
#include "ui_managermain.h"

managermain::managermain(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::managermain)
{
    ui->setupUi(this);
}

managermain::~managermain()
{
    delete ui;
}
