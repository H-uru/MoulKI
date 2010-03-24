#include "SetActiveDialog.h"
#include "ui_SetActiveDialog.h"

Q_DECLARE_METATYPE(authPlayer);

SetActiveDialog::SetActiveDialog(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::SetActiveDialog)
{
    qRegisterMetaType<authPlayer>("authPlayer");
    m_ui->setupUi(this);
    connect(this, SIGNAL(accepted()), this, SLOT(setActive()));
}

SetActiveDialog::~SetActiveDialog()
{
    delete m_ui;
}

void SetActiveDialog::changeEvent(QEvent *e)
{
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void SetActiveDialog::setPlayers(QList<authPlayer> plyrs) {
    players = plyrs;
    foreach(authPlayer plyr, plyrs) {
        QListWidgetItem* player = new QListWidgetItem();
        QVariant data;
        data.setValue(plyr);
        player->setData(Qt::UserRole, data);
        player->setText(QString(plString::Format("%s %s (%u)", plyr.Name.cstr(), plyr.avatar.cstr(), plyr.ID).cstr()));
        m_ui->playerList->addItem(player);
    }
}

void SetActiveDialog::setActive() {
    if(m_ui->playerList->selectedItems().count() == 1)
        emit setActive(m_ui->playerList->selectedItems()[0]->data(Qt::UserRole).value<authPlayer>().ID);
}
