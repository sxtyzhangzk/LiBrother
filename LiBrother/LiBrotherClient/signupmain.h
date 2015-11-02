#ifndef SIGNUPMAIN_H
#define SIGNUPMAIN_H

#include <QDialog>

namespace Ui {
class signupmain;
}

class signupmain : public QDialog
{
    Q_OBJECT

public:
    explicit signupmain(QWidget *parent = 0);
    ~signupmain();

private slots:
    void on_pushButton_3_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::signupmain *ui;
};

#endif // SIGNUPMAIN_H
