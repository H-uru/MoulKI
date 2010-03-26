#include "CreateNodeDialog.h"
#include "ui_CreateNodeDialog.h"

#include <QPushButton>

CreateNodeDialog::CreateNodeDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CreateNodeDialog)
{
    ui->setupUi(this);
    ui->nodeEditor->setNode(&node);
    connect(this, SIGNAL(accepted()), this, SLOT(sendCreate()));
    connect(ui->nodeEditor, SIGNAL(isDirty(bool)), ui->buttonBox->button(QDialogButtonBox::Ok), SLOT(setEnabled(bool)));
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
}

CreateNodeDialog::~CreateNodeDialog()
{
    delete ui;
}

void CreateNodeDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void CreateNodeDialog::setParent(hsUint32 idx) {
    parentIdx = idx;
}

void CreateNodeDialog::sendCreate() {
    emit createSig(node, parentIdx);
}
