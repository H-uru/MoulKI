#include "LoginDialog.h"
#include "SetActiveDialog.h"
#include "FetchDialog.h"
#include "RefDialog.h"
#include "CreateNodeDialog.h"

#include "MoulKI.h"
#include "ui_MoulKI.h"

#include <QLabel>
#include <QFileDialog>

Q_DECLARE_METATYPE(plUuid)
Q_DECLARE_METATYPE(plString)
Q_DECLARE_METATYPE(hsUint32)

MoulKI::MoulKI(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MoulKIClass), gameClient(NULL)
{
    ui->setupUi(this);

    qRegisterMetaType<plUuid>("plUuid");
    qRegisterMetaType<plString>("plString");
    qRegisterMetaType<hsUint32>("hsUint32");

    connect(ui->actionLogin, SIGNAL(triggered()), this, SLOT(showLoginDialog()));
    connect(ui->actionSet_Active, SIGNAL(triggered()), this, SLOT(showPlayers()));
    connect(ui->actionFind_Node, SIGNAL(triggered()), this, SLOT(showFindDialog()));
    connect(ui->actionSubscribe, SIGNAL(triggered()), this, SLOT(showFetchDialog()));
    connect(ui->actionSave_Vault, SIGNAL(triggered()), this, SLOT(writeVault()));
    connect(ui->actionLoad_Vault, SIGNAL(triggered()), this, SLOT(readVault()));
    connect(ui->actionJoin_Age, SIGNAL(triggered()), this, SLOT(showJoinAgeDialog()));
    connect(ui->vaultTree, SIGNAL(itemSelectionChanged()), this, SLOT(setShownNode()));
    connect(ui->applyButton, SIGNAL(clicked()), this, SLOT(saveNodeData()));
    connect(ui->revertButton, SIGNAL(clicked()), this, SLOT(revertNode()));
    connect(ui->nodeEditor, SIGNAL(isDirty(bool)), this, SLOT(nodeDirty(bool)));
    connect(ui->vaultTree, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showItemContextMenu(QPoint)));
    connect(ui->chatEntry, SIGNAL(returnPressed()), this, SLOT(sendGameChat()));

    authClient = new qtAuthClient(this);
    connect(authClient, SIGNAL(sigStatus(plString)), this, SLOT(setStatus(plString)));
    connect(authClient, SIGNAL(loginSuccessful()), this, SLOT(showPlayers()));
    connect(authClient, SIGNAL(foundNodes(int,QList<hsUint32>)), this, SLOT(showFoundDialog(int,QList<hsUint32>)));
    connect(authClient, SIGNAL(gotAge(hsUint32,plUuid,hsUint32,hsUint32)), this, SLOT(startGameServer(hsUint32,plUuid,hsUint32,hsUint32)));

    connect(&vault, SIGNAL(addedNode(hsUint32, hsUint32)), this, SLOT(addNode(hsUint32,hsUint32)));
    connect(&vault, SIGNAL(removedNode(hsUint32, hsUint32)), this, SLOT(removeNode(hsUint32,hsUint32)));
    connect(&vault, SIGNAL(gotRootNode(hsUint32)), this, SLOT(addRoot(hsUint32)));
    connect(&vault, SIGNAL(updatedNode(hsUint32)), this, SLOT(updateNode(hsUint32)));
    connect(&vault, SIGNAL(fetchComplete()), this, SLOT(checkCurrentAge()));

    ui->vaultTree->setContextMenuPolicy(Qt::CustomContextMenu);

    // set up the player list
    agePlayersItem = new QTreeWidgetItem(ui->playersTree);
    agePlayersItem->setText(0, "AGE PLAYERS");
    ui->playersTree->insertTopLevelItem(0, agePlayersItem);
    agePlayersItem->setExpanded(true);
    buddiesItem = new QTreeWidgetItem(ui->playersTree);
    buddiesItem->setText(0, "BUDDIES");
    buddiesItem->setExpanded(true);
    ui->playersTree->insertTopLevelItem(0, buddiesItem);
    neighborsItem = new QTreeWidgetItem(ui->playersTree);
    neighborsItem->setText(0, "NEIGHBORS");
    ui->playersTree->insertTopLevelItem(0, neighborsItem);
    neighborsItem->setExpanded(true);
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
    authClient->startLogin(user, pass);
}

