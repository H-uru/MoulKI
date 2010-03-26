#ifndef QTAUTHCLIENT_H
#define QTAUTHCLIENT_H

#include <net/auth/pnAuthClient.h>
#include <QObject>
#include <QList>

#include "qtVault.h"

Q_DECLARE_METATYPE(plString)

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
    // the fetch queue acts as a comprehensive list of all nodes fetched and pending fetch by the client
    QList<hsUint32> fetchQueue;
    plString user;
    plString pass;

    struct queuedRef {
        hsUint32 fTransId;
        hsUint32 fParent;

        bool operator==(const queuedRef& ref);
    };
    QList<queuedRef> refQueue;

public:
    QList<authPlayer> players;
    plUuid acctUuid;

    qtAuthClient(MoulKI* ki);
    ~qtAuthClient();

    void startLogin(QString user, QString pass);
    void onClientRegisterReply(hsUint32 serverChallenge);
    void onAcctLoginReply(hsUint32 transId, ENetError result, const plUuid& acctUuid, hsUint32 acctFlags, hsUint32 billingType, const hsUint32* encryptKey);
    void onAcctPlayerInfo(hsUint32 transId, hsUint32 playerId, const plString &playerName, const plString& avatarModel, hsUint32 explorer);

    void onVaultNodeRefsFetched(hsUint32 transId, ENetError result, size_t count, const pnVaultNodeRef* refs);
    void onVaultNodeFetched(hsUint32 transId, ENetError result, const pnVaultNode& node);
    void onVaultNodeChanged(hsUint32 nodeId, const plUuid& revisionId);
    void onVaultNodeAdded(hsUint32 parent, hsUint32 child, hsUint32 owner);
    void onVaultNodeRemoved(hsUint32 parent, hsUint32 child);
    void onVaultNodeCreated(hsUint32 transId, ENetError result, hsUint32 nodeId);
    void onVaultNodeFindReply(hsUint32 transId, ENetError result, size_t count, const hsUint32 *nodes);

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
};

#endif // QTAUTHCLIENT_H
