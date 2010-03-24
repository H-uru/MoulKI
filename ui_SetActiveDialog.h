/********************************************************************************
** Form generated from reading UI file 'SetActiveDialog.ui'
**
** Created: Wed Mar 24 14:16:31 2010
**      by: Qt User Interface Compiler version 4.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SETACTIVEDIALOG_H
#define UI_SETACTIVEDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QHeaderView>
#include <QtGui/QListWidget>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_SetActiveDialog
{
public:
    QVBoxLayout *verticalLayout;
    QListWidget *playerList;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *SetActiveDialog)
    {
        if (SetActiveDialog->objectName().isEmpty())
            SetActiveDialog->setObjectName(QString::fromUtf8("SetActiveDialog"));
        SetActiveDialog->resize(400, 300);
        verticalLayout = new QVBoxLayout(SetActiveDialog);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        playerList = new QListWidget(SetActiveDialog);
        playerList->setObjectName(QString::fromUtf8("playerList"));
        playerList->setEditTriggers(QAbstractItemView::NoEditTriggers);
        playerList->setSelectionBehavior(QAbstractItemView::SelectRows);

        verticalLayout->addWidget(playerList);

        buttonBox = new QDialogButtonBox(SetActiveDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout->addWidget(buttonBox);


        retranslateUi(SetActiveDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), SetActiveDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), SetActiveDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(SetActiveDialog);
    } // setupUi

    void retranslateUi(QDialog *SetActiveDialog)
    {
        SetActiveDialog->setWindowTitle(QApplication::translate("SetActiveDialog", "Set Player", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class SetActiveDialog: public Ui_SetActiveDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SETACTIVEDIALOG_H
