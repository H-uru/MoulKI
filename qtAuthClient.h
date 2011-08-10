#ifndef QTAUTHCLIENT_H
#define QTAUTHCLIENT_H

#include <auth/pnAuthClient.h>
#include <Stream/hsRAMStream.h>
#include <QObject>
#include <QList>
#include <QMap>

#include "qtVault.h"

class MoulKI;

struct authPlayer {
    uint32_t ID;
    plString Name;
    plString avatar;
};

class qtAuthClient : public QObject, public pnAuthClient
{
    Q_OBJECT
private:
    MoulKI* parent;
    /* the fetch queue acts as a comprehensive list of all nodes fetched
       and pending fetch by the client */
    QList<uint32_t> fetchQueue;
    plString user;
    plString pass;

    struct queuedRef {
        uint32_t fTransId;
        uint32_t fParent;

        bool operator==(const queuedRef& ref);
    };
    QList<queuedRef> refQueue;

    QMap<uint32_t, hsStream*> sdlFiles;

public:
    QList<authPlayer> players;
    plUuid acctUuid;

    qtAuthClient(MoulKI* ki);
    ~qtAuthClient();

    void startLogin(QString user, QString pass);
    void onClientRegisterReply(uint32_t serverChallenge);
    void onAcctLoginReply(uint32_t transId, ENetError result,
            const plUuid& acctUuid, uint32_t acctFlags,
            uint32_t billingType, const uint32_t* encryptKey);
    void onAcctPlayerInfo(uint32_t transId, uint32_t playerId,
            const plString &playerName, const plString& avatarModel,
            uint32_t explorer);
    void onAgeReply(uint32_t transId, ENetError result, uint32_t mcpId,
            const plUuid &ageInstanceId, uint32_t ageVaultId,
            uint32_t gameServerAddress);

    void onFileListReply(uint32_t transId, ENetError result,
            size_t count, const pnAuthFileItem* files);
    void onFileDownloadChunk(uint32_t transId, ENetError result,
            uint32_t totalSize, uint32_t chunkOffset,
            size_t chunkSize, const unsigned char* chunkData);

    void onVaultNodeRefsFetched(uint32_t transId, ENetError result,
            size_t count, const pnVaultNodeRef* refs);
    void onVaultNodeFetched(uint32_t transId, ENetError result,
            const pnVaultNode& node);
    void onVaultNodeChanged(uint32_t nodeId, const plUuid& revisionId);
    void onVaultNodeAdded(uint32_t parent, uint32_t child, uint32_t owner);
    void onVaultNodeRemoved(uint32_t parent, uint32_t child);
    void onVaultNodeCreated(uint32_t transId, ENetError result,
            uint32_t nodeId);
    void onVaultNodeFindReply(uint32_t transId, ENetError result,
            size_t count, const uint32_t *nodes);

    void onVaultSaveNodeReply(uint32_t transId, ENetError result);
    void onVaultAddNodeReply(uint32_t transId, ENetError result);
    void onVaultRemoveNodeReply(uint32_t transId, ENetError result);

    void setStatus(const char* msg);
    void setStatus(plString msg);

    void queueRef(uint32_t transId, uint32_t parent);

signals:
    void sigStatus(plString msg);
    void foundNodes(int count, QList<uint32_t> nodes);
    void loginSuccessful();
    void gotAge(uint32_t gameServerAddr, plUuid ageId, uint32_t mcpId,
            uint32_t vaultId);
    void gotEncKeys(uint32_t k0, uint32_t k1, uint32_t k2, uint32_t k3);
    void gotSDLFile(hsStream* file);
};

#endif // QTAUTHCLIENT_H
