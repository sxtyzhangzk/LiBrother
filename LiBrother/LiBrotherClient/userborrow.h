#ifndef USERBORROW_H
#define USERBORROW_H
#include <QStringListModel>

#include <QDialog>

namespace Ui {
class userborrow;
}

class userborrow : public QDialog
{
    Q_OBJECT

public:
    explicit userborrow(QWidget *parent = 0);
    ~userborrow();

private slots:


    void on_pushButton_clicked();


    void on_pushButton_4_clicked();

private:
    Ui::userborrow *ui;
    QStringList strList;

};

#endif // USERBORROW_H
