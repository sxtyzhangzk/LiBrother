#ifndef ADDBOOK_H
#define ADDBOOK_H

#include <QDialog>

namespace Ui {
class AddBook;
}

class AddBook : public QDialog
{
    Q_OBJECT
private slots:
      void on_pushButton_clicked();
      void on_pushButton_2_clicked();
public:
    explicit AddBook(QWidget *parent = 0);
    ~AddBook();

private:
    Ui::AddBook *ui;
};

#endif // ADDBOOK_H
