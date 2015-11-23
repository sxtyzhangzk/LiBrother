/********************************************************************************
** Form generated from reading UI file 'newbookedit.ui'
**
** Created by: Qt User Interface Compiler version 5.4.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_NEWBOOKEDIT_H
#define UI_NEWBOOKEDIT_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_newbookedit
{
public:
    QWidget *layoutWidget;
    QVBoxLayout *verticalLayout_6;
    QVBoxLayout *verticalLayout_7;
    QVBoxLayout *verticalLayout_8;
    QHBoxLayout *horizontalLayout_7;
    QLabel *label_7;
    QLineEdit *lineEdit_6;
    QSpacerItem *verticalSpacer_3;
    QHBoxLayout *horizontalLayout_8;
    QLabel *label_8;
    QLineEdit *lineEdit_7;
    QLabel *label;
    QVBoxLayout *verticalLayout_9;
    QHBoxLayout *horizontalLayout_9;
    QLabel *label_9;
    QLineEdit *lineEdit_8;
    QHBoxLayout *horizontalLayout_10;
    QLabel *label_10;
    QLineEdit *lineEdit_9;
    QHBoxLayout *horizontalLayout_11;
    QLabel *label_11;
    QLineEdit *lineEdit_10;
    QSpacerItem *verticalSpacer_4;
    QVBoxLayout *verticalLayout_10;
    QLabel *label_12;
    QTextEdit *textEdit_2;
    QHBoxLayout *horizontalLayout_12;
    QPushButton *pushButton_3;
    QPushButton *pushButton_4;

    void setupUi(QDialog *newbookedit)
    {
        if (newbookedit->objectName().isEmpty())
            newbookedit->setObjectName(QStringLiteral("newbookedit"));
        newbookedit->resize(382, 509);
        layoutWidget = new QWidget(newbookedit);
        layoutWidget->setObjectName(QStringLiteral("layoutWidget"));
        layoutWidget->setGeometry(QRect(10, 10, 364, 491));
        verticalLayout_6 = new QVBoxLayout(layoutWidget);
        verticalLayout_6->setObjectName(QStringLiteral("verticalLayout_6"));
        verticalLayout_6->setContentsMargins(0, 0, 0, 0);
        verticalLayout_7 = new QVBoxLayout();
        verticalLayout_7->setObjectName(QStringLiteral("verticalLayout_7"));
        verticalLayout_8 = new QVBoxLayout();
        verticalLayout_8->setObjectName(QStringLiteral("verticalLayout_8"));
        horizontalLayout_7 = new QHBoxLayout();
        horizontalLayout_7->setObjectName(QStringLiteral("horizontalLayout_7"));
        label_7 = new QLabel(layoutWidget);
        label_7->setObjectName(QStringLiteral("label_7"));

        horizontalLayout_7->addWidget(label_7);

        lineEdit_6 = new QLineEdit(layoutWidget);
        lineEdit_6->setObjectName(QStringLiteral("lineEdit_6"));

        horizontalLayout_7->addWidget(lineEdit_6);


        verticalLayout_8->addLayout(horizontalLayout_7);

        verticalSpacer_3 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_8->addItem(verticalSpacer_3);

        horizontalLayout_8 = new QHBoxLayout();
        horizontalLayout_8->setObjectName(QStringLiteral("horizontalLayout_8"));
        label_8 = new QLabel(layoutWidget);
        label_8->setObjectName(QStringLiteral("label_8"));

        horizontalLayout_8->addWidget(label_8);

        lineEdit_7 = new QLineEdit(layoutWidget);
        lineEdit_7->setObjectName(QStringLiteral("lineEdit_7"));

        horizontalLayout_8->addWidget(lineEdit_7);

        label = new QLabel(layoutWidget);
        label->setObjectName(QStringLiteral("label"));

        horizontalLayout_8->addWidget(label);


        verticalLayout_8->addLayout(horizontalLayout_8);

        verticalLayout_9 = new QVBoxLayout();
        verticalLayout_9->setObjectName(QStringLiteral("verticalLayout_9"));
        horizontalLayout_9 = new QHBoxLayout();
        horizontalLayout_9->setObjectName(QStringLiteral("horizontalLayout_9"));
        label_9 = new QLabel(layoutWidget);
        label_9->setObjectName(QStringLiteral("label_9"));

        horizontalLayout_9->addWidget(label_9);

        lineEdit_8 = new QLineEdit(layoutWidget);
        lineEdit_8->setObjectName(QStringLiteral("lineEdit_8"));

        horizontalLayout_9->addWidget(lineEdit_8);


        verticalLayout_9->addLayout(horizontalLayout_9);

        horizontalLayout_10 = new QHBoxLayout();
        horizontalLayout_10->setObjectName(QStringLiteral("horizontalLayout_10"));
        label_10 = new QLabel(layoutWidget);
        label_10->setObjectName(QStringLiteral("label_10"));

        horizontalLayout_10->addWidget(label_10);

        lineEdit_9 = new QLineEdit(layoutWidget);
        lineEdit_9->setObjectName(QStringLiteral("lineEdit_9"));

        horizontalLayout_10->addWidget(lineEdit_9);


        verticalLayout_9->addLayout(horizontalLayout_10);

        horizontalLayout_11 = new QHBoxLayout();
        horizontalLayout_11->setObjectName(QStringLiteral("horizontalLayout_11"));
        label_11 = new QLabel(layoutWidget);
        label_11->setObjectName(QStringLiteral("label_11"));

        horizontalLayout_11->addWidget(label_11);

        lineEdit_10 = new QLineEdit(layoutWidget);
        lineEdit_10->setObjectName(QStringLiteral("lineEdit_10"));

        horizontalLayout_11->addWidget(lineEdit_10);


        verticalLayout_9->addLayout(horizontalLayout_11);


        verticalLayout_8->addLayout(verticalLayout_9);


        verticalLayout_7->addLayout(verticalLayout_8);

        verticalSpacer_4 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_7->addItem(verticalSpacer_4);

        verticalLayout_10 = new QVBoxLayout();
        verticalLayout_10->setObjectName(QStringLiteral("verticalLayout_10"));
        label_12 = new QLabel(layoutWidget);
        label_12->setObjectName(QStringLiteral("label_12"));

        verticalLayout_10->addWidget(label_12);

        textEdit_2 = new QTextEdit(layoutWidget);
        textEdit_2->setObjectName(QStringLiteral("textEdit_2"));

        verticalLayout_10->addWidget(textEdit_2);


        verticalLayout_7->addLayout(verticalLayout_10);


        verticalLayout_6->addLayout(verticalLayout_7);

        horizontalLayout_12 = new QHBoxLayout();
        horizontalLayout_12->setObjectName(QStringLiteral("horizontalLayout_12"));
        pushButton_3 = new QPushButton(layoutWidget);
        pushButton_3->setObjectName(QStringLiteral("pushButton_3"));

        horizontalLayout_12->addWidget(pushButton_3);

        pushButton_4 = new QPushButton(layoutWidget);
        pushButton_4->setObjectName(QStringLiteral("pushButton_4"));

        horizontalLayout_12->addWidget(pushButton_4);


        verticalLayout_6->addLayout(horizontalLayout_12);


        retranslateUi(newbookedit);

        QMetaObject::connectSlotsByName(newbookedit);
    } // setupUi

    void retranslateUi(QDialog *newbookedit)
    {
        newbookedit->setWindowTitle(QApplication::translate("newbookedit", "Dialog", 0));
        label_7->setText(QApplication::translate("newbookedit", "\344\271\246\345\220\215\357\274\232", 0));
        lineEdit_6->setText(QString());
        label_8->setText(QApplication::translate("newbookedit", "\345\272\223\345\255\230\357\274\232", 0));
        label->setText(QApplication::translate("newbookedit", "\357\274\210\350\276\223\345\205\245\346\225\260\345\255\227\350\241\250\347\244\272\346\234\254\346\225\260\357\274\211", 0));
        label_9->setText(QApplication::translate("newbookedit", "\344\275\234\350\200\205\357\274\232", 0));
        label_10->setText(QApplication::translate("newbookedit", "\345\207\272\347\211\210\347\244\276\357\274\232", 0));
        label_11->setText(QApplication::translate("newbookedit", "ISBN\345\217\267\357\274\232", 0));
        label_12->setText(QApplication::translate("newbookedit", "\347\256\200\344\273\213\357\274\232", 0));
        pushButton_3->setText(QApplication::translate("newbookedit", "\346\226\260\344\271\246\345\275\225\345\205\245", 0));
        pushButton_4->setText(QApplication::translate("newbookedit", "\345\217\226\346\266\210", 0));
    } // retranslateUi

};

namespace Ui {
    class newbookedit: public Ui_newbookedit {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_NEWBOOKEDIT_H
