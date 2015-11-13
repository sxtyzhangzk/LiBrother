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

    void on_pushButton_4_clicked();

    void on_listView_doubleClicked(const QModelIndex &index);

    void on_pushButton_clicked();

private:
    Ui::userborrow *ui;
    QStringList strList;
    QString name[100];
    int i = 1;
    QStringListModel *model;
    //QStringListModel *model;
};

#endif // USERBORROW_H
