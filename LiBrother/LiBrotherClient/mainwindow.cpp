#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "login.h"
#include "signup.h"
#include "announcement.h"
#include "manager.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    login login1;
    login1.exec();
}

void MainWindow::on_pushButton_2_clicked()
{
    signup signup1;
    signup1.exec();
}

void MainWindow::on_pushButton_4_clicked()
{
    //QString book1;
    //book1 = ui->lineEdit->text();
    //ui->tableView->

}
void MainWindow::on_pushButton_5_clicked()
{
    announcement announcement1;
    announcement1.exec();
}

void MainWindow::on_pushButton_6_clicked()
{
    manager manager1;
    manager1.exec();
}

