#include "LoginDialog.h"
#include "SetActiveDialog.h"
#include "FetchDialog.h"
#include "RefDialog.h"
#include "CreateDialog.h"

#include "MoulKI.h"
#include "ui_MoulKI.h"

#include <QLabel>
#include <QFileDialog>

MoulKI::MoulKI(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MoulKIClass), authClient(this)
{
    ui->setupUi(this);
    connect(ui->actionLogin, SIGNAL(triggered()), this, SLOT(showLoginDialog()));
    connect(ui->actionSet_Active, SIGNAL(triggered()), this, SLOT(showPlayers()));
    connect(ui->actionFind_Node, SIGNAL(triggered()), this, SLOT(showFindDialog()));
    connect(ui->actionSubscribe, SIGNAL(triggered()), this, SLOT(showFetchDialog()));
    connect(ui->actionSave_Vault, SIGNAL(triggered()), this, SLOT(writeVault()));
    connect(ui->actionLoad_Vault, SIGNAL(triggered()), this, SLOT(readvault()));
    connect(ui->vaultTree, SIGNAL(itemSelectionChanged()), this, SLOT(setShownNode()));
    connect(ui->applyButton, SIGNAL(clicked()), this, SLOT(saveNodeData()));
    connect(ui->nodeEditor, SIGNAL(isDirty(bool)), this, SLOT(nodeDirty(bool)));

    connect(&authClient, SIGNAL(sigStatus(plString)), this, SLOT(setStatus(plString)));
    connect(&authClient, SIGNAL(loginSuccessful()), this, SLOT(showPlayers()));

    connect(&vault, SIGNAL(addedNode(hsUint32, hsUint32)), this, SLOT(addNode(hsUint32,hsUint32)));
    connect(&vault, SIGNAL(removedNode(hsUint32, hsUint32)), this, SLOT(removeNode(hsUint32,hsUint32)));
    connect(&vault, SIGNAL(gotRootNode(hsUint32)), this, SLOT(addRoot(hsUint32)));
    connect(&vault, SIGNAL(updatedNode(hsUint32)), this, SLOT(updateNode(hsUint32)));

    // create the treeview context menu
    QAction* addRef = new QAction("&Add Child", this);
    connect(addRef, SIGNAL(triggered()), this, SLOT(showRefDialog()));
    QAction* removeRef = new QAction("&Remove", this);
    connect(removeRef, SIGNAL(triggered()), this, SLOT(sendRemove()));
    QAction* createAndRef = new QAction("&Create Node", this);
    connect(createAndRef, SIGNAL(triggered()), this, SLOT(showCreateDialog()));
    ui->vaultTree->setContextMenuPolicy(Qt::ActionsContextMenu);
    ui->vaultTree->addAction(addRef);
    ui->vaultTree->addAction(removeRef);
    ui->vaultTree->addAction(createAndRef);
}

MoulKI::~MoulKI() {
    delete ui;
}

void MoulKI::showLoginDialog() {
    LoginDialog* dialog = new LoginDialog(this);
    connect(dialog, SIGNAL(login(QString,QString)), this, SLOT(login(QString,QString)));
    dialog->exec();
    delete dialog;
}

void MoulKI::login(QString user, QString pass) {
    authClient.startLogin(user, pass);
}

void MoulKI::setStatus(plString msg) {
    ui->statusBar->showMessage(msg.cstr());
}

void MoulKI::showPlayers() {
    // show a player list dialog
    SetActiveDialog* dialog = new SetActiveDialog(this);
    dialog->setPlayers(authClient.players);
    connect(dialog, SIGNAL(setActive(hsUint32)), this, SLOT(setActive(hsUint32)));
    dialog->exec();
    delete dialog;
}

void MoulKI::setActive(hsUint32 playerId) {
    vault.queueRoot(playerId);
    authClient.sendVaultNodeFetch(playerId);
    authClient.sendAcctSetPlayerRequest(playerId);
    activePlayer = playerId;
}

void MoulKI::addRoot(hsUint32 idx) {
    qtVaultNode* node = vault.getNode(idx);
    QTreeWidgetItem* item = node->newItem();
    ui->vaultTree->addTopLevelItem(item);
    ui->vaultTree->expandItem(item);
    if(authClient.isConnected()) {
        authClient.sendVaultFetchNodeRefs(node->getNodeIdx());
    }
}

void MoulKI::addNode(hsUint32 parent, hsUint32 child) {
    qtVaultNode* parentNode = vault.getNode(parent);
    qtVaultNode* childNode = vault.getNode(child);
    foreach(QTreeWidgetItem* item, parentNode->getItems()) {
        // recursively add children items for vault children that already exist on this vault node
        addItemChild(item, childNode);
    }
}

void MoulKI::addItemChild(QTreeWidgetItem* item, qtVaultNode* node) {
    if(!node->tryLock())
        qWarning("Add Node Failure: Recursive Vault Tree");
    if(!itemHasChild(item, node)) {
        QTreeWidgetItem* newItem = node->newItem();
        item->addChild(newItem);
        foreach(qtVaultNode* child, node->getChildren()) {
            addItemChild(newItem, child);
        }
    }
    node->unlockNode();
}

bool MoulKI::itemHasChild(QTreeWidgetItem* item, qtVaultNode* node) {
    for(int i = 0; i < item->childCount(); i++) {
        if(item->child(i)->data(0, Qt::UserRole).value<qtVaultNode*>() == node) {
            return true;
        }
    }
    return false;
}

