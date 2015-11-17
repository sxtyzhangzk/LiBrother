#ifndef MANAGERMAIN_H
#define MANAGERMAIN_H

#include <QDialog>

namespace Ui {
class managermain;
}

class managermain : public QDialog
{
    Q_OBJECT

public:
    explicit managermain(QWidget *parent = 0);
    ~managermain();

private:
    Ui::managermain *ui;
};

#endif // MANAGERMAIN_H
