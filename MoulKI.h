#ifndef MOULKI_H
#define MOULKI_H

#include <QtGui/QMainWindow>
#include <ResManager/plResManager.h>
#include "qtAuthClient.h"
#include "qtGameClient.h"
#include "qtVault.h"

class QTableWidgetItem;

namespace Ui
{
    class MoulKIClass;
}

class MoulKI : public QMainWindow
{
    Q_OBJECT

public:
    MoulKI(QWidget *parent = 0);
    ~MoulKI();

private:
    Ui::MoulKIClass *ui;

    qtAuthClient authClient;
    qtGameClient gameClient;

    void addItemChild(QTreeWidgetItem* item, qtVaultNode* node);
    bool itemHasChild(QTreeWidgetItem* item, qtVaultNode* node);

public slots:
    void showLoginDialog();
    void login(QString user, QString pass);
    void setStatus(plString msg);
    void showPlayers();
    void addNode(hsUint32 parent, hsUint32 child);
    void removeNode(hsUint32 parent, hsUint32 child);
    void updateNode(hsUint32 idx);
    void setActive(hsUint32 playerId);
    void addRoot(hsUint32 idx);
    // node editing slots
    void saveNodeData();
    void nodeDirty(bool);
    void setShownNode();
    // context menu and associated vault manipulation slots
    void showRefDialog();
    void showCreateDialog();
    void showFindDialog();
    void showFetchDialog();
    void sendAdd(hsUint32 parent, hsUint32 child, hsUint32 owner);
    void sendCreate(pnVaultNode& node, hsUint32 parent);
    void sendFind(pnVaultNode& node);
    void showFoundDialog(int count, QList<hsUint32> nodes);
    void fetchTree(hsUint32 idx);
    void sendRemove();
    void writeVault();
    void readvault();
    // gameserver events
    void joinAge();

public:
    plResManager resMgr;
    qtVault vault;
    hsUint32 activePlayer;
};

#endif // MOULKI_H
