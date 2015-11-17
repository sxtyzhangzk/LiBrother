#include "announcement.h"
#include "ui_announcement.h"

announcement::announcement(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::announcement)
{
    ui->setupUi(this);
}

announcement::~announcement()
{
    delete ui;
}
