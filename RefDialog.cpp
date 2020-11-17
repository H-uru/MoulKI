#include "RefDialog.h"
#include <QIntValidator>
#include "ui_RefDialog.h"

RefDialog::RefDialog(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::RefDialog)
{
    m_ui->setupUi(this);
    QIntValidator* val = new QIntValidator(this);
    val->setBottom(0);
    m_ui->parentBox->setValidator(val);
    val = new QIntValidator(this);
    val->setBottom(0);
    m_ui->childBox->setValidator(val);
    connect(this, SIGNAL(accepted()), this, SLOT(sendRef()));
}

RefDialog::~RefDialog()
{
    delete m_ui;
}

void RefDialog::changeEvent(QEvent *e)
{
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void RefDialog::sendRef() {
    bool ok;
    uint32_t parent = m_ui->parentBox->text().toUInt(&ok, 10);
    if(!ok) {
        qWarning("Invalid Parent");
        return;
    }
    uint32_t child = m_ui->childBox->text().toUInt(&ok, 10);
    if(!ok) {
        qWarning("Invalid Child");
        return;
    }
    if(m_ui->useOwnerBox->checkState())
        emit addRef(parent, child, owner);
    else
        emit addRef(parent, child, 0);
}

void RefDialog::setupRefBox(uint32_t owner, uint32_t parent) {
    this->owner = owner;
    if(parent) {
        m_ui->parentBox->setText(QString("%1").arg(parent, 0, 10));
        m_ui->parentBox->setEnabled(false);
    }
}
