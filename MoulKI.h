#ifndef MOULKI_H
#define MOULKI_H

#include <QtGui/QMainWindow>
#include "qtAuthClient.h"
#include "qtGameClient.h"
#include "qtVault.h"

#define BUILD_NUMBER 887
#define MOUL_HOST "184.73.198.22"
#include "keys.cpp"
static const plUuid s_moulUuid("ea489821-6c35-4bd0-9dae-bb17c585e680");

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

    qtVault vault;
    hsUint32 activePlayer;
    plString currentAgeName;

private:
    Ui::MoulKIClass *ui;

    qtAuthClient* authClient;
    qtGameClient* gameClient;

    void addItemChild(QTreeWidgetItem* item, qtVaultNode* node);
    bool itemHasChild(QTreeWidgetItem* item, qtVaultNode* node);
    void removeTreeNodes(QTreeWidgetItem* item, qtVaultNode* node);
    void addContextMenuToItem(QTreeWidgetItem* item);

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
    void revertNode();
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
    void showItemContextMenu(QPoint pos);
    void subscribe();
    void fetchTree(hsUint32 idx);
    void sendRemove();
    void writeVault();
    void readVault();
    // gameserver events
    void showJoinAgeDialog();
    void joinAge(plString name, plUuid uuid);
    void startGameServer(hsUint32 serverAddr, plUuid ageId, hsUint32 mcpId, hsUint32 ageVaultId);
    void addChatLine(QString line);
    void setOnline(hsUint32 playerId, plString ageFilename);
    void sendGameChat();
};

#endif // MOULKI_H
