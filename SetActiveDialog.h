#ifndef SETACTIVEDIALOG_H
#define SETACTIVEDIALOG_H

#include <QtGui/QDialog>

#include "qtAuthClient.h"

namespace Ui {
    class SetActiveDialog;
}

class SetActiveDialog : public QDialog {
    Q_OBJECT
    Q_DISABLE_COPY(SetActiveDialog)
public:
    explicit SetActiveDialog(QWidget *parent = 0);
    virtual ~SetActiveDialog();
    void setPlayers(QList<authPlayer> plyrs);
    void setFoundNodes(QList<hsUint32> nodes);

protected:
    virtual void changeEvent(QEvent *e);

private:
    Ui::SetActiveDialog *m_ui;

private slots:
    void setActive();
    void sendFetches();

signals:
    void setActive(hsUint32 player);
    void fetchFound(hsUint32 idx);
};

#endif // SETACTIVEDIALOG_H