void MoulKI::setStatus(plString msg) {
    ui->statusBar->showMessage(msg.cstr());
}

void MoulKI::showPlayers() {
    // show a player list dialog
    SetActiveDialog* dialog = new SetActiveDialog(this);
    dialog->setPlayers(authClient->players);
    connect(dialog, SIGNAL(setActive(hsUint32)), this, SLOT(setActive(hsUint32)));
    dialog->exec();
    delete dialog;
}

void MoulKI::setActive(hsUint32 playerId) {
    activePlayer = playerId;
    buddyListFolder = 0;
    buddyInfoIds.clear();
    clearBuddyList();
    // pre-fetch buddy data if it already exists
    if(vault.hasNode(playerId)) {
        qtVaultNode* playerChild = vault.getNode(playerId)->getBuddiesFolder();
        if(playerChild != NULL) {
            buddyListFolder = playerChild->getNodeIdx();
            foreach(qtVaultNode* buddyInfo, playerChild->getChildren()) {
                buddyInfoIds.append(buddyInfo->getNodeIdx());
                addRemoveBuddyItem(buddyInfo);
            }
        }
    }
    // return to normally scheduled operations
    vault.queueRoot(playerId);
    authClient->sendVaultNodeFetch(playerId);
    authClient->sendAcctSetPlayerRequest(playerId);
}

void MoulKI::showItemContextMenu(QPoint pos) {
    // create the treeview context menu
    QTreeWidgetItem* item = ui->vaultTree->itemAt(pos);
    if(!item)
        return;
    qtVaultNode* node = item->data(0, Qt::UserRole).value<qtVaultNode*>();
    QMenu* menu = new QMenu(this);
    QAction* addRef = new QAction("&Add Child", menu);
    connect(addRef, SIGNAL(triggered()), this, SLOT(showRefDialog()));
    QAction* removeRef = new QAction("&Remove", menu);
    connect(removeRef, SIGNAL(triggered()), this, SLOT(sendRemove()));
    QAction* createAndRef = new QAction("&Create Node", menu);
    connect(createAndRef, SIGNAL(triggered()), this, SLOT(showCreateDialog()));
    menu->addAction(addRef);
    menu->addAction(removeRef);
    menu->addAction(createAndRef);
    if(node->getNodeType() == plVault::kNodePlayerInfo || node->getNodeType() == plVault::kNodeAgeInfo) {
        menu->addSeparator();
        QAction* subscribe = new QAction("&Subscribe", menu);
        connect(subscribe, SIGNAL(triggered()), this, SLOT(subscribe()));
        menu->addAction(subscribe);
    }
    if(node->getNodeType() == plVault::kNodeAgeInfo || (node->getNodeType() == plVault::kNodePlayerInfo && node->getInt32(0) == 1)) {
        QAction* join = new QAction("&Join", menu);
        connect(join, SIGNAL(triggered()), this, SLOT(joinSelectedAge()));
        menu->addAction(join);
    }
    menu->popup(ui->vaultTree->mapToGlobal(pos), addRef);
}

void MoulKI::addRoot(hsUint32 idx) {
    qtVaultNode* node = vault.getNode(idx);
    QTreeWidgetItem* item = node->newItem();
    ui->vaultTree->addTopLevelItem(item);
    ui->vaultTree->expandItem(item);
    node->lockNode();
    foreach(qtVaultNode* childNode, node->getChildren()) {
        // root nodes may already have child refs
        // duplicate refs will not emit signals when added, so we need to add
        // preexisting children now
        addItemChild(item, childNode);
    }
    node->unlockNode();
    if(authClient->isConnected()) {
        authClient->sendVaultFetchNodeRefs(node->getNodeIdx());
    }
}

void MoulKI::addNode(hsUint32 parent, hsUint32 child) {
    qtVaultNode* parentNode = vault.getNode(parent);
    qtVaultNode* childNode = vault.getNode(child);
    // a new ref has been added, find all the parent items, and add the new child to all of them
    // if the child has existing children, they will be recursively added
    foreach(QTreeWidgetItem* item, parentNode->getItems()) {
        // recursively add children items for vault children that already exist on this vault node
        addItemChild(item, childNode);
    }
    // Now we hook this to handle buddy and Neighborhood owner releated stuff
    if(parent == activePlayer && childNode->getNodeType() == plVault::kNodePlayerInfoList && childNode->getInt32(0) == plVault::kBuddyListFolder) {
        buddyListFolder = child;
    }else if(parent == buddyListFolder) {
        buddyInfoIds.append(child);
        addRemoveBuddyItem(childNode);
    }
}

