#include "LoginDialog.h"
#include "SetActiveDialog.h"
#include "FetchDialog.h"
#include "RefDialog.h"

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
    connect(ui->actionSubscribe, SIGNAL(triggered()), this, SLOT(showFetchDialog()));
    connect(ui->actionSave_Vault, SIGNAL(triggered()), this, SLOT(writeVault()));
    connect(ui->actionLoad_Vault, SIGNAL(triggered()), this, SLOT(readvault()));
    connect(ui->vaultTree, SIGNAL(itemSelectionChanged()), this, SLOT(showNodeData()));

    connect(ui->nodeData, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(editNodeData(QTableWidgetItem*)));
    connect(ui->applyButton, SIGNAL(clicked()), this, SLOT(saveNodeData()));
    connect(ui->loadImageButton, SIGNAL(clicked()), this, SLOT(loadNodeImage()));
    connect(ui->saveImageButton, SIGNAL(clicked()), this, SLOT(saveNodeImage()));
    connect(ui->textNodeTitle, SIGNAL(textEdited(QString)), this, SLOT(editNodeTitle(QString)));
    connect(ui->imageNodeTitle, SIGNAL(textEdited(QString)), this, SLOT(editNodeTitle(QString)));

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
    connect(createAndRef, SIGNAL(triggered()), this, SLOT(createBlankNode()));
    ui->vaultTree->setContextMenuPolicy(Qt::ActionsContextMenu);
    ui->vaultTree->addAction(addRef);
    ui->vaultTree->addAction(removeRef);
    ui->vaultTree->addAction(createAndRef);

    // disable aux tabs
    ui->nodeDataArea->setTabEnabled(1, false);
    ui->nodeDataArea->setTabEnabled(2, false);
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
        /*
        // this is enough for a single vault, but multiple vaults loose duplicate refs
        QTreeWidgetItem* item = childNode->newItem();
        parentNode->items[i]->addChild(item);
        */
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
            showNodeData();
        }
    }
}

void MoulKI::editNodeData(QTableWidgetItem* item) {
    qWarning("Node data changed");
    qtVaultNode* node = ui->vaultTree->selectedItems()[0]->data(0, Qt::UserRole).value<qtVaultNode*>();
    int field = item->row();
    node->setFieldFromString(field, plString(item->text().toUtf8().data()));
    showNodeData();
}

void MoulKI::editNodeTitle(QString title) {
    /* really need to come up with a better way to do this
    qtVaultNode* node = ui->vaultTree->selectedItems()[0]->data(0, Qt::UserRole).value<qtVaultNode*>();
    node->setString64(0, plString(title.toUtf8().data()));
    ui->applyButton->setEnabled(true);
    showNodeData();
    */
}

void MoulKI::editNodeText() {
    /* really need to come up with a better way to do this
    qtVaultNode* node = ui->vaultTree->selectedItems()[0]->data(0, Qt::UserRole).value<qtVaultNode*>();
    node->setString64(0, plString(ui->textNodeEdit->document()->toPlainText().toUtf8().data()));
    ui->applyButton->setEnabled(true);
    showNodeData();
    */
}

void MoulKI::saveNodeImage() {
    qtVaultNode* node = ui->vaultTree->selectedItems()[0]->data(0, Qt::UserRole).value<qtVaultNode*>();
    node->lockNode();
    QString fileName = QFileDialog::getSaveFileName(this, "Save Image", node->getString64(0).cstr(), "*.jpg *.jpeg");
    QFile outFile(fileName);
    outFile.open(QIODevice::WriteOnly);
    outFile.write((const char*)node->getBlob(0).getData() + 4, node->getBlob(0).getSize() - 4);
    outFile.close();
    node->unlockNode();
}

void MoulKI::loadNodeImage() {
    qtVaultNode* node = ui->vaultTree->selectedItems()[0]->data(0, Qt::UserRole).value<qtVaultNode*>();
    node->lockNode();
    QString fileName = QFileDialog::getOpenFileName(this, "Load Image", "./", "*.jpg *.jpeg");
    QFile inFile(fileName);
    inFile.open(QIODevice::ReadOnly);
    QByteArray data = inFile.readAll();
    inFile.close();
    hsUint32 len = data.length();
    char* dataPtr = new char[len + 4];
    *(hsUint32*)dataPtr = len;
    memcpy(data.data(), dataPtr + 4, len);
    plVaultBlob blob;
    blob.setData(len + 4, (const unsigned char*)dataPtr);
    node->setBlob(0, blob);
    node->unlockNode();
    delete[] dataPtr;
    showNodeData();
}

