#ifndef NEWBOOKEDIT_H
#define NEWBOOKEDIT_H

#include <QDialog>

namespace Ui {
class newbookedit;
}

class newbookedit : public QDialog
{
    Q_OBJECT

public:
    explicit newbookedit(QWidget *parent = 0);
    ~newbookedit();

private slots:
    void on_pushButton_3_clicked();

private:
    Ui::newbookedit *ui;
};

#endif // NEWBOOKEDIT_H
