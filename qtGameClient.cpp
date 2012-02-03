#include "MoulKI.h"
#include "qtGameClient.h"
#include <Stream/hsRAMStream.h>
#include <PRP/NetMessage/plNetMsgLoadClone.h>
#include <PRP/NetMessage/plNetMsgMembersList.h>
#include <PRP/Message/plLoadAvatarMsg.h>
#include <PRP/Message/pfKIMsg.h>

qtGameClient::qtGameClient(MoulKI* ki) : QObject(ki), pnGameClient(ki->getResManager()), fPlayerNode(NULL), fAgeInfoNode(NULL) {
    setKeys(ki->Keys.Game.X, ki->Keys.Game.N);
    setClientInfo(BUILD_NUMBER, 50, 1, s_moulUuid);
}

qtGameClient::~qtGameClient() {
}

void qtGameClient::setPlayer(qtVaultNode* player) {
    fPlayerNode = player;
}

void qtGameClient::setAgeInfo(qtVaultNode* ageInfo) {
    fAgeInfoNode = ageInfo;
    emit setMeOnline(fPlayerNode->getNodeIdx(), fAgeInfoNode->getString64(1), fAgeInfoNode->getUuid(0));
}

void qtGameClient::joinAge(uint32_t serverAddr, uint32_t mcpId) {
    // I've seen a function that does this somewhere in zrax's code..
    // would be nice if I could find it, so I wouldn't have to roll my own here :P
    plString serverString = plString::Format("%u.%u.%u.%u",
                                             (serverAddr & 0xFF000000) >> 24,
                                             (serverAddr & 0x00FF0000) >> 16,
                                             (serverAddr & 0x0000FF00) >> 8,
                                             (serverAddr & 0x000000FF) >> 0
                                             );
    qWarning("Joining age server at: %s", serverString.cstr());
    uint32_t result;
    if((result = pnGameClient::connect(serverString.cstr())) != kNetSuccess) {
        qWarning("Error Connecting to Game Server (%s)", GetNetErrorString(result));
        return;
    }else{
        qWarning("Successfully connected to Game Server");
    }
    fMcpId = mcpId;
    sendJoinAgeRequest(mcpId, fAccountId, fPlayerNode->getNodeIdx());
}

void qtGameClient::onJoinAgeReply(uint32_t transId, ENetError result) {
    if(result == kNetSuccess) {
        qWarning("Successfuly Joined Age");
        plKeyData* clientMgr = new plKeyData();
        clientMgr->setName("kNetClientMgr_KEY");
        clientMgr->setID(0);
        clientMgr->setType(0x0052); //plNetClientMger
        plLocation clientMgrLoc(PlasmaVer::pvMoul);
        clientMgrLoc.setVirtual();
        clientMgrLoc.setFlags(0x0000);
        clientMgr->setLocation(clientMgrLoc);
        plKeyData* playerKey = new plKeyData();
        if(true) {
            playerKey->setName("Male");
            playerKey->setID(78);
            plLocation playerKeyLoc(PlasmaVer::pvMoul);
            playerKeyLoc.setPageNum(1);
            playerKeyLoc.setSeqPrefix(-6);
            playerKeyLoc.setFlags(0x0004);
            playerKey->setLocation(playerKeyLoc);
        }
        playerKey->setType(0x0001); //plSceneObject
        playerKey->setCloneIDs(2, fPlayerNode->getNodeIdx()); //not sure what the 2 signifies
        plKeyData* avMgr = new plKeyData();
        avMgr->setName("kAvatarMgr_KEY");
        plLocation avMgrLoc(PlasmaVer::pvMoul);
        avMgrLoc.setVirtual();
        avMgrLoc.setFlags(0x0000);
        avMgr->setLocation(avMgrLoc);
        avMgr->setType(0x00F4); //plAvatarMgr
        avMgr->setID(0);
        plLoadAvatarMsg* loadAvMsg = new plLoadAvatarMsg();
        loadAvMsg->addReceiver(plKey(clientMgr));
        loadAvMsg->setBCastFlags(0x00000840);
        loadAvMsg->setCloneKey(plKey(playerKey));
        loadAvMsg->setRequestor(plKey(avMgr));
        loadAvMsg->setOriginatingPlayerID(fPlayerNode->getNodeIdx());
        loadAvMsg->setUserData(0);
        loadAvMsg->setValidMsg(1);
        loadAvMsg->setIsLoading(1);
        loadAvMsg->setIsPlayer(1);
        plNetMsgLoadClone loadClone;
        loadClone.setFlags(0x00041001);
        loadClone.setTimeSent(plUnifiedTime::GetCurrentTime());
        loadClone.setPlayerID(fPlayerNode->getNodeIdx());
        loadClone.setMessage(loadAvMsg);
        loadClone.setIsPlayer(1);
        loadClone.setIsLoading(1);
        loadClone.setIsInitialState(0);
        loadClone.setCompressionType(0);
        loadClone.setObject(playerKey->getUoid());
        propagateMessage(&loadClone);
        qWarning("Sent LoadClone");
        plNetMsgMembersListReq listReq;
        listReq.setFlags(0x00061001);
        listReq.setTimeSent(plUnifiedTime::GetCurrentTime());
        listReq.setPlayerID(fPlayerNode->getNodeIdx());
        propagateMessage(&listReq);
        qWarning("Sent Members Request");
    }else{
        qWarning("Join Age Failed (%s)", GetNetErrorString(result));
    }
}

