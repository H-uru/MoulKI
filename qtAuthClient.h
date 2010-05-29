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
    hsUint32 ID;
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
    QList<hsUint32> fetchQueue;
    plString user;
    plString pass;

    struct queuedRef {
        hsUint32 fTransId;
        hsUint32 fParent;

        bool operator==(const queuedRef& ref);
    };
    QList<queuedRef> refQueue;

    QMap<hsUint32, hsStream*> sdlFiles;

public:
    QList<authPlayer> players;
    plUuid acctUuid;

    qtAuthClient(MoulKI* ki);
    ~qtAuthClient();

    void startLogin(QString user, QString pass);
    void onClientRegisterReply(hsUint32 serverChallenge);
    void onAcctLoginReply(hsUint32 transId, ENetError result,
            const plUuid& acctUuid, hsUint32 acctFlags,
            hsUint32 billingType, const hsUint32* encryptKey);
    void onAcctPlayerInfo(hsUint32 transId, hsUint32 playerId,
            const plString &playerName, const plString& avatarModel,
            hsUint32 explorer);
    void onAgeReply(hsUint32 transId, ENetError result, hsUint32 mcpId,
            const plUuid &ageInstanceId, hsUint32 ageVaultId,
            hsUint32 gameServerAddress);

    void onFileListReply(hsUint32 transId, ENetError result,
            size_t count, const pnAuthFileItem* files);
    void onFileDownloadChunk(hsUint32 transId, ENetError result,
            hsUint32 totalSize, hsUint32 chunkOffset,
            size_t chunkSize, const unsigned char* chunkData);

    void onVaultNodeRefsFetched(hsUint32 transId, ENetError result,
            size_t count, const pnVaultNodeRef* refs);
    void onVaultNodeFetched(hsUint32 transId, ENetError result,
            const pnVaultNode& node);
    void onVaultNodeChanged(hsUint32 nodeId, const plUuid& revisionId);
    void onVaultNodeAdded(hsUint32 parent, hsUint32 child, hsUint32 owner);
    void onVaultNodeRemoved(hsUint32 parent, hsUint32 child);
    void onVaultNodeCreated(hsUint32 transId, ENetError result,
            hsUint32 nodeId);
    void onVaultNodeFindReply(hsUint32 transId, ENetError result,
            size_t count, const hsUint32 *nodes);

    void onVaultSaveNodeReply(hsUint32 transId, ENetError result);
    void onVaultAddNodeReply(hsUint32 transId, ENetError result);
    void onVaultRemoveNodeReply(hsUint32 transId, ENetError result);

    void setStatus(const char* msg);
    void setStatus(plString msg);

    void queueRef(hsUint32 transId, hsUint32 parent);

signals:
    void sigStatus(plString msg);
    void foundNodes(int count, QList<hsUint32> nodes);
    void loginSuccessful();
    void gotAge(hsUint32 gameServerAddr, plUuid ageId, hsUint32 mcpId,
            hsUint32 vaultId);
    void gotEncKeys(hsUint32 k0, hsUint32 k1, hsUint32 k2, hsUint32 k3);
    void gotSDLFile(hsStream* file);
};

#endif // QTAUTHCLIENT_H