void MoulKI::showNodeData() {
    disconnect(ui->nodeData, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(editNodeData(QTableWidgetItem*)));
    ui->applyButton->setEnabled(false);
    ui->nodeData->clearContents();
    if(ui->vaultTree->selectedItems().count() == 1) {
        qtVaultNode* node = ui->vaultTree->selectedItems()[0]->data(0, Qt::UserRole).value<qtVaultNode*>();
        node->lockNode();
        ui->nodeData->setEnabled(true);
        for(int i = 0; i < qtVaultNode::kNumFields; i++) {
            QTableWidgetItem* fitem = new QTableWidgetItem();
            fitem->setText(QString(node->fieldName(i).cstr()));
            fitem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
            QTableWidgetItem* citem = new QTableWidgetItem();
            citem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
            citem->setText(QString(node->getFieldAsString(i).cstr()));
            if(node->hasDirty(i)) {
                QColor rowColor(200, 127, 127);
                fitem->setBackground(QBrush(rowColor));
                citem->setBackground(QBrush(rowColor));
                ui->applyButton->setEnabled(true);
            }
            ui->nodeData->setItem(i, 0, fitem);
            ui->nodeData->setItem(i, 1, citem);
        }
        // set the type-specific content
        switch(node->getNodeType()) {
            case 25: // Image
                {
                    if(node->getBlob(0).getSize()) {
                        QImage image = QImage::fromData(node->getBlob(0).getData() + 4, (int)node->getBlob(0).getSize() - 4, "JPEG");
                        qWarning("Image Size: %u, %u", image.width(), image.height());
                        ui->imageLabel->setPixmap(QPixmap::fromImage(image));
                    }else{
                        qWarning("No Image in Image Node");
                        ui->imageLabel->setPixmap(QPixmap());
                    }
                    ui->imageLabel->setMaximumSize(320, 240);
                    ui->imageNodeTitle->setText(QString(node->getString64(0).cstr()));
                    ui->nodeDataArea->setTabEnabled(1, true);
                    ui->nodeDataArea->setTabEnabled(2, false);
                }
                break;
            case 26: // Text
                disconnect(ui->textNodeEdit, SIGNAL(textChanged()), this, SLOT(editNodeText()));
                ui->textNodeEdit->setPlainText(QString(node->getText(0)));
                ui->textNodeTitle->setText(QString(node->getString64(0)));
                ui->nodeDataArea->setTabEnabled(1, false);
                ui->nodeDataArea->setTabEnabled(2, true);
                connect(ui->textNodeEdit, SIGNAL(textChanged()), this, SLOT(editNodeText()));
                break;
            default:
                ui->nodeDataArea->setTabEnabled(1, false);
                ui->nodeDataArea->setTabEnabled(2, false);
                break;
        }
        node->unlockNode();
    }else{
        ui->nodeData->setEnabled(false);
    }
    connect(ui->nodeData, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(editNodeData(QTableWidgetItem*)));
}

void MoulKI::saveNodeData() {
    ui->applyButton->setEnabled(false);
    qtVaultNode* node = ui->vaultTree->selectedItems()[0]->data(0, Qt::UserRole).value<qtVaultNode*>();
    authClient.sendVaultNodeSave(node->getNodeIdx(), plUuid(), *node);
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

void MoulKI::fetchTree(hsUint32 idx) {
    vault.queueRoot(idx);
    authClient.sendVaultNodeFetch(idx);
}

void MoulKI::sendAdd(hsUint32 parent, hsUint32 child, hsUint32 owner) {
    authClient.sendVaultNodeAdd(parent, child, owner);
}

void MoulKI::sendCreate(pnVaultNode& node, hsUint32 parent) {
    authClient.queueRef(authClient.sendVaultNodeCreate(node), parent);
}

void MoulKI::sendRemove() {
    QTreeWidgetItem* item = ui->vaultTree->selectedItems()[0];
    qtVaultNode* parent = item->parent()->data(0, Qt::UserRole).value<qtVaultNode*>();
    qtVaultNode* child = item->data(0, Qt::UserRole).value<qtVaultNode*>();
    authClient.sendVaultNodeRemove(parent->getNodeIdx(), child->getNodeIdx());
}

void MoulKI::createBlankNode() {
    if(ui->vaultTree->selectedItems().count() == 1) {
        pnVaultNode node;
        node.setNodeType(0);
        sendCreate(node, ui->vaultTree->selectedItems()[0]->data(0, Qt::UserRole).value<qtVaultNode*>()->getNodeIdx());
    }
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
