/********************************************************************************
** Form generated from reading UI file 'userborrow.ui'
**
** Created by: Qt User Interface Compiler version 5.4.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_USERBORROW_H
#define UI_USERBORROW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_userborrow
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *label_2;
    QHBoxLayout *horizontalLayout;
    QLineEdit *lineEdit;
    QComboBox *comboBox;
    QPushButton *pushButton_4;
    QListWidget *listWidget;
    QPushButton *pushButton_2;
    QPushButton *pushButton;

    void setupUi(QDialog *userborrow)
    {
        if (userborrow->objectName().isEmpty())
            userborrow->setObjectName(QStringLiteral("userborrow"));
        userborrow->resize(391, 426);
        verticalLayout = new QVBoxLayout(userborrow);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        label_2 = new QLabel(userborrow);
        label_2->setObjectName(QStringLiteral("label_2"));

        verticalLayout->addWidget(label_2);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        lineEdit = new QLineEdit(userborrow);
        lineEdit->setObjectName(QStringLiteral("lineEdit"));

        horizontalLayout->addWidget(lineEdit);

        comboBox = new QComboBox(userborrow);
        comboBox->setObjectName(QStringLiteral("comboBox"));

        horizontalLayout->addWidget(comboBox);

        pushButton_4 = new QPushButton(userborrow);
        pushButton_4->setObjectName(QStringLiteral("pushButton_4"));
        pushButton_4->setMinimumSize(QSize(81, 32));

        horizontalLayout->addWidget(pushButton_4);


        verticalLayout->addLayout(horizontalLayout);

        listWidget = new QListWidget(userborrow);
        listWidget->setObjectName(QStringLiteral("listWidget"));

        verticalLayout->addWidget(listWidget);

        pushButton_2 = new QPushButton(userborrow);
        pushButton_2->setObjectName(QStringLiteral("pushButton_2"));

        verticalLayout->addWidget(pushButton_2);

        pushButton = new QPushButton(userborrow);
        pushButton->setObjectName(QStringLiteral("pushButton"));

        verticalLayout->addWidget(pushButton);


        retranslateUi(userborrow);

        QMetaObject::connectSlotsByName(userborrow);
    } // setupUi

    void retranslateUi(QDialog *userborrow)
    {
        userborrow->setWindowTitle(QApplication::translate("userborrow", "Dialog", 0));
        label_2->setText(QApplication::translate("userborrow", "\350\257\267\350\276\223\345\205\245\346\202\250\351\234\200\350\246\201\346\237\245\346\211\276\347\232\204\344\271\246\346\234\254\344\277\241\346\201\257\357\274\232", 0));
        comboBox->clear();
        comboBox->insertItems(0, QStringList()
         << QApplication::translate("userborrow", "\344\271\246\345\220\215", 0)
         << QApplication::translate("userborrow", "ISBN", 0)
        );
        pushButton_4->setText(QApplication::translate("userborrow", "\346\220\234\347\264\242", 0));
        pushButton_2->setText(QApplication::translate("userborrow", "\347\274\226\350\276\221\346\255\244\344\271\246\357\274\210\347\256\241\347\220\206\345\221\230\357\274\211", 0));
        pushButton->setText(QApplication::translate("userborrow", "\350\257\246\346\203\205\344\277\241\346\201\257", 0));
    } // retranslateUi

};

namespace Ui {
    class userborrow: public Ui_userborrow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_USERBORROW_H
