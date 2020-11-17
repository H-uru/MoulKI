#include "FetchDialog.h"
#include <QIntValidator>
#include "ui_FetchDialog.h"

FetchDialog::FetchDialog(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::FetchDialog)
{
    m_ui->setupUi(this);
    QIntValidator* val = new QIntValidator(this);
    val->setBottom(0);
    m_ui->nodeIdBox->setValidator(val);
    connect(this, SIGNAL(accepted()), this, SLOT(fetchSlot()));
}

FetchDialog::~FetchDialog()
{
    delete m_ui;
}

void FetchDialog::changeEvent(QEvent *e)
{
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void FetchDialog::fetchSlot() {
    bool ok;
    uint32_t idx = m_ui->nodeIdBox->text().toUInt(&ok, 10);
    if(!ok) {
        qWarning("Invalid Node Id");
        return;
    }
    emit fetchNode(idx);
}
