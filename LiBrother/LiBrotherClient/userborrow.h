#ifndef USERBORROW_H
#define USERBORROW_H

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

private:
    Ui::userborrow *ui;
};

#endif // USERBORROW_H
