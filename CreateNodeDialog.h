#ifndef CREATENODEDIALOG_H
#define CREATENODEDIALOG_H

#include <QDialog>

#include "qtVault.h"

namespace Ui {
    class CreateNodeDialog;
}

class CreateNodeDialog : public QDialog {
    Q_OBJECT
public:
    CreateNodeDialog(QWidget *parent = 0);
    ~CreateNodeDialog();

    void setParent(hsUint32 idx);

public slots:
    void sendCreate();

signals:
    void createSig(pnVaultNode& node, hsUint32 parent);

protected:
    void changeEvent(QEvent *e);

private:
    Ui::CreateNodeDialog *ui;
    qtVaultNode node;
    hsUint32 parentIdx;
};

#endif // CREATEDIALOG_H
