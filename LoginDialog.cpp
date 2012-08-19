#include "LoginDialog.h"
#include "ui_LoginDialog.h"

#include <QDir>
#include <iostream>

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::LoginDialog),
    settings("cyan.com", "VaultManager")
{
    m_ui->setupUi(this);
    QDir dir = QCoreApplication::applicationDirPath();
    QStringList filters;
    filters.append("*.ini");
    dir.setNameFilters(filters);
    m_ui->shardSelectBox->insertItems(0, dir.entryList());
    QString shardName = settings.value("Shard", "").toString();
    int shardIndex = dir.entryList().indexOf(shardName);
    if(shardIndex == -1)
        shardIndex = 0;
    m_ui->shardSelectBox->setCurrentIndex(shardIndex);
    shardName = m_ui->shardSelectBox->itemText(shardIndex);
    recallShard(shardName);
    connect(m_ui->shardSelectBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(recallShard(QString)));
    connect(this, SIGNAL(accepted()), this, SLOT(sendLogin()));
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

void LoginDialog::recallShard(QString shardName) {
    settings.beginGroup(shardName);
    m_ui->userBox->setText(settings.value("Username", "").toString());
    m_ui->passBox->setText(settings.value("Password", "").toString());
    m_ui->rememberLoginBox->setChecked(settings.value("Remember", false).toBool());
    settings.endGroup();
}

void LoginDialog::sendLogin() {
    QString shardName = m_ui->shardSelectBox->currentText();
    bool remember = m_ui->rememberLoginBox->checkState();
    settings.setValue("Shard", shardName);
    settings.beginGroup(shardName);
    settings.setValue("Remember", remember);
    if(remember) {
        settings.setValue("Username", m_ui->userBox->text());
        settings.setValue("Password", m_ui->passBox->text());
    }else{
        settings.remove("Username");
        settings.remove("Password");
    }
    settings.endGroup();
    emit login(m_ui->userBox->text(), m_ui->passBox->text(), shardName);
}
