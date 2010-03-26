#ifndef QTGAMECLIENT_H
#define QTGAMECLIENT_H

#include <QObject>
#include <net/game/pnGameClient.h>

class qtGameClient : public QObject, public pnGameClient
{
    Q_OBJECT
public:
    qtGameClient(QObject* parent = 0);
    ~qtGameClient();

    void onPropagateMessage(plCreatable *msg);

signals:
    void receivedGameMsg(QString);
};

#endif // QTGAMECLIENT_H
