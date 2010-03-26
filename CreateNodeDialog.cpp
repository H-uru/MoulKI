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

void CreateNodeDialog::setActFind() {
    setWindowTitle("Find Node");
    act = 1;
}

void CreateNodeDialog::setParent(hsUint32 idx) {
    parentIdx = idx;
    act = 0;
}

void CreateNodeDialog::sendCreate() {
    if(act)
        emit findSig(node);
    else
        emit createSig(node, parentIdx);
}
