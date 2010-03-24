/********************************************************************************
** Form generated from reading UI file 'FetchDialog.ui'
**
** Created: Wed Mar 24 14:16:31 2010
**      by: Qt User Interface Compiler version 4.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FETCHDIALOG_H
#define UI_FETCHDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QFormLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>

QT_BEGIN_NAMESPACE

class Ui_FetchDialog
{
public:
    QFormLayout *formLayout;
    QLabel *nodeLabel;
    QLineEdit *nodeIdBox;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *FetchDialog)
    {
        if (FetchDialog->objectName().isEmpty())
            FetchDialog->setObjectName(QString::fromUtf8("FetchDialog"));
        FetchDialog->resize(199, 84);
        formLayout = new QFormLayout(FetchDialog);
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        nodeLabel = new QLabel(FetchDialog);
        nodeLabel->setObjectName(QString::fromUtf8("nodeLabel"));

        formLayout->setWidget(0, QFormLayout::LabelRole, nodeLabel);

        nodeIdBox = new QLineEdit(FetchDialog);
        nodeIdBox->setObjectName(QString::fromUtf8("nodeIdBox"));

        formLayout->setWidget(0, QFormLayout::FieldRole, nodeIdBox);

        buttonBox = new QDialogButtonBox(FetchDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        formLayout->setWidget(1, QFormLayout::SpanningRole, buttonBox);


        retranslateUi(FetchDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), FetchDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), FetchDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(FetchDialog);
    } // setupUi

    void retranslateUi(QDialog *FetchDialog)
    {
        FetchDialog->setWindowTitle(QApplication::translate("FetchDialog", "Fetch Node", 0, QApplication::UnicodeUTF8));
        nodeLabel->setText(QApplication::translate("FetchDialog", "Node ID:", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class FetchDialog: public Ui_FetchDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FETCHDIALOG_H
