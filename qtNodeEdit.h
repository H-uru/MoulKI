#ifndef QTNODEEDIT_H
#define QTNODEEDIT_H

#include <QWidget>
#include <QTableWidgetItem>
#include "qtVault.h"

namespace Ui {
    class qtNodeEdit;
}

class qtNodeEdit : public QWidget {
    Q_OBJECT
public:
    qtNodeEdit(QWidget *parent = 0);
    ~qtNodeEdit();

    void setNode(qtVaultNode* pn);
    qtVaultNode* getNode();
    void update();

signals:
    void isDirty(bool);

protected:
    void changeEvent(QEvent *e);

private:
    Ui::qtNodeEdit *ui;
    qtVaultNode* node;

private slots:
    void dataRowChanged(QTableWidgetItem* item);
    void editNodeTitle(QString);
    void editNodeText();
    void loadNodeImage();
    void saveNodeImage();

};

#endif // QTNODEEDIT_H
