/********************************************************************************
** Form generated from reading UI file 'LoginDialog.ui'
**
** Created: Wed Mar 24 14:16:31 2010
**      by: Qt User Interface Compiler version 4.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LOGINDIALOG_H
#define UI_LOGINDIALOG_H

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

class Ui_LoginDialog
{
public:
    QFormLayout *formLayout;
    QLineEdit *userBox;
    QLineEdit *passBox;
    QDialogButtonBox *buttonBox;
    QLabel *label;
    QLabel *Pasword;
    QCheckBox *rememberLoginBox;

    void setupUi(QDialog *LoginDialog)
    {
        if (LoginDialog->objectName().isEmpty())
            LoginDialog->setObjectName(QString::fromUtf8("LoginDialog"));
        LoginDialog->resize(276, 151);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(LoginDialog->sizePolicy().hasHeightForWidth());
        LoginDialog->setSizePolicy(sizePolicy);
        formLayout = new QFormLayout(LoginDialog);
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        userBox = new QLineEdit(LoginDialog);
        userBox->setObjectName(QString::fromUtf8("userBox"));

        formLayout->setWidget(0, QFormLayout::FieldRole, userBox);

        passBox = new QLineEdit(LoginDialog);
        passBox->setObjectName(QString::fromUtf8("passBox"));
        passBox->setEchoMode(QLineEdit::Password);

        formLayout->setWidget(2, QFormLayout::FieldRole, passBox);

        buttonBox = new QDialogButtonBox(LoginDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        formLayout->setWidget(4, QFormLayout::FieldRole, buttonBox);

        label = new QLabel(LoginDialog);
        label->setObjectName(QString::fromUtf8("label"));

        formLayout->setWidget(0, QFormLayout::LabelRole, label);

        Pasword = new QLabel(LoginDialog);
        Pasword->setObjectName(QString::fromUtf8("Pasword"));

        formLayout->setWidget(2, QFormLayout::LabelRole, Pasword);

        rememberLoginBox = new QCheckBox(LoginDialog);
        rememberLoginBox->setObjectName(QString::fromUtf8("rememberLoginBox"));

        formLayout->setWidget(3, QFormLayout::FieldRole, rememberLoginBox);


        retranslateUi(LoginDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), LoginDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), LoginDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(LoginDialog);
    } // setupUi

    void retranslateUi(QDialog *LoginDialog)
    {
        LoginDialog->setWindowTitle(QApplication::translate("LoginDialog", "Login", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("LoginDialog", "Username:", 0, QApplication::UnicodeUTF8));
        Pasword->setText(QApplication::translate("LoginDialog", "Password:", 0, QApplication::UnicodeUTF8));
        rememberLoginBox->setText(QApplication::translate("LoginDialog", "Remember login", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class LoginDialog: public Ui_LoginDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LOGINDIALOG_H
