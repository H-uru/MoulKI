#include "MoulKI.h"
#include "qtGameClient.h"
#include <core/Stream/hsRAMStream.h>
#include <core/PRP/NetMessage/plNetMsgLoadClone.h>
#include <core/PRP/Message/plLoadAvatarMsg.h>

Q_DECLARE_METATYPE(plUuid);

qtGameClient::qtGameClient(QObject* parent) : QObject(parent) {
    qRegisterMetaType<plUuid>();
    setKeys(KEY_Game_X, KEY_Game_N);
    setClientInfo(BUILD_NUMBER, 50, 1, s_moulUuid);
}

qtGameClient::~qtGameClient() {
}

void qtGameClient::joinAge(hsUint32 serverAddr, hsUint32 playerId, hsUint32 mcpId) {
    // I've seen a function that does this somewhere in zrax's code..
    // would be nice if I could find it, so I wouldn't have to roll my own here :P
    plString serverString = plString::Format("%u.%u.%u.%u",
                                             (serverAddr & 0xFF000000) >> 24,
                                             (serverAddr & 0x00FF0000) >> 16,
                                             (serverAddr & 0x0000FF00) >> 8,
                                             (serverAddr & 0x000000FF) >> 0
                                             );
    qWarning("Joining age server at: %s", serverString.cstr());
    hsUint32 result;
    if((result = pnGameClient::connect(serverString.cstr())) != kNetSuccess) {
        qWarning("Error Connecting to Game Server (%s)", GetNetErrorString(result));
        return;
    }else{
        qWarning("Successfully connected to Game Server");
    }
    fMcpId = mcpId;
    fPlayerId = playerId;
    sendJoinAgeRequest(mcpId, fAccountId, playerId);
}

void qtGameClient::onJoinAgeReply(hsUint32 transId, ENetError result) {
    if(result == kNetSuccess) {
        qWarning("Joined Age");
        plUoid clientMgr;
        clientMgr.setName("kNetClientMgr_KEY");
        clientMgr.setID(0);
        clientMgr.setType(0x0052); //plNetClientMger
        plLocation clientMgrLoc(pvLive);
        clientMgrLoc.setVirtual();
        clientMgrLoc.setFlags(0x0000);
        clientMgr.setLocation(clientMgrLoc);
        plUoid playerKey;
        if(true) {
            playerKey.setName("Male");
            playerKey.setID(78);
            plLocation playerKeyLoc(pvLive);
            playerKeyLoc.setPageNum(1);
            playerKeyLoc.setSeqPrefix(-6);
            playerKeyLoc.setFlags(0x0004);
            //playerKey.setLocation(playerKey);
        }
        playerKey.setType(0x0001); //plSceneObject
        playerKey.setCloneIDs(2, fPlayerId); //not sure what the 2 signifies
        plUoid avMgr;
        avMgr.setName("kAvatarMgr_KEY");
        plLocation avMgrLoc(pvLive);
        avMgrLoc.setVirtual();
        avMgrLoc.setFlags(0x0000);
        avMgr.setLocation(avMgrLoc);
        avMgr.setType(0x00F4); //plAvatarMgr
        avMgr.setID(0);
        plLoadAvatarMsg loadAvMsg;
        //loadAvMsg.addReceiver(clientMgr);
        loadAvMsg.setBCastFlags(0x00000840);
        //loadAvMsg.setCloneKey(playerKey);
        //loadAvMsg.setRequestorKey(avMgr);
        loadAvMsg.setOriginatingPlayerID(fPlayerId);
        loadAvMsg.setUserData(0);
        loadAvMsg.setValidMsg(1);
        loadAvMsg.setIsLoading(1);
        loadAvMsg.setIsPlayer(1);
        plNetMsgLoadClone loadClone;
        loadClone.setPlayerID(fPlayerId);
        loadClone.setMessage(&loadAvMsg);
        loadClone.setIsPlayer(1);
        loadClone.setIsLoading(1);
        loadClone.setIsInitialState(0);
        plNetMsgObjectHelper helper;
        helper.setUoid(playerKey);
        loadClone.setObject(helper);
        propagateMessage(&loadClone);
    }else{
        qWarning("Join Age Failed (%s)", GetNetErrorString(result));
    }
}

void qtGameClient::onPropagateMessage(plCreatable *msg) {
    hsRAMStream S(pvLive);
    pfPrcHelper prc(&S);
    msg->prcWrite(&prc);
    char* data = new char[S.size()];
    S.copyTo(data, S.size());
    emit receivedGameMsg(QString(data));
    delete[] data;
}
