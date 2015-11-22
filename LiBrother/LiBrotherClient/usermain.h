#ifndef USERMAIN_H
#define USERMAIN_H

#include <QDialog>
#include <client_interfaces.h>

namespace Ui {
class usermain;
}

class usermain : public QDialog
{
    Q_OBJECT

public:
    explicit usermain(QWidget *parent = 0);
    ~usermain();

private slots:


    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();



    void on_pushButton_5_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_6_clicked();

    void on_pushButton_7_clicked();

private:
    Ui::usermain *ui;
    IClassFactoryClient *factory1;
};

#endif // USERMAIN_H
