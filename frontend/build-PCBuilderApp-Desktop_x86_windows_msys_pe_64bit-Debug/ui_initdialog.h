/********************************************************************************
** Form generated from reading UI file 'initdialog.ui'
**
** Created by: Qt User Interface Compiler version 5.15.14
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_INITDIALOG_H
#define UI_INITDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>

QT_BEGIN_NAMESPACE

class Ui_InitDialog
{
public:

    void setupUi(QDialog *InitDialog)
    {
        if (InitDialog->objectName().isEmpty())
            InitDialog->setObjectName(QString::fromUtf8("InitDialog"));
        InitDialog->setEnabled(true);
        InitDialog->resize(787, 568);
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(InitDialog->sizePolicy().hasHeightForWidth());
        InitDialog->setSizePolicy(sizePolicy);

        retranslateUi(InitDialog);

        QMetaObject::connectSlotsByName(InitDialog);
    } // setupUi

    void retranslateUi(QDialog *InitDialog)
    {
        InitDialog->setWindowTitle(QCoreApplication::translate("InitDialog", "Dialog", nullptr));
    } // retranslateUi

};

namespace Ui {
    class InitDialog: public Ui_InitDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_INITDIALOG_H
