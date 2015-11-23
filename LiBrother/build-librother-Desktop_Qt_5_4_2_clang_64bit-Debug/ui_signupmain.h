/********************************************************************************
** Form generated from reading UI file 'signupmain.ui'
**
** Created by: Qt User Interface Compiler version 5.4.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SIGNUPMAIN_H
#define UI_SIGNUPMAIN_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_signupmain
{
public:
    QWidget *verticalLayoutWidget;
    QVBoxLayout *verticalLayout;
    QLabel *label;
    QGridLayout *gridLayout;
    QLabel *label_2;
    QLineEdit *lineEdit;
    QPushButton *pushButton;
    QLabel *label_3;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_4;
    QLineEdit *lineEdit_2;
    QLabel *label_5;
    QHBoxLayout *horizontalLayout;
    QLabel *label_6;
    QLineEdit *lineEdit_3;
    QSpacerItem *verticalSpacer_3;
    QGridLayout *gridLayout_2;
    QLabel *label_9;
    QLabel *label_7;
    QLineEdit *lineEdit_6;
    QLineEdit *lineEdit_4;
    QSpacerItem *verticalSpacer_6;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *pushButton_2;
    QPushButton *pushButton_3;

    void setupUi(QDialog *signupmain)
    {
        if (signupmain->objectName().isEmpty())
            signupmain->setObjectName(QStringLiteral("signupmain"));
        signupmain->resize(497, 413);
        verticalLayoutWidget = new QWidget(signupmain);
        verticalLayoutWidget->setObjectName(QStringLiteral("verticalLayoutWidget"));
        verticalLayoutWidget->setGeometry(QRect(10, 10, 481, 344));
        verticalLayout = new QVBoxLayout(verticalLayoutWidget);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        label = new QLabel(verticalLayoutWidget);
        label->setObjectName(QStringLiteral("label"));

        verticalLayout->addWidget(label);

        gridLayout = new QGridLayout();
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        label_2 = new QLabel(verticalLayoutWidget);
        label_2->setObjectName(QStringLiteral("label_2"));

        gridLayout->addWidget(label_2, 0, 0, 1, 1);

        lineEdit = new QLineEdit(verticalLayoutWidget);
        lineEdit->setObjectName(QStringLiteral("lineEdit"));

        gridLayout->addWidget(lineEdit, 0, 1, 1, 1);

        pushButton = new QPushButton(verticalLayoutWidget);
        pushButton->setObjectName(QStringLiteral("pushButton"));

        gridLayout->addWidget(pushButton, 0, 3, 1, 1);

        label_3 = new QLabel(verticalLayoutWidget);
        label_3->setObjectName(QStringLiteral("label_3"));

        gridLayout->addWidget(label_3, 0, 2, 1, 1);


        verticalLayout->addLayout(gridLayout);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        label_4 = new QLabel(verticalLayoutWidget);
        label_4->setObjectName(QStringLiteral("label_4"));

        horizontalLayout_3->addWidget(label_4);

        lineEdit_2 = new QLineEdit(verticalLayoutWidget);
        lineEdit_2->setObjectName(QStringLiteral("lineEdit_2"));
        lineEdit_2->setEchoMode(QLineEdit::Password);

        horizontalLayout_3->addWidget(lineEdit_2);

        label_5 = new QLabel(verticalLayoutWidget);
        label_5->setObjectName(QStringLiteral("label_5"));

        horizontalLayout_3->addWidget(label_5);


        verticalLayout->addLayout(horizontalLayout_3);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        label_6 = new QLabel(verticalLayoutWidget);
        label_6->setObjectName(QStringLiteral("label_6"));

        horizontalLayout->addWidget(label_6);

        lineEdit_3 = new QLineEdit(verticalLayoutWidget);
        lineEdit_3->setObjectName(QStringLiteral("lineEdit_3"));
        lineEdit_3->setEchoMode(QLineEdit::Password);

        horizontalLayout->addWidget(lineEdit_3);


        verticalLayout->addLayout(horizontalLayout);

        verticalSpacer_3 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer_3);

        gridLayout_2 = new QGridLayout();
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        label_9 = new QLabel(verticalLayoutWidget);
        label_9->setObjectName(QStringLiteral("label_9"));

        gridLayout_2->addWidget(label_9, 1, 0, 1, 1);

        label_7 = new QLabel(verticalLayoutWidget);
        label_7->setObjectName(QStringLiteral("label_7"));

        gridLayout_2->addWidget(label_7, 0, 0, 1, 1);

        lineEdit_6 = new QLineEdit(verticalLayoutWidget);
        lineEdit_6->setObjectName(QStringLiteral("lineEdit_6"));

        gridLayout_2->addWidget(lineEdit_6, 1, 1, 1, 1);

        lineEdit_4 = new QLineEdit(verticalLayoutWidget);
        lineEdit_4->setObjectName(QStringLiteral("lineEdit_4"));

        gridLayout_2->addWidget(lineEdit_4, 0, 1, 1, 1);


        verticalLayout->addLayout(gridLayout_2);

        verticalSpacer_6 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer_6);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        pushButton_2 = new QPushButton(verticalLayoutWidget);
        pushButton_2->setObjectName(QStringLiteral("pushButton_2"));

        horizontalLayout_2->addWidget(pushButton_2);

        pushButton_3 = new QPushButton(verticalLayoutWidget);
        pushButton_3->setObjectName(QStringLiteral("pushButton_3"));

        horizontalLayout_2->addWidget(pushButton_3);


        verticalLayout->addLayout(horizontalLayout_2);


        retranslateUi(signupmain);

        QMetaObject::connectSlotsByName(signupmain);
    } // setupUi

    void retranslateUi(QDialog *signupmain)
    {
        signupmain->setWindowTitle(QString());
        label->setText(QApplication::translate("signupmain", "<html><head/><body><p align=\"center\"><span style=\" font-size:18pt;\">\347\224\250\346\210\267\346\263\250\345\206\214</span></p></body></html>", 0));
        label_2->setText(QApplication::translate("signupmain", "<html><head/><body><p><span style=\" font-size:14pt;\">\347\224\250\346\210\267\345\220\215</span></p></body></html>", 0));
        pushButton->setText(QApplication::translate("signupmain", "\346\243\200\346\265\213", 0));
        label_3->setText(QApplication::translate("signupmain", "<html><head/><body><p>\357\274\210\347\224\250\346\210\267\345\220\215\347\224\2611\357\275\23620\344\275\215\350\213\261\346\226\207\346\210\226\346\225\260\345\255\227\347\273\204\346\210\220\357\274\211</p></body></html>", 0));
        label_4->setText(QApplication::translate("signupmain", "<html><head/><body><p><span style=\" font-size:14pt;\">\345\257\206\347\240\201</span></p></body></html>", 0));
        label_5->setText(QApplication::translate("signupmain", "<html><head/><body><p>\357\274\210\345\257\206\347\240\201\347\224\261\350\207\263\345\260\2216\344\275\215\350\213\261\346\226\207\345\255\227\346\257\215\346\210\226\346\225\260\345\255\227\347\273\204\346\210\220\357\274\211</p></body></html>", 0));
        label_6->setText(QApplication::translate("signupmain", "\345\257\206\347\240\201\347\241\256\350\256\244", 0));
        label_9->setText(QApplication::translate("signupmain", "\351\202\256\347\256\261", 0));
        label_7->setText(QApplication::translate("signupmain", "\347\234\237\345\256\236\345\247\223\345\220\215", 0));
        pushButton_2->setText(QApplication::translate("signupmain", "\347\241\256\350\256\244", 0));
        pushButton_3->setText(QApplication::translate("signupmain", "\345\217\226\346\266\210", 0));
    } // retranslateUi

};

namespace Ui {
    class signupmain: public Ui_signupmain {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SIGNUPMAIN_H
