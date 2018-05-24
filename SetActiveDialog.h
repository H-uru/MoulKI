#ifndef SETACTIVEDIALOG_H
#define SETACTIVEDIALOG_H

#include <QDialog>

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
    void setFoundNodes(QList<uint32_t> nodes);
    void setAgeNodes(QList<qtVaultNode*> nodes);

protected:
    virtual void changeEvent(QEvent *e);

private:
    Ui::SetActiveDialog *m_ui;

private slots:
    void setActive();
    void sendFetches();
    void joinAge();

signals:
    void setActive(uint32_t player);
    void fetchFound(uint32_t idx);
    void joinAge(ST::string name, plUuid uuid);
};

#endif // SETACTIVEDIALOG_H
