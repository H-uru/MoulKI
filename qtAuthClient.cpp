#include "MoulKI.h"
#include "qtAuthClient.h"

qtAuthClient::qtAuthClient(MoulKI* ki) : pnAuthClient(ki->getResManager()),
        QObject(ki), parent(ki) {
    setKeys(ki->Keys.Auth.X, ki->Keys.Auth.N);
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
    if(pnAuthClient::connect(parent->Host.toAscii().data()) != kNetSuccess) {
        setStatus("Error Connecting To Server");
        return;
    }
    sendClientRegisterRequest();
}

void qtAuthClient::onClientRegisterReply(uint32_t serverChallenge) {
    setStatus("Authenticating...");
    sendAcctLoginRequest(serverChallenge, rand(), user, pass);
}

void qtAuthClient::onAcctPlayerInfo(uint32_t, uint32_t playerId, const plString& playerName, const plString& avatarModel, uint32_t) {
    authPlayer player;
    player.ID = playerId;
    player.Name = playerName;
    player.avatar = avatarModel;
    players.append(player);
    qWarning("Added player %s (%u)", playerName.cstr(), playerId);
}

void qtAuthClient::onAcctLoginReply(uint32_t, ENetError result,
        const plUuid& acctUuid, uint32_t, uint32_t,
        const uint32_t* encryptKey) {
    if (result != kNetSuccess) {
        setStatus(plString::Format("Auth Failed (%s)",
                    GetNetErrorString(result)).cstr());
        return;
    }

    setStatus("Auth Successful");
    this->acctUuid = acctUuid;
    this->sendFileListRequest("SDL", "sdl");

    emit gotEncKeys(encryptKey[0], encryptKey[1],
            encryptKey[2], encryptKey[3]);
    emit loginSuccessful();
}

void qtAuthClient::onPublicAgeList(uint32_t, ENetError result, size_t count, const pnNetAgeInfo* ages) {
    if(result != kNetSuccess) {
        setStatus(plString::Format("Get Public Ages Failed (%s)",
                    GetNetErrorString(result)).cstr());
        return;
    }

    setStatus(plString::Format("Got %d Public Ages", count).cstr());

    QList< QPair<QString, plUuid> > publicAges;
    for(size_t i = 0; i < count; i++) {
        publicAges.append(QPair<QString, plUuid>(ages[i].fAgeFilename.cstr(), ages[i].fAgeInstanceId));
    }

    emit gotPublicAges(publicAges);
}

void qtAuthClient::onFileListReply(uint32_t, ENetError,
        size_t count, const pnAuthFileItem* files) {

    for (size_t i = 0; i < count; i++) {
        qWarning("Downloading file %s (%d bytes)",
                files[i].fFilename.cstr(), files[i].fFileSize);

        uint32_t fileTrans;
        fileTrans = this->sendFileDownloadRequest(files[i].fFilename);
        sdlFiles.insert(fileTrans, new hsRAMStream(PlasmaVer::pvMoul));
    }
}

void qtAuthClient::onFileDownloadChunk(uint32_t transId, ENetError result,
        uint32_t totalSize, uint32_t chunkOffset, size_t chunkSize,
        const unsigned char* chunkData) {
    if (result != kNetSuccess) {
        setStatus(plString::Format("File download failed (%s)",
                    GetNetErrorString(result)).cstr());
        return;
    }

    hsStream* S = sdlFiles[transId];
    S->write(chunkSize, chunkData);

    if (chunkOffset + chunkSize == totalSize) {
        S->rewind();
        qWarning("Successfully downloaded a %d byte file.", totalSize);
        emit gotSDLFile(S);
    }
}

void qtAuthClient::onVaultNodeRefsFetched(uint32_t, ENetError, size_t count, const pnVaultNodeRef* refs) {
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

void qtAuthClient::onVaultNodeFetched(uint32_t, ENetError, const pnVaultNode& node) {
    setStatus(plString::Format("Node: (%u)", node.getNodeIdx()));
    parent->vault.addNode(node);
}

void qtAuthClient::onVaultNodeChanged(uint32_t nodeId, const plUuid&) {
    if(!fetchQueue.contains(nodeId))
        fetchQueue.append(nodeId);
    sendVaultNodeFetch(nodeId);
}

void qtAuthClient::onVaultNodeAdded(uint32_t parent, uint32_t child, uint32_t owner) {
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

void qtAuthClient::onVaultNodeRemoved(uint32_t parent, uint32_t child) {
    setStatus(plString::Format("UnRef: {%u -> %u}", parent, child).cstr());
    this->parent->vault.removeRef(parent, child);
}

void qtAuthClient::onVaultNodeCreated(uint32_t transId, ENetError result, uint32_t nodeId) {
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

void qtAuthClient::queueRef(uint32_t transId, uint32_t parent) {
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

void qtAuthClient::onVaultNodeFindReply(uint32_t, ENetError result, size_t count, const uint32_t *nodes) {
    if(result == kNetSuccess) {
        setStatus(plString::Format("Found %u Nodes", count));
        // it's not safe to send an array allocated elsewhere via a queued signal
        QList<uint32_t> nodeList;
        for(uint32_t i = 0; i < count; i++) {
            nodeList.append(nodes[i]);
        }
        emit foundNodes(nodeList);
    }else{
        setStatus(plString::Format("Find Node Failure: (%s)", GetNetErrorString(result)));
    }
}

void qtAuthClient::onVaultAddNodeReply(uint32_t, ENetError result) {
    if(result == kNetSuccess) {
        setStatus("Add Node Successful");
    }else{
        setStatus(plString::Format("Add Node Failure: (%s)", GetNetErrorString(result)));
    }
}

void qtAuthClient::onVaultSaveNodeReply(uint32_t, ENetError result) {
    if(result == kNetSuccess) {
        setStatus("Save Node Successful");
    }else{
        setStatus(plString::Format("Save Node Failure: (%s)", GetNetErrorString(result)));
    }
}

void qtAuthClient::onVaultRemoveNodeReply(uint32_t, ENetError result) {
    if(result == kNetSuccess) {
        setStatus("Remove Node Successful");
    }else{
        setStatus(plString::Format("Remove Node Failure: (%s)", GetNetErrorString(result)));
    }
}

void qtAuthClient::onAgeReply(uint32_t, ENetError result, uint32_t mcpId, const plUuid &ageInstanceId, uint32_t ageVaultId, uint32_t gameServerAddress) {
    if(result == kNetSuccess) {
        setStatus("Age Request Successful");
        emit gotAge(gameServerAddress, ageInstanceId, mcpId, ageVaultId);
    }else{
        setStatus(plString::Format("Age Request Failed: (%s)", GetNetErrorString(result)));
    }
}
