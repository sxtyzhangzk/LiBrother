/********************************************************************************
** Form generated from reading UI file 'manager.ui'
**
** Created by: Qt User Interface Compiler version 5.4.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MANAGER_H
#define UI_MANAGER_H

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

class Ui_manager
{
public:
    QLabel *label;
    QLabel *label_2;
    QLabel *label_3;
    QLineEdit *lineEdit;
    QLineEdit *lineEdit_2;
    QPushButton *pushButton;
    QPushButton *pushButton_2;

    void setupUi(QDialog *manager)
    {
        if (manager->objectName().isEmpty())
            manager->setObjectName(QStringLiteral("manager"));
        manager->resize(366, 253);
        label = new QLabel(manager);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(30, 90, 71, 21));
        label_2 = new QLabel(manager);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(30, 140, 71, 16));
        label_3 = new QLabel(manager);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(150, 20, 171, 61));
        lineEdit = new QLineEdit(manager);
        lineEdit->setObjectName(QStringLiteral("lineEdit"));
        lineEdit->setGeometry(QRect(120, 90, 201, 21));
        lineEdit_2 = new QLineEdit(manager);
        lineEdit_2->setObjectName(QStringLiteral("lineEdit_2"));
        lineEdit_2->setGeometry(QRect(120, 130, 201, 21));
        pushButton = new QPushButton(manager);
        pushButton->setObjectName(QStringLiteral("pushButton"));
        pushButton->setGeometry(QRect(50, 190, 115, 32));
        pushButton_2 = new QPushButton(manager);
        pushButton_2->setObjectName(QStringLiteral("pushButton_2"));
        pushButton_2->setGeometry(QRect(210, 190, 115, 32));

        retranslateUi(manager);
        QObject::connect(pushButton_2, SIGNAL(clicked()), manager, SLOT(close()));

        QMetaObject::connectSlotsByName(manager);
    } // setupUi

    void retranslateUi(QDialog *manager)
    {
        manager->setWindowTitle(QApplication::translate("manager", "Dialog", 0));
        label->setText(QApplication::translate("manager", "\347\256\241\347\220\206\345\221\230\350\264\246\345\217\267\357\274\232", 0));
        label_2->setText(QApplication::translate("manager", "\347\256\241\347\220\206\345\221\230\345\257\206\347\240\201\357\274\232", 0));
        label_3->setText(QApplication::translate("manager", "\347\256\241\347\220\206\345\221\230\347\231\273\345\275\225\347\225\214\351\235\242", 0));
        pushButton->setText(QApplication::translate("manager", "\347\231\273\345\275\225", 0));
        pushButton_2->setText(QApplication::translate("manager", "\345\217\226\346\266\210", 0));
    } // retranslateUi

};

namespace Ui {
    class manager: public Ui_manager {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MANAGER_H
