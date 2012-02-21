#ifndef QTNODEEDIT_H
#define QTNODEEDIT_H

#include <QWidget>
#include <QTableWidgetItem>
#include "qtVault.h"

namespace Ui {
    class qtNodeEdit;
}

class plSDLMgr;
class plResManager;
class plStateDataRecord;

class qtNodeEdit : public QWidget {
    Q_OBJECT
public:
    qtNodeEdit(QWidget *parent = 0);
    ~qtNodeEdit();

    void setNode(qtVaultNode* pn);
    void setMgrs(plSDLMgr* sdl, plResManager* res);
    qtVaultNode* getNode();
    void update();

signals:
    void isDirty(bool);

protected:
    void changeEvent(QEvent *e);

private:
    Ui::qtNodeEdit *ui;
    qtVaultNode* node;
    plSDLMgr* sdlmgr;
    plResManager* resmgr;
    bool titleEdited;
    bool textEdited;
    QString newTitle;

private slots:
    void dataRowChanged(QTableWidgetItem* item);
    void editNodeTitle(QString title);
    void editNodeText();
    void loadNodeImage();
    void saveNodeImage();
    void sdlChanged(plStateDataRecord* sdl);
    void tabActivated(int tab);

};

#endif // QTNODEEDIT_H
