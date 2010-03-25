#ifndef CREATEDIALOG_H
#define CREATEDIALOG_H

#include <QDialog>

#include "qtVault.h"

namespace Ui {
    class CreateDialog;
}

class CreateDialog : public QDialog {
    Q_OBJECT
public:
    CreateDialog(QWidget *parent = 0);
    ~CreateDialog();

    void setParent(hsUint32 idx);

public slots:
    void sendCreate();

signals:
    void createSig(pnVaultNode& node, hsUint32 parent);

protected:
    void changeEvent(QEvent *e);

private:
    Ui::CreateDialog *ui;
    qtVaultNode node;
    hsUint32 parentIdx;
};

#endif // CREATEDIALOG_H
