#ifndef BOOKDATA_H
#define BOOKDATA_H

#include <QDialog>

namespace Ui {
class bookdata;
}

class bookdata : public QDialog
{
    Q_OBJECT

public:
    explicit bookdata(QWidget *parent = 0);
    ~bookdata();

private:
    Ui::bookdata *ui;
};

#endif // BOOKDATA_H
