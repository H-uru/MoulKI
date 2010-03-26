#include "MoulKI.h"
#include "qtGameClient.h"
#include <core/Stream/hsRAMStream.h>

qtGameClient::qtGameClient(QObject* parent) : QObject(parent) {
    setKeys(KEY_Game_X, KEY_Game_N);
    setClientInfo(871, 50, 1, s_moulUuid);
}

qtGameClient::~qtGameClient() {
}

void qtGameClient::onPropagateMessage(plCreatable *msg) {
    hsRAMStream S(pvLive);
    pfPrcHelper prc(&S);
    msg->prcWrite(&prc);
    char data[S.size()];
    S.copyTo(data, S.size());
    emit receivedGameMsg(QString(data));
}
