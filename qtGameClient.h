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
};

#endif // QTGAMECLIENT_H
