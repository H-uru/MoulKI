#include "MoulKI.h"
#include "qtAuthClient.h"

qtAuthClient::qtAuthClient(MoulKI* ki) : QObject(ki), parent(ki) {
    setKeys(KEY_Auth_X, KEY_Auth_N);
    setClientInfo(BUILD_NUMBER, 50, 1, s_moulUuid);
}

qtAuthClient::~qtAuthClient() {
}

void qtAuthClient::startLogin(QString user, QString pass) {
    players.clear();
    // apparently HSPlasma still doesn't lowercase the username
    this->user = plString(user.toLower().toAscii().constData());
    this->pass = plString(pass.toAscii().constData());
    setStatus("Connecting...");
    if(pnAuthClient::connect(MOUL_HOST) != kNetSuccess) {
        setStatus("Error Connecting To Server");
        return;
    }
    sendClientRegisterRequest();
}

void qtAuthClient::onClientRegisterReply(hsUint32 serverChallenge) {
    setStatus("Authenticating...");
    sendAcctLoginRequest(serverChallenge, rand(), user, pass);
}

void qtAuthClient::onAcctPlayerInfo(hsUint32 transId, hsUint32 playerId, const plString& playerName, const plString& avatarModel, hsUint32 explorer) {
    authPlayer player;
    player.ID = playerId;
    player.Name = playerName;
    player.avatar = avatarModel;
    players.append(player);
    qWarning("Added player %s (%u)", playerName.cstr(), playerId);
}

void qtAuthClient::onAcctLoginReply(hsUint32 transId, ENetError result, const plUuid& acctUuid, hsUint32 acctFlags, hsUint32 billingType, const hsUint32* encryptKey) {
    if(result == kNetSuccess) {
        setStatus("Auth Successful");
        this->acctUuid = acctUuid;
        emit loginSuccessful();
    }else{
        setStatus(plString::Format("Auth Failed (%s)", GetNetErrorString(result)).cstr());
    }
}

void qtAuthClient::onVaultNodeRefsFetched(hsUint32 transId, ENetError result, size_t count, const pnVaultNodeRef* refs) {
    for(unsigned int i = 0; i < count; i++) {
        setStatus(plString::Format("Ref: {%u -> %u} %u", refs[i].fParent, refs[i].fChild, refs[i].fOwner).cstr());
        parent->vault.addRef(refs[i]);
        if(!fetchQueue.contains(refs[i].fChild)) {
            fetchQueue.append(refs[i].fChild);
            sendVaultNodeFetch(refs[i].fChild);
        }
        if(!fetchQueue.contains(refs[i].fParent)) {
            fetchQueue.append(refs[i].fParent);
            sendVaultNodeFetch(refs[i].fParent);
        }
    }
}

void qtAuthClient::onVaultNodeFetched(hsUint32 transId, ENetError result, const pnVaultNode& node) {
    setStatus(plString::Format("Node: (%u)", node.getNodeIdx()));
    parent->vault.addNode(node);
}

void qtAuthClient::onVaultNodeChanged(hsUint32 nodeId, const plUuid &revisionId) {
    if(!fetchQueue.contains(nodeId))
        fetchQueue.append(nodeId);
    sendVaultNodeFetch(nodeId);
}

void qtAuthClient::onVaultNodeAdded(hsUint32 parent, hsUint32 child, hsUint32 owner) {
    pnVaultNodeRef ref;
    ref.fParent = parent;
    ref.fChild = child;
    ref.fOwner = owner;
    setStatus(plString::Format("Ref: {%u -> %u} %u", ref.fParent, ref.fChild, ref.fOwner).cstr());
    this->parent->vault.addRef(ref);
    if(!fetchQueue.contains(parent)) {
        fetchQueue.append(parent);
        sendVaultNodeFetch(parent);
        sendVaultFetchNodeRefs(parent);
    }
    if(!fetchQueue.contains(child)) {
        fetchQueue.append(child);
        sendVaultNodeFetch(child);
        sendVaultFetchNodeRefs(child);
    }
}

void qtAuthClient::onVaultNodeRemoved(hsUint32 parent, hsUint32 child) {
    setStatus(plString::Format("UnRef: {%u -> %u}", parent, child).cstr());
    this->parent->vault.removeRef(parent, child);
}

void qtAuthClient::onVaultNodeCreated(hsUint32 transId, ENetError result, hsUint32 nodeId) {
    if(result == kNetSuccess) {
        foreach(queuedRef ref, refQueue) {
            if(ref.fTransId == transId) {
                sendVaultNodeAdd(ref.fParent, nodeId, 0);
                refQueue.removeAll(ref);
                break;
            }
        }
    }else{
        setStatus(plString::Format("Create Node Failed (%s)", GetNetErrorString(result)));
    }
}

void qtAuthClient::queueRef(hsUint32 transId, hsUint32 parent) {
    queuedRef ref;
    ref.fTransId = transId;
    ref.fParent = parent;
    refQueue.append(ref);
}

bool qtAuthClient::queuedRef::operator ==(const qtAuthClient::queuedRef& ref) {
    return fTransId == ref.fTransId;
}

void qtAuthClient::setStatus(const char* msg) {
    emit sigStatus(plString(msg));
}

void qtAuthClient::setStatus(plString msg) {
    emit sigStatus(msg);
}

void qtAuthClient::onVaultNodeFindReply(hsUint32 transId, ENetError result, size_t count, const hsUint32 *nodes) {
    if(result == kNetSuccess) {
        setStatus(plString::Format("Found %u Nodes", count));
        // it's not safe to send an array allocated elsewhere via a queued signal
        QList<hsUint32> nodeList;
        for(hsUint32 i = 0; i < count; i++) {
            nodeList.append(nodes[i]);
        }
        emit foundNodes(count, nodeList);
    }else{
        setStatus(plString::Format("Find Node Failure: (%s)", GetNetErrorString(result)));
    }
}

void qtAuthClient::onVaultAddNodeReply(hsUint32 transId, ENetError result) {
    if(result == kNetSuccess) {
        setStatus("Add Node Successful");
    }else{
        setStatus(plString::Format("Add Node Failure: (%s)", GetNetErrorString(result)));
    }
}

void qtAuthClient::onVaultSaveNodeReply(hsUint32 transId, ENetError result) {
    if(result == kNetSuccess) {
        setStatus("Save Node Successful");
    }else{
        setStatus(plString::Format("Save Node Failure: (%s)", GetNetErrorString(result)));
    }
}

void qtAuthClient::onVaultRemoveNodeReply(hsUint32 transId, ENetError result) {
    if(result == kNetSuccess) {
        setStatus("Remove Node Successful");
    }else{
        setStatus(plString::Format("Remove Node Failure: (%s)", GetNetErrorString(result)));
    }
}

void qtAuthClient::onAgeReply(hsUint32 transId, ENetError result, hsUint32 mcpId, const plUuid &ageInstanceId, hsUint32 ageVaultId, hsUint32 gameServerAddress) {
    if(result == kNetSuccess) {
        setStatus("Age Request Successful");
        emit gotAge(gameServerAddress, ageInstanceId, mcpId, ageVaultId);
    }else{
        setStatus(plString::Format("Age Request Failed: (%s)", GetNetErrorString(result)));
    }
}
