/********************************************************************************
** Form generated from reading UI file 'managermain.ui'
**
** Created by: Qt User Interface Compiler version 5.4.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MANAGERMAIN_H
#define UI_MANAGERMAIN_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_managermain
{
public:
    QLabel *label;
    QWidget *layoutWidget;
    QVBoxLayout *verticalLayout;
    QPushButton *pushButton;
    QPushButton *pushButton_2;
    QPushButton *pushButton_4;

    void setupUi(QDialog *managermain)
    {
        if (managermain->objectName().isEmpty())
            managermain->setObjectName(QStringLiteral("managermain"));
        managermain->resize(125, 157);
        label = new QLabel(managermain);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(30, 50, 71, 16));
        layoutWidget = new QWidget(managermain);
        layoutWidget->setObjectName(QStringLiteral("layoutWidget"));
        layoutWidget->setGeometry(QRect(10, 10, 107, 134));
        verticalLayout = new QVBoxLayout(layoutWidget);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        pushButton = new QPushButton(layoutWidget);
        pushButton->setObjectName(QStringLiteral("pushButton"));

        verticalLayout->addWidget(pushButton);

        pushButton_2 = new QPushButton(layoutWidget);
        pushButton_2->setObjectName(QStringLiteral("pushButton_2"));

        verticalLayout->addWidget(pushButton_2);

        pushButton_4 = new QPushButton(layoutWidget);
        pushButton_4->setObjectName(QStringLiteral("pushButton_4"));

        verticalLayout->addWidget(pushButton_4);


        retranslateUi(managermain);
        QObject::connect(pushButton_4, SIGNAL(clicked()), managermain, SLOT(close()));

        QMetaObject::connectSlotsByName(managermain);
    } // setupUi

    void retranslateUi(QDialog *managermain)
    {
        managermain->setWindowTitle(QApplication::translate("managermain", "Dialog", 0));
        label->setText(QString());
        pushButton->setText(QApplication::translate("managermain", "\347\274\226\350\276\221\345\233\276\344\271\246", 0));
        pushButton_2->setText(QApplication::translate("managermain", "\346\267\273\345\212\240\346\226\260\345\233\276\344\271\246", 0));
        pushButton_4->setText(QApplication::translate("managermain", "\351\200\200\345\207\272", 0));
    } // retranslateUi

};

namespace Ui {
    class managermain: public Ui_managermain {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MANAGERMAIN_H
