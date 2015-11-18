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

<<<<<<< HEAD
    void on_pushButton_4_clicked();

    //void on_listView_doubleClicked(const QModelIndex &index);
=======
>>>>>>> 11fd821d7be4093ee3f7b6985d1db12642c78d15

    void on_pushButton_clicked();


private:
    Ui::userborrow *ui;
    QStringList strList;

};

#endif // USERBORROW_H
