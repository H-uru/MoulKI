#ifndef QTGAMECLIENT_H
#define QTGAMECLIENT_H

#include <QObject>
#include <net/game/pnGameClient.h>

class qtGameClient : public QObject, public pnGameClient
{
    Q_OBJECT
private:
    plUuid acctId;
    plUuid ageInstance;
    plString ageName;
    hsUint32 playerId;
    hsUint32 mcpId;
public:
    qtGameClient(QObject* parent = 0);
    ~qtGameClient();

    void joinAge(hsUint32 serverAddr, hsUint32 playerId, hsUint32 mcpId);
    void onPropagateMessage(plCreatable *msg);

signals:
    void receivedGameMsg(QString);
};

#endif // QTGAMECLIENT_H
