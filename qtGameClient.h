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
    plString fAgeFilename;

public:
    qtGameClient(QObject* parent = 0);
    ~qtGameClient();

    void joinAge(hsUint32 serverAddr, hsUint32 playerId, hsUint32 mcpId, plString ageFilename);
    void onPropagateMessage(plCreatable *msg);
    void onJoinAgeReply(hsUint32 transId, ENetError result);

public slots:
    void sendChat(plString);

signals:
    void receivedGameMsg(QString);
    void setMeOnline(hsUint32 player, plString ageFilename);
};

#endif // QTGAMECLIENT_H
