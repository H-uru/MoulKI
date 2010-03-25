#include "CreateDialog.h"
#include "ui_CreateDialog.h"

CreateDialog::CreateDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CreateDialog)
{
    ui->setupUi(this);
    ui->nodeEditor->setNode(&node);
    connect(this, SIGNAL(accepted()), this, SLOT(sendCreate()));
    connect(ui->nodeEditor, SIGNAL(isDirty(bool)), ui->buttonBox, SLOT(setEnabled(bool)));
}

CreateDialog::~CreateDialog()
{
    delete ui;
}

void CreateDialog::changeEvent(QEvent *e)
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

void CreateDialog::setParent(hsUint32 idx) {
    parentIdx = idx;
}

void CreateDialog::sendCreate() {
    emit createSig(node, parentIdx);
}
