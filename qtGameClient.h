#ifndef QTGAMECLIENT_H
#define QTGAMECLIENT_H

#include <QObject>
#include <net/game/pnGameClient.h>

class qtVaultNode;

class qtGameClient : public QObject, public pnGameClient
{
    Q_OBJECT
private:
    hsUint32 fMcpId;
    plString fAgeFilename;
    hsTArray<unsigned int> fAgePlayers;
    qtVaultNode* fPlayerNode;

public:
    qtGameClient(QObject* parent = 0);
    ~qtGameClient();

    void setPlayer(qtVaultNode* player);
    void joinAge(hsUint32 serverAddr, hsUint32 mcpId, plString ageFilename);
    void onPropagateMessage(plCreatable *msg);
    void onJoinAgeReply(hsUint32 transId, ENetError result);

public slots:
    void sendAgeChat(plString message);

signals:
    void receivedGameMsg(QString);
    void setMeOnline(hsUint32 player, plString ageFilename);
};

#endif // QTGAMECLIENT_H
