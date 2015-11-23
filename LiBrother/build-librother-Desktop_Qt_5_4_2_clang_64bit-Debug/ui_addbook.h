/********************************************************************************
** Form generated from reading UI file 'addbook.ui'
**
** Created by: Qt User Interface Compiler version 5.4.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ADDBOOK_H
#define UI_ADDBOOK_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_AddBook
{
public:
    QLabel *label;
    QLabel *label_2;
    QLabel *label_3;
    QLabel *label_4;
    QLabel *label_5;
    QLineEdit *lineEdit;
    QLineEdit *lineEdit_2;
    QLineEdit *lineEdit_3;
    QLineEdit *lineEdit_4;
    QLineEdit *lineEdit_5;
    QPushButton *pushButton;
    QPushButton *pushButton_2;

    void setupUi(QDialog *AddBook)
    {
        if (AddBook->objectName().isEmpty())
            AddBook->setObjectName(QStringLiteral("AddBook"));
        AddBook->resize(400, 300);
        label = new QLabel(AddBook);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(40, 50, 71, 21));
        label_2 = new QLabel(AddBook);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(40, 80, 71, 16));
        label_3 = new QLabel(AddBook);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(30, 110, 81, 21));
        label_4 = new QLabel(AddBook);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setGeometry(QRect(10, 140, 101, 16));
        label_5 = new QLabel(AddBook);
        label_5->setObjectName(QStringLiteral("label_5"));
        label_5->setGeometry(QRect(50, 180, 59, 16));
        lineEdit = new QLineEdit(AddBook);
        lineEdit->setObjectName(QStringLiteral("lineEdit"));
        lineEdit->setGeometry(QRect(130, 50, 141, 21));
        lineEdit_2 = new QLineEdit(AddBook);
        lineEdit_2->setObjectName(QStringLiteral("lineEdit_2"));
        lineEdit_2->setGeometry(QRect(130, 80, 141, 21));
        lineEdit_3 = new QLineEdit(AddBook);
        lineEdit_3->setObjectName(QStringLiteral("lineEdit_3"));
        lineEdit_3->setGeometry(QRect(130, 110, 141, 21));
        lineEdit_4 = new QLineEdit(AddBook);
        lineEdit_4->setObjectName(QStringLiteral("lineEdit_4"));
        lineEdit_4->setGeometry(QRect(130, 140, 141, 21));
        lineEdit_5 = new QLineEdit(AddBook);
        lineEdit_5->setObjectName(QStringLiteral("lineEdit_5"));
        lineEdit_5->setGeometry(QRect(130, 170, 141, 21));
        pushButton = new QPushButton(AddBook);
        pushButton->setObjectName(QStringLiteral("pushButton"));
        pushButton->setGeometry(QRect(80, 230, 115, 32));
        pushButton_2 = new QPushButton(AddBook);
        pushButton_2->setObjectName(QStringLiteral("pushButton_2"));
        pushButton_2->setGeometry(QRect(230, 230, 115, 32));

        retranslateUi(AddBook);

        QMetaObject::connectSlotsByName(AddBook);
    } // setupUi

    void retranslateUi(QDialog *AddBook)
    {
        AddBook->setWindowTitle(QApplication::translate("AddBook", "Dialog", 0));
        label->setText(QApplication::translate("AddBook", "bookname:", 0));
        label_2->setText(QApplication::translate("AddBook", "bookISBN:", 0));
        label_3->setText(QApplication::translate("AddBook", "bookauthor:", 0));
        label_4->setText(QApplication::translate("AddBook", "bookpublisher:", 0));
        label_5->setText(QApplication::translate("AddBook", "bookID:", 0));
        pushButton->setText(QApplication::translate("AddBook", "\347\241\256\345\256\232", 0));
        pushButton_2->setText(QApplication::translate("AddBook", "\345\217\226\346\266\210", 0));
    } // retranslateUi

};

namespace Ui {
    class AddBook: public Ui_AddBook {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ADDBOOK_H
