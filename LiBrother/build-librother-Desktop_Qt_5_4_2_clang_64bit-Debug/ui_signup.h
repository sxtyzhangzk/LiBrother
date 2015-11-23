/********************************************************************************
** Form generated from reading UI file 'signup.ui'
**
** Created by: Qt User Interface Compiler version 5.4.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SIGNUP_H
#define UI_SIGNUP_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTextEdit>

QT_BEGIN_NAMESPACE

class Ui_signup
{
public:
    QTextEdit *textEdit;
    QPushButton *pushButton;
    QPushButton *pushButton_2;

    void setupUi(QDialog *signup)
    {
        if (signup->objectName().isEmpty())
            signup->setObjectName(QStringLiteral("signup"));
        signup->setEnabled(true);
        signup->resize(353, 277);
        textEdit = new QTextEdit(signup);
        textEdit->setObjectName(QStringLiteral("textEdit"));
        textEdit->setGeometry(QRect(10, 20, 331, 171));
        pushButton = new QPushButton(signup);
        pushButton->setObjectName(QStringLiteral("pushButton"));
        pushButton->setGeometry(QRect(120, 200, 115, 32));
        pushButton_2 = new QPushButton(signup);
        pushButton_2->setObjectName(QStringLiteral("pushButton_2"));
        pushButton_2->setGeometry(QRect(120, 230, 115, 32));

        retranslateUi(signup);

        QMetaObject::connectSlotsByName(signup);
    } // setupUi

    void retranslateUi(QDialog *signup)
    {
        signup->setWindowTitle(QString());
        textEdit->setHtml(QApplication::translate("signup", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'.Helvetica Neue DeskInterface'; font-size:13pt; font-weight:400; font-style:normal;\">\n"
"<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:24pt;\">\347\224\263\346\230\216</span></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">1.\347\263\273\347\273\237\346\213\231\350\256\241\357\274\214\350\257\267\344\275\240\345\226\204\345\276\205\344\273\226</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">2.\347\224\250\346\210\267\350\207\252\350\247\211\347\273\264\346\212\244\345\233\276"
                        "\344\271\246\351\246\206\345\200\237\344\271\246\345\205\254\347\272\246</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">3.\347\224\250\346\210\267\347\273\264\346\212\244\345\245\275\345\200\237\351\230\205\345\233\276\344\271\246</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">4.\350\257\267\347\224\250\346\210\267\346\214\211\346\227\266\345\275\222\350\277\230\345\233\276\344\271\246</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">5.\345\246\202\350\277\235\345\217\215\350\247\204\345\256\232\347\232\204\347\224\250\346\210\267\357\274\214\344\274\232\345\217\227\345\210\260\347\275\232\346\254\276\346\210\226\346\263\250\351\224\200\350\264\246\346\210\267\347\232\204\345\244\204\347\275\232</p></body></html>", 0));
        pushButton->setText(QApplication::translate("signup", "\345\220\214\346\204\217", 0));
        pushButton_2->setText(QApplication::translate("signup", "\346\210\221\346\213\222\347\273\235", 0));
    } // retranslateUi

};

namespace Ui {
    class signup: public Ui_signup {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SIGNUP_H
