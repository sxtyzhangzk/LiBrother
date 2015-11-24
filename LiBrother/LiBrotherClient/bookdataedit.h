#ifndef BOOKDATAEDIT_H
#define BOOKDATAEDIT_H

#include <QDialog>

namespace Ui {
class bookdataedit;
}

class bookdataedit : public QDialog
{
    Q_OBJECT

public:
    explicit bookdataedit(int nID, QWidget *parent = 0);
    ~bookdataedit();
    //void setBookID(const int & bID1);
protected:
    int m_strBookID;

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::bookdataedit *ui;
};

#endif // BOOKDATAEDIT_H
