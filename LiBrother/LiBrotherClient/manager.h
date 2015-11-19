#ifndef MANAGER_H
#define MANAGER_H

#include <QDialog>

namespace Ui {
class manager;
}

class manager : public QDialog
{
    Q_OBJECT

public:
    explicit manager(QWidget *parent = 0);
    ~manager();
private slots:

    void on_pushButton_6_clicked();
    void on_pushButton_clicked();


private:
    Ui::manager *ui;
};

#endif // MANAGER_H