void MoulKI::addItemChild(QTreeWidgetItem* item, qtVaultNode* node) {
    // this creates a child item from node and adds it to the given item.
    // nodes in the subtree are locked to prevent children from being added
    if(!node->tryLock())
        qWarning("Add Node Failure: Recursive Vault Tree");
    if(!itemHasChild(item, node)) {
        QTreeWidgetItem* newItem = node->newItem();
        item->addChild(newItem);
        foreach(qtVaultNode* child, node->getChildren()) {
            // items thus added also need to have their subchildren added
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
    childNode->lockNode();
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
    childNode->unlockNode();
    // now we hook this for buddy updates
    if(buddyInfoIds.contains(child)) {
        buddyInfoIds.removeAll(child);
        addRemoveBuddyItem(childNode, 1);
    }
}

void MoulKI::updateNode(hsUint32 idx) {
    qtVaultNode* node = vault.getNode(idx);
    // update all the items associated with this node
    foreach(QTreeWidgetItem* item, node->getItems()) {
        item->setText(0, QString(node->displayName().cstr()));
        item->setIcon(0, node->getIcon());
        if(node->getNodeType() == plVault::kNodePlayerInfo)
            updateNode(node->getUint32(0));
        // if the node is currently being shown, update the display
        if(item->isSelected()) {
            ui->nodeEditor->update();
        }
    }
    // now we hook this for buddy updates
    if(buddyInfoIds.contains(idx)) {
        addRemoveBuddyItem(node);
    }
}

void MoulKI::saveNodeData() {
    ui->applyButton->setEnabled(false);
    qtVaultNode* node = ui->nodeEditor->getNode();
    if(authClient->isConnected()) {
        authClient->sendVaultNodeSave(node->getNodeIdx(), plUuid(), *node);
    }
}

void MoulKI::revertNode() {
    ui->revertButton->setEnabled(false);
    qtVaultNode* node = ui->nodeEditor->getNode();
    if(authClient->isConnected()) {
        authClient->sendVaultNodeFetch(node->getNodeIdx());
    }
}

void MoulKI::setShownNode() {
    ui->nodeEditor->setNode(ui->vaultTree->selectedItems()[0]->data(0, Qt::UserRole).value<qtVaultNode*>());
}

void MoulKI::subscribe() {
    QTreeWidgetItem* item = ui->vaultTree->selectedItems()[0];
    qtVaultNode* child = item->data(0, Qt::UserRole).value<qtVaultNode*>();
    fetchTree(child->getCreatorIdx());
}

void MoulKI::nodeDirty(bool dirty) {
    ui->applyButton->setEnabled(dirty);
    ui->revertButton->setEnabled(dirty);
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
        CreateNodeDialog* dialog = new CreateNodeDialog(this);
        dialog->setParent(parent);
        connect(dialog, SIGNAL(createSig(pnVaultNode&,hsUint32)), this, SLOT(sendCreate(pnVaultNode&,hsUint32)));
        dialog->exec();
        delete dialog;
    }
}

void MoulKI::showFindDialog() {
    CreateNodeDialog* dialog = new CreateNodeDialog(this);
    dialog->setActFind();
    connect(dialog, SIGNAL(findSig(pnVaultNode&)), this, SLOT(sendFind(pnVaultNode&)));
    dialog->exec();
    delete dialog;
}

void MoulKI::showFoundDialog(int count, QList<hsUint32> nodes) {
    SetActiveDialog* dialog = new SetActiveDialog(this);
    dialog->setFoundNodes(nodes);
    connect(dialog, SIGNAL(fetchFound(hsUint32)), this, SLOT(fetchTree(hsUint32)));
    dialog->exec();
    delete dialog;
}

void MoulKI::fetchTree(hsUint32 idx) {
    vault.queueRoot(idx);
    if(authClient->isConnected())
        authClient->sendVaultNodeFetch(idx);
}

void MoulKI::sendAdd(hsUint32 parent, hsUint32 child, hsUint32 owner) {
    if(authClient->isConnected())
        authClient->sendVaultNodeAdd(parent, child, owner);
}

void MoulKI::sendCreate(pnVaultNode& node, hsUint32 parent) {
    if(authClient->isConnected())
        authClient->queueRef(authClient->sendVaultNodeCreate(node), parent);
}

void MoulKI::sendFind(pnVaultNode& node) {
    if(authClient->isConnected())
        authClient->sendVaultNodeFind(node);
}

void MoulKI::showJoinAgeDialog() {
    // this should really grab all the ageinfos and pop up the listbox dialog
    QList<qtVaultNode*> ages;
    foreach(qtVaultNode* folder, vault.getNode(activePlayer)->getChildren()) {
        if(folder->getNodeType() == plVault::kNodeAgeInfoList && folder->getInt32(0) == plVault::kAgesIOwnFolder) {
            foreach(qtVaultNode* ageLink, folder->getChildren()) {
                ages.append(ageLink->getChildren()[0]); // ageLinks should only have one child
            }
        }
    }
    SetActiveDialog* dialog = new SetActiveDialog(this);
    connect(dialog, SIGNAL(joinAge(plString,plUuid)), this, SLOT(joinAge(plString,plUuid)));
    dialog->setAgeNodes(ages);
    dialog->exec();
    delete dialog;
}

void MoulKI::joinAge(plString name, plUuid uuid) {
    currentAgeName = name;
    authClient->sendAgeRequest(name, uuid);
}

void MoulKI::joinSelectedAge() {
    if(ui->vaultTree->selectedItems().count() == 1) {
        qtVaultNode* node = ui->vaultTree->selectedItems()[0]->data(0, Qt::UserRole).value<qtVaultNode*>();
        if(node->getNodeType() == plVault::kNodeAgeInfo) {
            authClient->sendAgeRequest(node->getString64(1), node->getUuid(0));
        }else if(node->getNodeType() == plVault::kNodePlayerInfo && !node->getUuid(0).isNull()) {
            authClient->sendAgeRequest(node->getString64(0), node->getUuid(0));
        }
    }
}

void MoulKI::setOnline(hsUint32 playerId, plString ageFilename) {
    qtVaultNode* playerNode = vault.getNode(playerId);
    foreach(qtVaultNode* node, playerNode->getChildren()) {
        if(node->getNodeType() == plVault::kNodePlayerInfo) {
            node->setInt32(0, 1);
            node->setString64(0, ageFilename);
            authClient->sendVaultNodeSave(node->getNodeIdx(), plUuid(), *node);
            break;
        }
    }
}

void MoulKI::startGameServer(hsUint32 serverAddr, plUuid ageId, hsUint32 mcpId, hsUint32 ageVaultId) {
    currentAgeId = ageVaultId;
    fetchTree(ageVaultId); // fetch the age Vault tree, because the client does, and we will get updates
    qtVaultNode* player = vault.getNode(activePlayer);
    if(gameClient != NULL)
        delete gameClient;
    gameClient = new qtGameClient(this);
    connect(gameClient, SIGNAL(receivedGameMsg(QString)), this, SLOT(addChatLine(QString)));
    connect(gameClient, SIGNAL(setMeOnline(hsUint32,plString)), this, SLOT(setOnline(hsUint32,plString)));
    connect(gameClient, SIGNAL(clearAgeList()), this, SLOT(clearAgeList()));
    connect(gameClient, SIGNAL(addAgePlayer(hsUint32,plString)), this, SLOT(addAgePlayer(hsUint32,plString)));
    connect(gameClient, SIGNAL(removeAgePlayer(hsUint32,plString)), this, SLOT(removeAgePlayer(hsUint32,plString)));
    gameClient->setPlayer(player);
    if(vault.hasNode(ageVaultId)) { // this will only be true if we've already been to the age this session
        qtVaultNode* info = vault.getNode(ageVaultId)->getAgeInfoNode();
        if(info != NULL) {
            gameClient->setAgeInfo(info);
        }
    }
    gameClient->setJoinInfo(player->getUuid(0), ageId);
    gameClient->joinAge(serverAddr, mcpId, currentAgeName);
}

void MoulKI::checkCurrentAge() {
    if(vault.hasNode(currentAgeId)) {
        qtVaultNode* info = vault.getNode(currentAgeId)->getAgeInfoNode();
        if(info != NULL && gameClient != NULL) {
            gameClient->setAgeInfo(info);
        }
    }
}

void MoulKI::sendRemove() {
    QTreeWidgetItem* item = ui->vaultTree->selectedItems()[0];
    qtVaultNode* child = item->data(0, Qt::UserRole).value<qtVaultNode*>();
    if(item->parent()) {
        qtVaultNode* parent = item->parent()->data(0, Qt::UserRole).value<qtVaultNode*>();
        authClient->sendVaultNodeRemove(parent->getNodeIdx(), child->getNodeIdx());
    }else{
        // if there's no parent node, then it's a treeview root, so let's clean up
        removeTreeNodes(item, child);
        delete ui->vaultTree->takeTopLevelItem(ui->vaultTree->indexOfTopLevelItem(item));
    }
}

void MoulKI::removeTreeNodes(QTreeWidgetItem* item, qtVaultNode* node) {
    for(int i = 0; i < item->childCount(); i++) {
        removeTreeNodes(item->child(i), item->child(i)->data(0, Qt::UserRole).value<qtVaultNode*>());
    }
    node->removeItem(item);
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

void MoulKI::readVault() {
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

void MoulKI::addChatLine(QString line) {
    QTextCursor cursor = ui->chatPane->textCursor();
    cursor.movePosition(QTextCursor::End);
    cursor.insertText(line);
}

void MoulKI::sendGameChat() {
    if(gameClient == NULL)
        return;
    plString line = plString(ui->chatEntry->text().toAscii().data());
    if(ui->playersTree->selectedItems().count() == 1) {
        QTreeWidgetItem* item = ui->playersTree->selectedItems()[0];
        QVariant userData = item->data(0, Qt::UserRole);
        if(userData.canConvert<hsUint32>()) {
            gameClient->sendPrivate(line, userData.value<hsUint32>());
            addChatLine(plString::Format("To %s: %s\n", item->text(0).toAscii().data(), line.cstr()).cstr());
        }else{
            gameClient->sendAgeChat(line);
            addChatLine(plString::Format("%s: %s\n", vault.getNode(activePlayer)->getIString64(0).cstr(), line.cstr()).cstr());
        }
    }else{
        gameClient->sendAgeChat(line);
        addChatLine(plString::Format("%s: %s\n", vault.getNode(activePlayer)->getIString64(0).cstr(), line.cstr()).cstr());
    }
    ui->chatEntry->clear();
}

void MoulKI::addAgePlayer(hsUint32 playerId, plString playerName) {
    QTreeWidgetItem* item = new QTreeWidgetItem();
    item->setText(0, QString(playerName.cstr()));
    item->setData(0, Qt::UserRole, QVariant(playerId));
    agePlayersItem->addChild(item);
}

void MoulKI::removeAgePlayer(hsUint32 playerId, plString playerName) {
    for(int i = 0; i < agePlayersItem->childCount(); i++) {
        if(agePlayersItem->child(i)->data(0, Qt::UserRole).value<hsUint32>() == playerId) {
            agePlayersItem->removeChild(agePlayersItem->child(i));
        }
    }
}

void MoulKI::clearAgeList() {
    while(agePlayersItem->childCount() > 0) {
        agePlayersItem->removeChild(agePlayersItem->child(0));
    }
}

void MoulKI::clearBuddyList() {
    while(buddiesItem->childCount() > 0) {
        buddiesItem->removeChild(buddiesItem->child(0));
    }
}

void MoulKI::addRemoveBuddyItem(qtVaultNode* infoNode, bool remove) {
    if(infoNode->getInt32(0) == 0 || remove) {
        for(int i = 0; i < buddiesItem->childCount(); i++) {
            if(buddiesItem->child(i)->data(0, Qt::UserRole).value<hsUint32>() == infoNode->getUint32(0)) {
                buddiesItem->removeChild(buddiesItem->child(i));
                break;
            }
        }
    }else{
        if(!buddyTreeContains(infoNode->getUint32(0))) {
            QTreeWidgetItem* item = new QTreeWidgetItem();
            item->setText(0, infoNode->getIString64(0).cstr());
            item->setData(0, Qt::UserRole, QVariant(infoNode->getUint32(0)));
            buddiesItem->addChild(item);
        }
    }
}

bool MoulKI::buddyTreeContains(hsUint32 playerId) {
    for(int i = 0; i < buddiesItem->childCount(); i++) {
        if(buddiesItem->child(i)->data(0, Qt::UserRole).value<hsUint32>() == playerId) {
            return true;
        }
    }
    return false;
}
