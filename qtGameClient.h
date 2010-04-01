#ifndef QTGAMECLIENT_H
#define QTGAMECLIENT_H

#include <QObject>
#include <net/game/pnGameClient.h>

class qtGameClient : public QObject, public pnGameClient
{
    Q_OBJECT
private:
    hsUint32 fMcpId;
    hsUint32 fPlayerId;
public:
    qtGameClient(QObject* parent = 0);
    ~qtGameClient();

    void joinAge(hsUint32 serverAddr, hsUint32 playerId, hsUint32 mcpId);
    void onPropagateMessage(plCreatable *msg);
    void onJoinAgeReply(hsUint32 transId, ENetError result);

signals:
    void receivedGameMsg(QString);
};

#endif // QTGAMECLIENT_H