void qtGameClient::onPropagateMessage(plCreatable *msg) {
    hsRAMStream S(PlasmaVer::pvMoul);
    pfPrcHelper prc(&S);
    msg->prcWrite(&prc);
    char* data = new char[S.size() + 1];
    S.copyTo(data, S.size());
    data[S.size()] = 0;
    qWarning("%s", data);
    delete[] data;
    if(msg->ClassIndex() == kNetMsgGameMessageDirected) {
        plMessage* gameMsg = ((plNetMsgGameMessageDirected*)msg)->getMessage();
        if(gameMsg->ClassIndex() == kKIMsg) {
            pfKIMsg* kiMsg = (pfKIMsg*)gameMsg;
            QString user = QString(kiMsg->getUser().cstr());
            QString message = QString(kiMsg->getString().cstr());
            if(kiMsg->getFlags() & pfKIMsg::kStatusMsg) { // 0x10 (/me action)
                emit receivedGameMsg(message + "\n");
            }else if(kiMsg->getFlags() & pfKIMsg::kUNUSED1) { // sender is out-of-age (and sending us a location)
                int splitIndex = message.indexOf(">>");
                emit receivedGameMsg("From " + user + " in " + message.mid(2, splitIndex - 2) + ": " + message.mid(splitIndex + 2) + "\n");
            }else if(kiMsg->getFlags() & (pfKIMsg::kPrivateMsg | pfKIMsg::kNeighborMsg)) {
                emit receivedGameMsg("From " + user + ": " + message + "\n");
            }else{ // anything else
                emit receivedGameMsg(user + ": " + message + "\n");
            }
        }
    }else if(msg->ClassIndex() == kNetMsgMembersList) {
        fAgePlayers.clear();
        emit clearAgeList();
        plNetMsgMembersList* membersList = (plNetMsgMembersList*)msg;
        for(unsigned int i = 0; i < membersList->getMembers().getSize(); i++) {
            const plNetMsgMemberInfoHelper* info = &membersList->getMembers()[i];
            const plClientGuid* guid = &info->getClientGuid();
            qWarning("Age Player: %s", guid->getPlayerName().cstr());
            fAgePlayers.append(guid->getPlayerID());
            emit addAgePlayer(guid->getPlayerID(), guid->getPlayerName());
        }
    }else if(msg->ClassIndex() == kNetMsgMemberUpdate) {
        plNetMsgMemberUpdate* memberUpdate = (plNetMsgMemberUpdate*)msg;
        const plNetMsgMemberInfoHelper* info = &memberUpdate->getMember();
        const plClientGuid* guid = &info->getClientGuid();
        if(memberUpdate->getAddMember()) {
            // add member
            qWarning("Added Player: %s", guid->getPlayerName().cstr());
            fAgePlayers.append(guid->getPlayerID());
            emit receivedGameMsg(plString::Format("* %s joined the age\n", guid->getPlayerName().cstr()).cstr());
            emit addAgePlayer(guid->getPlayerID(), guid->getPlayerName());
        }else{
            // remove member
            qWarning("Removed Player: %s", guid->getPlayerName().cstr());
            fAgePlayers.remove(fAgePlayers.find(guid->getPlayerID()));
            emit receivedGameMsg("* Someone left the age\n");
            emit removeAgePlayer(guid->getPlayerID(), guid->getPlayerName());
        }
    }
}

