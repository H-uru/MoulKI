#ifndef QTGAMECLIENT_H
#define QTGAMECLIENT_H

#include <QObject>
#include <game/pnGameClient.h>

class qtVaultNode;
class MoulKI;

class qtGameClient : public QObject, public pnGameClient
{
    Q_OBJECT
private:
    uint32_t fMcpId;
    std::vector<unsigned int> fAgePlayers;
    qtVaultNode* fPlayerNode;
    qtVaultNode* fAgeInfoNode;

public:
    qtGameClient(MoulKI* ki = 0);
    ~qtGameClient();

    void setPlayer(qtVaultNode* player);
    void setAgeInfo(qtVaultNode* ageInfo);
    void joinAge(uint32_t serverAddr, uint32_t mcpId);
    void onPropagateMessage(plCreatable *msg);
    void onJoinAgeReply(uint32_t transId, ENetError result);

public slots:
    void sendAgeChat(ST::string message);
    void sendBroadcast(ST::string message, QList<uint32_t> targets, int type);
    void sendPrivate(ST::string message, uint32_t target);

signals:
    void receivedGameMsg(QString);
    void setMeOnline(uint32_t player, ST::string ageFilename, plUuid ageUuid);
    void addAgePlayer(uint32_t, ST::string);
    void removeAgePlayer(uint32_t, ST::string);
    void clearAgeList();
};

#endif // QTGAMECLIENT_H
