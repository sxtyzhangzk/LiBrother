/********************************************************************************
** Form generated from reading UI file 'announcement.ui'
**
** Created by: Qt User Interface Compiler version 5.4.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ANNOUNCEMENT_H
#define UI_ANNOUNCEMENT_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTextEdit>

QT_BEGIN_NAMESPACE

class Ui_announcement
{
public:
    QTextEdit *textEdit;
    QTextEdit *textEdit_2;
    QPushButton *pushButton5;

    void setupUi(QDialog *announcement)
    {
        if (announcement->objectName().isEmpty())
            announcement->setObjectName(QStringLiteral("announcement"));
        announcement->setEnabled(true);
        announcement->resize(400, 300);
        textEdit = new QTextEdit(announcement);
        textEdit->setObjectName(QStringLiteral("textEdit"));
        textEdit->setGeometry(QRect(40, 60, 321, 201));
        textEdit_2 = new QTextEdit(announcement);
        textEdit_2->setObjectName(QStringLiteral("textEdit_2"));
        textEdit_2->setGeometry(QRect(150, 0, 91, 51));
        pushButton5 = new QPushButton(announcement);
        pushButton5->setObjectName(QStringLiteral("pushButton5"));
        pushButton5->setGeometry(QRect(150, 270, 115, 32));

        retranslateUi(announcement);
        QObject::connect(pushButton5, SIGNAL(pressed()), announcement, SLOT(close()));

        QMetaObject::connectSlotsByName(announcement);
    } // setupUi

    void retranslateUi(QDialog *announcement)
    {
        announcement->setWindowTitle(QApplication::translate("announcement", "Dialog", 0));
        textEdit->setHtml(QApplication::translate("announcement", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'.Helvetica Neue DeskInterface'; font-size:13pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:24pt;\">\345\233\275\345\272\206\350\212\202\346\234\237\351\227\264\345\233\276\344\271\246\351\246\206\351\227\255\351\246\206\357\274\201</span></p></body></html>", 0));
        textEdit_2->setHtml(QApplication::translate("announcement", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'.Helvetica Neue DeskInterface'; font-size:13pt; font-weight:400; font-style:normal;\">\n"
"<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:36pt; font-weight:600; font-style:italic;\">\345\205\254\345\221\212</span></p></body></html>", 0));
        pushButton5->setText(QApplication::translate("announcement", "\346\210\221\347\237\245\351\201\223\344\272\206", 0));
    } // retranslateUi

};

namespace Ui {
    class announcement: public Ui_announcement {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ANNOUNCEMENT_H
