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

protected:
    virtual void changeEvent(QEvent *e);

private:
    Ui::SetActiveDialog *m_ui;
    QList<authPlayer> players;

private slots:
    void setActive();

signals:
    void setActive(hsUint32 player);
};

#endif // SETACTIVEDIALOG_H
