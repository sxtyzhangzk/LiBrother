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
    void on_label_3_linkActivated(const QString &link);

private:
    Ui::usermain *ui;
};

#endif // USERMAIN_H
