#include "LoginDialog.h"
#include "ui_LoginDialog.h"

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::LoginDialog),
    settings("cyan.com", "VaultManager")
{
    m_ui->setupUi(this);
    connect(this, SIGNAL(accepted()), this, SLOT(sendLogin()));
    m_ui->userBox->setText(settings.value("Username", "").toString());
    m_ui->passBox->setText(settings.value("Password", "").toString());
}

LoginDialog::~LoginDialog()
{
    delete m_ui;
}

void LoginDialog::changeEvent(QEvent *e)
{
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void LoginDialog::sendLogin() {
    if(m_ui->rememberLoginBox->checkState()) {
        settings.setValue("Username", m_ui->userBox->text());
        settings.setValue("Password", m_ui->passBox->text());
    }
    emit login(m_ui->userBox->text(), m_ui->passBox->text());
}
