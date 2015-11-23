/********************************************************************************
** Form generated from reading UI file 'bookdata.ui'
**
** Created by: Qt User Interface Compiler version 5.4.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_BOOKDATA_H
#define UI_BOOKDATA_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLCDNumber>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_bookdata
{
public:
    QLabel *label;
    QLabel *label_2;
    QLCDNumber *lcdNumber;
    QLabel *label_3;
    QTextEdit *textEdit;
    QWidget *verticalLayoutWidget;
    QVBoxLayout *verticalLayout;
    QPushButton *pushButton;
    QPushButton *pushButton_2;
    QWidget *layoutWidget;
    QVBoxLayout *verticalLayout_2;
    QLabel *label_4;
    QLabel *label_6;
    QLabel *label_5;

    void setupUi(QDialog *bookdata)
    {
        if (bookdata->objectName().isEmpty())
            bookdata->setObjectName(QStringLiteral("bookdata"));
        bookdata->resize(400, 362);
        label = new QLabel(bookdata);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(10, 10, 241, 61));
        QFont font;
        font.setFamily(QStringLiteral("Libian SC"));
        font.setPointSize(24);
        font.setBold(false);
        font.setWeight(50);
        label->setFont(font);
        label_2 = new QLabel(bookdata);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(10, 160, 81, 31));
        QFont font1;
        font1.setFamily(QStringLiteral("Libian SC"));
        font1.setPointSize(18);
        label_2->setFont(font1);
        lcdNumber = new QLCDNumber(bookdata);
        lcdNumber->setObjectName(QStringLiteral("lcdNumber"));
        lcdNumber->setGeometry(QRect(320, 10, 64, 61));
        label_3 = new QLabel(bookdata);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(260, 10, 71, 61));
        label_3->setFont(font1);
        textEdit = new QTextEdit(bookdata);
        textEdit->setObjectName(QStringLiteral("textEdit"));
        textEdit->setGeometry(QRect(10, 190, 381, 161));
        verticalLayoutWidget = new QWidget(bookdata);
        verticalLayoutWidget->setObjectName(QStringLiteral("verticalLayoutWidget"));
        verticalLayoutWidget->setGeometry(QRect(290, 90, 94, 66));
        verticalLayout = new QVBoxLayout(verticalLayoutWidget);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        pushButton = new QPushButton(verticalLayoutWidget);
        pushButton->setObjectName(QStringLiteral("pushButton"));
        pushButton->setAcceptDrops(false);
        pushButton->setAutoFillBackground(false);

        verticalLayout->addWidget(pushButton);

        pushButton_2 = new QPushButton(verticalLayoutWidget);
        pushButton_2->setObjectName(QStringLiteral("pushButton_2"));

        verticalLayout->addWidget(pushButton_2);

        layoutWidget = new QWidget(bookdata);
        layoutWidget->setObjectName(QStringLiteral("layoutWidget"));
        layoutWidget->setGeometry(QRect(10, 80, 261, 71));
        verticalLayout_2 = new QVBoxLayout(layoutWidget);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        label_4 = new QLabel(layoutWidget);
        label_4->setObjectName(QStringLiteral("label_4"));

        verticalLayout_2->addWidget(label_4);

        label_6 = new QLabel(layoutWidget);
        label_6->setObjectName(QStringLiteral("label_6"));

        verticalLayout_2->addWidget(label_6);

        label_5 = new QLabel(layoutWidget);
        label_5->setObjectName(QStringLiteral("label_5"));

        verticalLayout_2->addWidget(label_5);


        retranslateUi(bookdata);

        QMetaObject::connectSlotsByName(bookdata);
    } // setupUi

    void retranslateUi(QDialog *bookdata)
    {
        bookdata->setWindowTitle(QApplication::translate("bookdata", "Dialog", 0));
        label->setText(QApplication::translate("bookdata", "\344\271\246\345\220\215", 0));
        label_2->setText(QApplication::translate("bookdata", "\347\256\200\344\273\213\357\274\232", 0));
        label_3->setText(QApplication::translate("bookdata", "\345\272\223\345\255\230\357\274\232", 0));
        pushButton->setText(QApplication::translate("bookdata", "\346\210\221\350\246\201\345\200\237\351\230\205", 0));
        pushButton_2->setText(QApplication::translate("bookdata", "\351\200\200\345\233\236", 0));
        label_4->setText(QApplication::translate("bookdata", "\344\275\234\350\200\205\357\274\232", 0));
        label_6->setText(QApplication::translate("bookdata", "\345\207\272\347\211\210\347\244\276\357\274\232", 0));
        label_5->setText(QApplication::translate("bookdata", "ISBN\345\217\267\357\274\232", 0));
    } // retranslateUi

};

namespace Ui {
    class bookdata: public Ui_bookdata {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_BOOKDATA_H