void MoulKI::removeNode(hsUint32 parent, hsUint32 child) {
    qtVaultNode* parentNode = vault.getNode(parent);
    qtVaultNode* childNode = vault.getNode(child);
    parentNode->lockNode();
    QList<QTreeWidgetItem*> removedItems;
    foreach(QTreeWidgetItem* childItem, childNode->getItems()) {
        if(childItem->parent()->data(0, Qt::UserRole).value<qtVaultNode*>() == parentNode) {
            childItem->parent()->removeChild(childItem);
            removedItems.append(childItem);
        }
    }
    foreach(QTreeWidgetItem* removedItem, removedItems) {
        childNode->removeItem(removedItem);
    }
    parentNode->unlockNode();
}

void MoulKI::updateNode(hsUint32 idx) {
    qtVaultNode* node = vault.getNode(idx);
    // update all the items associated with this node
    foreach(QTreeWidgetItem* item, node->getItems()) {
        item->setText(0, QString(node->displayName().cstr()));
        // if the node is currently being shown, update the display
        if(item->isSelected()) {
            ui->nodeEditor->update();
        }
    }
}

void MoulKI::saveNodeData() {
    ui->applyButton->setEnabled(false);
    qtVaultNode* node = ui->nodeEditor->getNode();
    if(authClient.isConnected()) {
        authClient.sendVaultNodeSave(node->getNodeIdx(), plUuid(), *node);
    }
}

void MoulKI::setShownNode() {
    ui->nodeEditor->setNode(ui->vaultTree->selectedItems()[0]->data(0, Qt::UserRole).value<qtVaultNode*>());
}

void MoulKI::nodeDirty(bool dirty) {
    ui->applyButton->setEnabled(dirty);
}

void MoulKI::showRefDialog() {
    if(ui->vaultTree->selectedItems().count() == 1) {
        qtVaultNode* node = ui->vaultTree->selectedItems()[0]->data(0, Qt::UserRole).value<qtVaultNode*>();
        RefDialog* dialog = new RefDialog(this);
        dialog->setupRefBox(activePlayer, node->getNodeIdx());
        connect(dialog, SIGNAL(addRef(hsUint32,hsUint32,hsUint32)), this, SLOT(sendAdd(hsUint32,hsUint32,hsUint32)));
        dialog->exec();
        delete dialog;
    }
}

void MoulKI::showFetchDialog() {
    FetchDialog* dialog = new FetchDialog(this);
    connect(dialog, SIGNAL(fetchNode(hsUint32)), this, SLOT(fetchTree(hsUint32)));
    dialog->exec();
    delete dialog;
}

void MoulKI::showCreateDialog() {
    if(ui->vaultTree->selectedItems().count() == 1) {
        hsUint32 parent = ui->vaultTree->selectedItems()[0]->data(0, Qt::UserRole).value<qtVaultNode*>()->getNodeIdx();
        CreateDialog* dialog = new CreateDialog(this);
        dialog->setParent(parent);
        connect(dialog, SIGNAL(createSig(pnVaultNode&,hsUint32)), this, SLOT(sendCreate(pnVaultNode&,hsUint32)));
        dialog->exec();
        delete dialog;
    }
}

void MoulKI::showFindDialog() {

}

void MoulKI::fetchTree(hsUint32 idx) {
    vault.queueRoot(idx);
    if(authClient.isConnected())
        authClient.sendVaultNodeFetch(idx);
}

void MoulKI::sendAdd(hsUint32 parent, hsUint32 child, hsUint32 owner) {
    if(authClient.isConnected())
        authClient.sendVaultNodeAdd(parent, child, owner);
}

void MoulKI::sendCreate(pnVaultNode& node, hsUint32 parent) {
    if(authClient.isConnected())
        authClient.queueRef(authClient.sendVaultNodeCreate(node), parent);
}

void MoulKI::sendRemove() {
    QTreeWidgetItem* item = ui->vaultTree->selectedItems()[0];
    qtVaultNode* parent = item->parent()->data(0, Qt::UserRole).value<qtVaultNode*>();
    qtVaultNode* child = item->data(0, Qt::UserRole).value<qtVaultNode*>();
    authClient.sendVaultNodeRemove(parent->getNodeIdx(), child->getNodeIdx());
}

void MoulKI::writeVault() {
    QString fileName = QFileDialog::getSaveFileName(this, "Save Vault", "./", "*.vault");
    if(!fileName.isEmpty()) {
        hsFileStream file(pvLive);
        file.open(fileName.toAscii().data(), fmWrite);
        int rootCount = ui->vaultTree->topLevelItemCount();
        file.writeInt(rootCount);
        for(int i = 0; i < rootCount; i++) {
            file.writeInt(ui->vaultTree->topLevelItem(i)->data(0, Qt::UserRole).value<qtVaultNode*>()->getNodeIdx());
        }
        vault.writeVault(file);
        file.close();
    }
}

void MoulKI::readvault() {
    QString fileName = QFileDialog::getOpenFileName(this, "Load Vault", "./", "*.vault");
    if(!fileName.isEmpty()) {
        hsFileStream file(pvLive);
        file.open(fileName.toAscii().data(), fmRead);
        int rootCount = file.readInt();
        for(int i = 0; i < rootCount; i++) {
            hsUint32 root = file.readInt();
            vault.queueRoot(root);
            qWarning("Read root: %u", root);
        }
        vault.readVault(file);
        file.close();
    }
}
