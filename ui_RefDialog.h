/********************************************************************************
** Form generated from reading UI file 'RefDialog.ui'
**
** Created: Wed Mar 24 14:16:31 2010
**      by: Qt User Interface Compiler version 4.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_REFDIALOG_H
#define UI_REFDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QFormLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>

QT_BEGIN_NAMESPACE

class Ui_RefDialog
{
public:
    QFormLayout *formLayout;
    QLabel *label;
    QLabel *label_2;
    QLineEdit *childBox;
    QCheckBox *useOwnerBox;
    QLineEdit *parentBox;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *RefDialog)
    {
        if (RefDialog->objectName().isEmpty())
            RefDialog->setObjectName(QString::fromUtf8("RefDialog"));
        RefDialog->resize(298, 157);
        formLayout = new QFormLayout(RefDialog);
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        label = new QLabel(RefDialog);
        label->setObjectName(QString::fromUtf8("label"));

        formLayout->setWidget(0, QFormLayout::LabelRole, label);

        label_2 = new QLabel(RefDialog);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        formLayout->setWidget(2, QFormLayout::LabelRole, label_2);

        childBox = new QLineEdit(RefDialog);
        childBox->setObjectName(QString::fromUtf8("childBox"));

        formLayout->setWidget(2, QFormLayout::FieldRole, childBox);

        useOwnerBox = new QCheckBox(RefDialog);
        useOwnerBox->setObjectName(QString::fromUtf8("useOwnerBox"));

        formLayout->setWidget(4, QFormLayout::FieldRole, useOwnerBox);

        parentBox = new QLineEdit(RefDialog);
        parentBox->setObjectName(QString::fromUtf8("parentBox"));

        formLayout->setWidget(0, QFormLayout::FieldRole, parentBox);

        buttonBox = new QDialogButtonBox(RefDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        formLayout->setWidget(5, QFormLayout::FieldRole, buttonBox);


        retranslateUi(RefDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), RefDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), RefDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(RefDialog);
    } // setupUi

    void retranslateUi(QDialog *RefDialog)
    {
        RefDialog->setWindowTitle(QApplication::translate("RefDialog", "Add Ref", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("RefDialog", "Parent:", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("RefDialog", "Child:", 0, QApplication::UnicodeUTF8));
        useOwnerBox->setText(QApplication::translate("RefDialog", "Set Owner", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class RefDialog: public Ui_RefDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_REFDIALOG_H
