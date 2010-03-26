#include "SetActiveDialog.h"
#include "ui_SetActiveDialog.h"

Q_DECLARE_METATYPE(authPlayer);
Q_DECLARE_METATYPE(QList<hsUint32>)

SetActiveDialog::SetActiveDialog(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::SetActiveDialog)
{
    qRegisterMetaType<QList<hsUint32> >();
    qRegisterMetaType<authPlayer>("authPlayer");
    m_ui->setupUi(this);
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
    foreach(authPlayer plyr, plyrs) {
        QListWidgetItem* player = new QListWidgetItem();
        QVariant data;
        data.setValue(plyr);
        player->setData(Qt::UserRole, data);
        player->setText(QString(plString::Format("%s %s (%u)", plyr.Name.cstr(), plyr.avatar.cstr(), plyr.ID).cstr()));
        m_ui->playerList->addItem(player);
    }
    connect(this, SIGNAL(accepted()), this, SLOT(setActive()));
}

void SetActiveDialog::setFoundNodes(QList<hsUint32> nodes) {
    foreach(hsUint32 node, nodes) {
        QListWidgetItem* item = new QListWidgetItem();
        QVariant data;
        data.setValue(node);
        item->setData(Qt::UserRole, data);
        item->setText(QString(plString::Format("%u", node).cstr()));
        m_ui->playerList->addItem(item);
    }
    setWindowTitle("Fetch Found Nodes");
    m_ui->playerList->setSelectionMode(QAbstractItemView::MultiSelection);
    connect(this, SIGNAL(accepted()), this, SLOT(sendFetches()));
}

void SetActiveDialog::setActive() {
    if(m_ui->playerList->selectedItems().count() == 1)
        emit setActive(m_ui->playerList->selectedItems()[0]->data(Qt::UserRole).value<authPlayer>().ID);
}

void SetActiveDialog::sendFetches() {
    for(int i = 0; i < m_ui->playerList->selectedItems().count(); i++) {
        emit fetchFound(m_ui->playerList->selectedItems()[i]->data(Qt::UserRole).value<hsUint32>());
    }
}