void qtGameClient::sendAgeChat(plString message) {
    plNetMsgGameMessageDirected gameMsg;
    pfKIMsg* kiMsg = new pfKIMsg();
    gameMsg.setFlags(0x00049001);
    gameMsg.setTimeSent(plUnifiedTime::GetCurrentTime());
    gameMsg.setPlayerID(fPlayerNode->getNodeIdx());
    gameMsg.setReceivers(fAgePlayers);
    kiMsg->setBCastFlags(0x00000248);
    kiMsg->setCommand(0);
    if(message.startsWith("/me")) {
        kiMsg->setFlags(pfKIMsg::kStatusMsg);
    }else{
        kiMsg->setFlags(0x00); // standard age chat
    }
    kiMsg->setPlayerID(fPlayerNode->getNodeIdx());
    kiMsg->setUser(fPlayerNode->getIString64(0));
    kiMsg->setString(message);
    gameMsg.setMessage(kiMsg);
    propagateMessage(&gameMsg);
    qWarning("Sent Chat: %s", message.cstr());
    // debug
    /*hsRAMStream S(PlasmaVer::pvMoul);
    pfPrcHelper prc(&S);
    gameMsg.prcWrite(&prc);
    char* data = new char[S.size()];
    S.copyTo(data, S.size());
    qWarning(QString(QByteArray(data, S.size())).toAscii().data());
    delete[] data;*/
}

void qtGameClient::sendPrivate(plString message, uint32_t target) {
    hsTArray<uint32_t> targets;
    targets.append(target);
    plNetMsgGameMessageDirected gameMsg;
    pfKIMsg* kiMsg = new pfKIMsg();
    gameMsg.setFlags(0x00049001);
    gameMsg.setTimeSent(plUnifiedTime::GetCurrentTime());
    gameMsg.setPlayerID(fPlayerNode->getNodeIdx());
    gameMsg.setReceivers(targets);
    kiMsg->setBCastFlags(0x00004248);
    kiMsg->setCommand(0);
    kiMsg->setFlags(pfKIMsg::kPrivateMsg | pfKIMsg::kUNUSED1); // Private message 0x09
    kiMsg->setPlayerID(fPlayerNode->getNodeIdx());
    kiMsg->setUser(fPlayerNode->getIString64(0));
    if(fAgeInfoNode != NULL) {
        kiMsg->setString(plString::Format("<<%s>>%s", fAgeInfoNode->displayName().cstr(), message.cstr()));
    }else{
        kiMsg->setString(plString::Format("<<%s>>%s", "???", message.cstr()));
    }
    gameMsg.setMessage(kiMsg);
    propagateMessage(&gameMsg);
    qWarning("Sent Private Chat to %u: %s", target, message.cstr());
}

void qtGameClient::sendBroadcast(plString message, QList<uint32_t> buddies, int type) {
    hsTArray<uint32_t> targets;
    foreach(uint32_t target, buddies) {
        targets.append(target);
    }
    plNetMsgGameMessageDirected gameMsg;
    pfKIMsg* kiMsg = new pfKIMsg();
    gameMsg.setFlags(0x00049001);
    gameMsg.setTimeSent(plUnifiedTime::GetCurrentTime());
    gameMsg.setPlayerID(fPlayerNode->getNodeIdx());
    gameMsg.setReceivers(targets);
    kiMsg->setBCastFlags(0x00004248);
    kiMsg->setCommand(0);
    if(type == 1) {
        kiMsg->setFlags(pfKIMsg::kUNUSED1); // BUDDIES 0x08
    }else{
        kiMsg->setFlags(pfKIMsg::kNeighborMsg | pfKIMsg::kUNUSED1); // NEIGHBORS 0x28
    }
    kiMsg->setPlayerID(fPlayerNode->getNodeIdx());
    kiMsg->setUser(fPlayerNode->getIString64(0));
    if(fAgeInfoNode != NULL) {
        kiMsg->setString(plString::Format("<<%s>>%s", fAgeInfoNode->displayName().cstr(), message.cstr()));
    }else{
        kiMsg->setString(plString::Format("<<%s>>%s", "???", message.cstr()));
    }
    gameMsg.setMessage(kiMsg);
    propagateMessage(&gameMsg);
    qWarning("Sent Broadcast Chat: %s", message.cstr());
}
