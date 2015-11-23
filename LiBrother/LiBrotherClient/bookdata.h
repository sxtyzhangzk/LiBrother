#ifndef BOOKDATA_H
#define BOOKDATA_H
#include <QString>
#include <QDialog>

namespace Ui {
class bookdata;
}

class bookdata : public QDialog
{
    Q_OBJECT

public:
    explicit bookdata(int nID, QWidget *parent = 0);
    ~bookdata();

protected:
    //QString m_strCombo;
    int m_nBookID;

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::bookdata *ui;
};

#endif // BOOKDATA_H
