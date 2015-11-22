#ifndef USEREDIT_H
#define USEREDIT_H

#include <QDialog>

namespace Ui {
class useredit;
}

class useredit : public QDialog
{
    Q_OBJECT

public:
    explicit useredit(QWidget *parent = 0);
    ~useredit();

private slots:
    void on_pushButton_3_clicked();

    void on_pushButton_clicked();
    void on_pushButton_2_clicked();

protected:
    QString uName;

private:
    Ui::useredit *ui;
};

#endif // USEREDIT_H
