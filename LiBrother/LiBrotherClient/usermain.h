#ifndef USERMAIN_H
#define USERMAIN_H

#include <QDialog>

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

    void on_pushButton_4_clicked();



private:
    Ui::usermain *ui;
};

#endif // USERMAIN_H
