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

    void setParent(uint32_t idx);
    void setActFind();

public slots:
    void sendCreate();

signals:
    void createSig(pnVaultNode& node, uint32_t parent);
    void findSig(pnVaultNode& node);

protected:
    void changeEvent(QEvent *e);

private:
    Ui::CreateNodeDialog *ui;
    qtVaultNode node;
    uint32_t parentIdx;
    int act;
};

#endif // CREATEDIALOG_H
