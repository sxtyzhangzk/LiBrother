/********************************************************************************
** Form generated from reading UI file 'usermain.ui'
**
** Created by: Qt User Interface Compiler version 5.4.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_USERMAIN_H
#define UI_USERMAIN_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_usermain
{
public:
    QWidget *layoutWidget;
    QVBoxLayout *verticalLayout_5;
    QVBoxLayout *verticalLayout_4;
    QHBoxLayout *horizontalLayout;
    QLabel *label_2;
    QLabel *label;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_4;
    QLabel *label_5;
    QVBoxLayout *verticalLayout_3;
    QLabel *label_6;
    QHBoxLayout *horizontalLayout_2;
    QListWidget *listWidget;
    QVBoxLayout *verticalLayout_2;
    QVBoxLayout *verticalLayout;
    QPushButton *pushButton_2;
    QPushButton *pushButton_3;
    QPushButton *pushButton_4;
    QPushButton *pushButton_6;
    QSpacerItem *verticalSpacer;
    QPushButton *pushButton_7;
    QPushButton *pushButton_5;
    QPushButton *pushButton;

    void setupUi(QDialog *usermain)
    {
        if (usermain->objectName().isEmpty())
            usermain->setObjectName(QStringLiteral("usermain"));
        usermain->resize(542, 437);
        layoutWidget = new QWidget(usermain);
        layoutWidget->setObjectName(QStringLiteral("layoutWidget"));
        layoutWidget->setGeometry(QRect(10, 10, 521, 411));
        verticalLayout_5 = new QVBoxLayout(layoutWidget);
        verticalLayout_5->setObjectName(QStringLiteral("verticalLayout_5"));
        verticalLayout_5->setContentsMargins(0, 0, 0, 0);
        verticalLayout_4 = new QVBoxLayout();
        verticalLayout_4->setObjectName(QStringLiteral("verticalLayout_4"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        label_2 = new QLabel(layoutWidget);
        label_2->setObjectName(QStringLiteral("label_2"));

        horizontalLayout->addWidget(label_2);

        label = new QLabel(layoutWidget);
        label->setObjectName(QStringLiteral("label"));

        horizontalLayout->addWidget(label);


        verticalLayout_4->addLayout(horizontalLayout);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        label_4 = new QLabel(layoutWidget);
        label_4->setObjectName(QStringLiteral("label_4"));

        horizontalLayout_3->addWidget(label_4);

        label_5 = new QLabel(layoutWidget);
        label_5->setObjectName(QStringLiteral("label_5"));

        horizontalLayout_3->addWidget(label_5);


        verticalLayout_4->addLayout(horizontalLayout_3);


        verticalLayout_5->addLayout(verticalLayout_4);

        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        label_6 = new QLabel(layoutWidget);
        label_6->setObjectName(QStringLiteral("label_6"));

        verticalLayout_3->addWidget(label_6);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        listWidget = new QListWidget(layoutWidget);
        listWidget->setObjectName(QStringLiteral("listWidget"));

        horizontalLayout_2->addWidget(listWidget);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        pushButton_2 = new QPushButton(layoutWidget);
        pushButton_2->setObjectName(QStringLiteral("pushButton_2"));

        verticalLayout->addWidget(pushButton_2);

        pushButton_3 = new QPushButton(layoutWidget);
        pushButton_3->setObjectName(QStringLiteral("pushButton_3"));

        verticalLayout->addWidget(pushButton_3);

        pushButton_4 = new QPushButton(layoutWidget);
        pushButton_4->setObjectName(QStringLiteral("pushButton_4"));

        verticalLayout->addWidget(pushButton_4);

        pushButton_6 = new QPushButton(layoutWidget);
        pushButton_6->setObjectName(QStringLiteral("pushButton_6"));

        verticalLayout->addWidget(pushButton_6);


        verticalLayout_2->addLayout(verticalLayout);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_2->addItem(verticalSpacer);

        pushButton_7 = new QPushButton(layoutWidget);
        pushButton_7->setObjectName(QStringLiteral("pushButton_7"));

        verticalLayout_2->addWidget(pushButton_7);

        pushButton_5 = new QPushButton(layoutWidget);
        pushButton_5->setObjectName(QStringLiteral("pushButton_5"));

        verticalLayout_2->addWidget(pushButton_5);

        pushButton = new QPushButton(layoutWidget);
        pushButton->setObjectName(QStringLiteral("pushButton"));

        verticalLayout_2->addWidget(pushButton);


        horizontalLayout_2->addLayout(verticalLayout_2);


        verticalLayout_3->addLayout(horizontalLayout_2);


        verticalLayout_5->addLayout(verticalLayout_3);


        retranslateUi(usermain);

        QMetaObject::connectSlotsByName(usermain);
    } // setupUi

    void retranslateUi(QDialog *usermain)
    {
        usermain->setWindowTitle(QApplication::translate("usermain", "Dialog", 0));
        label_2->setText(QApplication::translate("usermain", "<html><head/><body><p><span style=\" font-size:18pt;\">XXXXXXXX</span></p></body></html>", 0));
        label->setText(QApplication::translate("usermain", "<html><head/><body><p><span style=\" font-size:18pt;\">\346\202\250\345\245\275\357\274\201</span></p></body></html>", 0));
        label_4->setText(QApplication::translate("usermain", "\347\224\250\346\210\267ID\357\274\232", 0));
        label_5->setText(QApplication::translate("usermain", "123456789", 0));
        label_6->setText(QApplication::translate("usermain", "\346\202\250\345\267\262\345\200\237\351\230\205\344\272\206\344\273\245\344\270\213\344\271\246\346\234\254\357\274\232", 0));
        pushButton_2->setText(QApplication::translate("usermain", "\347\273\247\347\273\255\345\200\237\344\271\246", 0));
        pushButton_3->setText(QApplication::translate("usermain", "\345\275\222\350\277\230\346\255\244\344\271\246", 0));
        pushButton_4->setText(QApplication::translate("usermain", "\347\256\241\347\220\206\345\233\276\344\271\246\357\274\210\345\210\235\347\272\247\347\256\241\347\220\206\345\221\230\357\274\211", 0));
        pushButton_6->setText(QApplication::translate("usermain", "\347\256\241\347\220\206\347\224\250\346\210\267\357\274\210\351\253\230\347\272\247\347\256\241\347\220\206\345\221\230\357\274\211", 0));
        pushButton_7->setText(QApplication::translate("usermain", "\345\210\267\346\226\260", 0));
        pushButton_5->setText(QApplication::translate("usermain", "\344\277\256\346\224\271\345\257\206\347\240\201", 0));
        pushButton->setText(QApplication::translate("usermain", "\347\231\273\345\207\272 Log out", 0));
    } // retranslateUi

};

namespace Ui {
    class usermain: public Ui_usermain {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_USERMAIN_H
