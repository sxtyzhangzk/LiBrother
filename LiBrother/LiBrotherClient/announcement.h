#ifndef ANNOUNCEMENT_H
#define ANNOUNCEMENT_H

#include <QDialog>

namespace Ui {
class announcement;
}

class announcement : public QDialog
{
    Q_OBJECT

public:
    explicit announcement(QWidget *parent = 0);
    ~announcement();

private:
    Ui::announcement *ui;
};

#endif // ANNOUNCEMENT_H
