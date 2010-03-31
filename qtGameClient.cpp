#include "MoulKI.h"
#include "qtGameClient.h"
#include <core/Stream/hsRAMStream.h>

Q_DECLARE_METATYPE(plUuid);

qtGameClient::qtGameClient(QObject* parent) : QObject(parent) {
    qRegisterMetaType<plUuid>();
    setKeys(KEY_Game_X, KEY_Game_N);
    setClientInfo(871, 50, 1, s_moulUuid);
}

qtGameClient::~qtGameClient() {
}

void qtGameClient::joinAge(hsUint32 serverAddr, hsUint32 playerId, hsUint32 mcpId) {
    // I've seen a function that does this somewhere in zrax's code..
    // would be nice if I could find it, so I wouldn't have to roll my own here :P
    plString serverString = plString::Format("%u.%u.%u.%u",
                                             (serverAddr & 0x000000FF) >> 0,
                                             (serverAddr & 0x0000FF00) >> 8,
                                             (serverAddr & 0x00FF0000) >> 16,
                                             (serverAddr & 0xFF000000) >> 24
                                             );
    qWarning("Joining age server at: %s", serverString.cstr());
    hsUint32 result;
    if((result = pnGameClient::connect(serverString.cstr())) != kNetSuccess) {
        qWarning("Error Connecting to Game Server (%s)", GetNetErrorString(result));
        return;
    }else{
        qWarning("Successfully connected to Game Server");
    }
    sendJoinAgeRequest(mcpId, fAccountId, playerId);
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
