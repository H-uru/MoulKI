#ifndef MOULKI_H
#define MOULKI_H

#include <ResManager/plResManager.h>
#include <SDL/plSDLMgr.h>
#include <QMainWindow>
#include "qtAuthClient.h"
#include "qtGameClient.h"
#include "qtVault.h"

#define BUILD_NUMBER 918
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
    MoulKI(plResManager* resMgr, plSDLMgr* sdlMgr, QWidget *parent = 0);
    ~MoulKI();

    qtVault vault;
    uint32_t activePlayer;

    uint32_t buddyListFolder;
    QList<uint32_t> buddyInfoIds;
    uint32_t neighborListFolder;
    QList<uint32_t> neighborInfoIds;

    ST::string currentAgeName;
    uint32_t currentAgeId;

    struct {
        struct {
            unsigned char N[64];
            unsigned char X[64];
            int G;
        } Auth;
        struct {
            unsigned char N[64];
            unsigned char X[64];
            int G;
        } Game;
    } Keys;

    QString Host;

private:
    Ui::MoulKIClass *ui;

    plResManager* resmgr;
    plSDLMgr* sdlmgr;
    qtAuthClient* authClient;
    qtGameClient* gameClient;
    uint32_t ntdKeys[4];

    struct pendingTrans {
        uint32_t transId;
        uint32_t nodeIdx;

        bool operator==(const pendingTrans& p) const {
            return p.transId == this->transId && p.nodeIdx == this->nodeIdx;
        }
    };

    QList<pendingTrans> pendingSaves;

    void addItemChild(QTreeWidgetItem* item, qtVaultNode* node);
    bool itemHasChild(QTreeWidgetItem* item, qtVaultNode* node);
    void removeTreeNodes(QTreeWidgetItem* item, qtVaultNode* node);
    void addContextMenuToItem(QTreeWidgetItem* item);
    void closeEvent(QCloseEvent* event);

    QTreeWidgetItem* agePlayersItem;
    QTreeWidgetItem* buddiesItem;
    QTreeWidgetItem* neighborsItem;

public:
    plResManager* getResManager() const { return resmgr; }
    plSDLMgr* getSDLMgr() const { return sdlmgr; }

public slots:
    void showLoginDialog();
    void login(QString user, QString pass, QString iniFilename);
    void setStatus(ST::string msg);
    void showPlayers();
    void addNode(uint32_t parent, uint32_t child);
    void removeNode(uint32_t parent, uint32_t child);
    void updateNode(uint32_t idx);
    void setActive(uint32_t playerId);
    void addRoot(uint32_t idx);
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
    void sendAdd(uint32_t parent, uint32_t child, uint32_t owner);
    void sendCreate(pnVaultNode& node, uint32_t parent);
    void sendFind(pnVaultNode& node);
    void showFoundDialog(QList<uint32_t> nodes);
    void showItemContextMenu(QPoint pos);
    void subscribe();
    void fetchTree(uint32_t idx);
    void sendRemove();
    void writeVault();
    void readVault();
    void logoutActivePlayer();
    void getPublicAgeList();
    // authserver events
    void setEncryptionKeys(uint32_t k0, uint32_t k1, uint32_t k2, uint32_t k3);
    void loadStateDescriptors(hsStream* S);
    void nodeSaveComplete(uint32_t transId);
    // gameserver events
    void showJoinAgeDialog();
    void joinAge(ST::string name, plUuid uuid);
    void joinSelectedAge();
    void startGameServer(uint32_t serverAddr, plUuid ageId, uint32_t mcpId, uint32_t ageVaultId);
    void addChatLine(QString line);
    void setOnline(uint32_t playerId, ST::string ageFilename, plUuid ageUuid);
    void sendGameChat();
    void checkCurrentAge();
    // target manipulation
    void addAgePlayer(uint32_t, ST::string);
    void removeAgePlayer(uint32_t, ST::string);
    void clearAgeList();
    void clearChatTargetList(QTreeWidgetItem* item);
    void addRemoveChatTargetItem(QTreeWidgetItem* item, qtVaultNode* infoNode, bool remove=0);
    bool itemTreeContains(QTreeWidgetItem* item, uint32_t playerId);
};

#endif // MOULKI_H
