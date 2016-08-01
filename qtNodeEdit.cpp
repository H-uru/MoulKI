#include "qtNodeEdit.h"
#include "ui_qtNodeEdit.h"
#include "qtSDLTreeModel.h"

#include <QFile>
#include <QComboBox>
#include <QFileDialog>
#include <QTableWidgetItem>
#include <SDL/plSDLMgr.h>
#include <SDL/plStateDataRecord.h>
#include <Stream/hsRAMStream.h>

qtNodeEdit::qtNodeEdit(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::qtNodeEdit),
    typeBox(new NodeTypeDelegate(this)),
    folderBox(new FolderTypeDelegate(this))
{
    ui->setupUi(this);
    connect(ui->nodeData, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(dataRowChanged(QTableWidgetItem*)));
    connect(ui->loadImageButton, SIGNAL(clicked()), this, SLOT(loadNodeImage()));
    connect(ui->saveImageButton, SIGNAL(clicked()), this, SLOT(saveNodeImage()));
    connect(ui->textNodeTitle, SIGNAL(textEdited(QString)), this, SLOT(editNodeTitle(QString)));
    connect(ui->imageNodeTitle, SIGNAL(textEdited(QString)), this, SLOT(editNodeTitle(QString)));
    connect(ui->nodeDataArea, SIGNAL(currentChanged(int)), this, SLOT(tabActivated(int)));

    // disable aux tabs
    ui->nodeDataArea->setTabEnabled(1, false);
    ui->nodeDataArea->setTabEnabled(2, false);
    ui->nodeDataArea->setTabEnabled(3, false);

    // set header behavior
    ui->SDLTreeView->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
}

qtNodeEdit::~qtNodeEdit() {
    delete ui;
}

void qtNodeEdit::changeEvent(QEvent *e) {
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void qtNodeEdit::setNode(qtVaultNode *pn) {
    textEdited = false;
    titleEdited = false;
    node = pn;
    update();
}

void qtNodeEdit::setMgrs(plSDLMgr* sdl, plResManager* res) {
    sdlmgr = sdl;
    resmgr = res;
}

void qtNodeEdit::dataRowChanged(QTableWidgetItem* item) {
    qWarning("Node data changed");
    int field = item->row();
    node->setFieldFromString(field, plString(item->text().toUtf8().data()));
    update();
}

void qtNodeEdit::editNodeTitle(QString title) {
    newTitle = title;
    titleEdited = true;
}

void qtNodeEdit::editNodeText() {
    textEdited = true;
}

void qtNodeEdit::tabActivated(int) {
    if(titleEdited)
        node->setString64(0, plString(newTitle.toUtf8().data()));
    if(textEdited)
        node->setText(0, plString(ui->textNodeEdit->document()->toPlainText().toUtf8().data()));
    if(titleEdited || textEdited)
        update();
    titleEdited = false;
    textEdited = false;
}

void qtNodeEdit::saveNodeImage() {
    plString extension, filter;
    int offset = 0;

    node->lockNode();
    switch(node->getInt32(0)) {
        case plVault::kJPEG:
            extension = ".jpg";
            filter = "*.jpg *.jpeg";
            offset = 4;
            break;
        case plVault::kPNG:
            extension = ".png";
            filter = "*.png";
            break;
    }
    QString fileName = QFileDialog::getSaveFileName(this, "Save Image", (node->getString64(0) + extension).cstr(), filter.cstr());
    QFile outFile(fileName);
    outFile.open(QIODevice::WriteOnly);
    outFile.write((const char*)node->getBlob(0).getData() + offset, node->getBlob(0).getSize() - offset);
    outFile.close();
    node->unlockNode();
}

void qtNodeEdit::loadNodeImage() {
    QString fileName = QFileDialog::getOpenFileName(this, "Load Image", "./", "*.jpg *.jpeg *.png");
    if(fileName.isEmpty()) {
        return;
    }
    int imageType = plVault::kNone;
    int offset = 0;
    if(fileName.endsWith(".jpg") || fileName.endsWith(".jpeg")) {
        imageType = plVault::kJPEG;
        offset = 4;
    } else if (fileName.endsWith(".png"))
        imageType = plVault::kPNG;
    else
        return;
    node->lockNode();
    QFile inFile(fileName);
    inFile.open(QIODevice::ReadOnly);
    QByteArray data = inFile.readAll();
    inFile.close();
    uint32_t len = data.length();
    char* dataPtr = new char[len + offset];
    if(imageType == plVault::kJPEG)
        *(uint32_t*)dataPtr = len;
    memcpy(dataPtr + offset, data.data(), len);
    plVaultBlob blob;
    blob.setData(len + offset, (const unsigned char*)dataPtr);
    node->setBlob(0, blob);
    node->setInt32(0, imageType);
    node->unlockNode();
    delete[] dataPtr;
    update();
}

void qtNodeEdit::editSDL(plStateDataRecord *sdl) {
    hsRAMStream S(PlasmaVer::pvMoul);
    plStateDescriptor* desc = sdl->getDescriptor();
    sdl->WriteStreamHeader(&S, desc->getName(), desc->getVersion(), NULL);
    sdl->write(&S, resmgr);
    plVaultBlob blob = node->getBlob(0);
    char* data = new char[S.size()];
    S.copyTo(data, S.size());
    blob.setData(S.size(), (const unsigned char*)data);
    delete[] data;
    node->setBlob(0, blob);
    update(true);
}

NodeTypeDelegate::NodeTypeDelegate(QObject *parent) : QStyledItemDelegate(parent) {}

QWidget* NodeTypeDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem&, const QModelIndex&) const {
    QComboBox* box = new QComboBox(parent);
    box->setFrame(false);
    box->setInsertPolicy(QComboBox::NoInsert);
    box->setEditable(true);
    box->addItems({
                      "Player",
                      "Age",
                      "Folder",
                      "PlayerInfo",
                      "System",
                      "Image",
                      "TextNote",
                      "SDL",
                      "AgeLink",
                      "Chronicle",
                      "PlayerInfoList",
                      "Marker",
                      "AgeInfo",
                      "AgeInfoList",
                      "MarkerList"
                  });
    return box;
}

void NodeTypeDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const {
    QString str = index.model()->data(index, Qt::EditRole).toString();
    static_cast<QComboBox*>(editor)->setCurrentText(str);
}

void NodeTypeDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const {
    model->setData(index, static_cast<QComboBox*>(editor)->currentText(), Qt::EditRole);
}

void NodeTypeDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex&) const {
    editor->setGeometry(option.rect);
}

FolderTypeDelegate::FolderTypeDelegate(QObject *parent) : QStyledItemDelegate(parent) {}

QWidget* FolderTypeDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem&, const QModelIndex&) const {
    QComboBox* box = new QComboBox(parent);
    box->setFrame(false);
    box->addItems({
                      "UserDefinedNode",
                      "InboxFolder",
                      "BuddyListFolder",
                      "IgnoreListFolder",
                      "PeopleIKnowAboutFolder",
                      "VaultMgrGlobalDataFolder",
                      "ChronicleFolder",
                      "AvatarOutfitFolder",
                      "AgeTypeJournalFolder",
                      "SubAgesFolder",
                      "DeviceInboxFolder",
                      "HoodMembersFolder",
                      "AllPlayersFolder",
                      "AgeMembersFolder",
                      "AgeJournalsFolder",
                      "AgeDevicesFolder",
                      "AgeInstanceSDLNode",
                      "AgeGlobalSDLNode",
                      "CanVisitFolder",
                      "AgeOwnersFolder",
                      "AllAgeGlobalSDLNodesFolder",
                      "PlayerInfoNode",
                      "PublicAgesFolder",
                      "AgesIOwnFolder",
                      "AgesICanVisitFolder",
                      "AvatarClosetFolder",
                      "AgeInfoNode",
                      "SystemNode",
                      "PlayerInviteFolder",
                      "CCRPlayersFolder",
                      "GlobalInboxFolder",
                      "ChildAgesFolder",
                      "GameScoresFolder",
                      "LastStandardNode"
                  });
    return box;
}

void FolderTypeDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const {
    int i = index.model()->data(index, Qt::EditRole).toInt();
    static_cast<QComboBox*>(editor)->setCurrentIndex(i);
}

void FolderTypeDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const {
    model->setData(index, static_cast<QComboBox*>(editor)->currentIndex(), Qt::EditRole);
}

void FolderTypeDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex&) const {
    editor->setGeometry(option.rect);
}

void qtNodeEdit::update(bool sdlEdit) {
    disconnect(ui->nodeData, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(dataRowChanged(QTableWidgetItem*)));
    emit isDirty(false);
    ui->nodeData->clearContents();
    if(node != NULL) {
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
                emit isDirty(true);
            }
            if(node->fieldName(i) == "NodeType") {
                ui->nodeData->setItemDelegateForRow(i, typeBox);
            }else if(node->fieldName(i) == "Int32_1" && QStringList({
                "Folder",
                "PlayerInfoList",
                "AgeInfoList",
                "MarkerList"
            }).contains(node->getFieldAsString(7).cstr())) {
                ui->nodeData->setItemDelegateForRow(i, folderBox);
            }else{
                ui->nodeData->setItemDelegateForRow(i, 0);
            }
            ui->nodeData->setItem(i, 0, fitem);
            ui->nodeData->setItem(i, 1, citem);
        }
        // set the type-specific content
        switch(node->getNodeType()) {
            case plVault::kNodeImage:
                {
                    if(node->getBlob(0).getSize()) {
                        int offset = 0;

                        if(node->getInt32(0) == plVault::kJPEG) // JPEG Streams have an additional size at the start
                            offset = 4;

                        QImage image = QImage::fromData(node->getBlob(0).getData() + offset, (int)node->getBlob(0).getSize() - offset);
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
                    ui->nodeDataArea->setTabEnabled(3, false);
                }
                break;
            case plVault::kNodeChronicle:
            case plVault::kNodeTextNote:
                disconnect(ui->textNodeEdit, SIGNAL(textChanged()), this, SLOT(editNodeText()));
                ui->textNodeEdit->setPlainText(QString(node->getText(0)));
                ui->textNodeTitle->setText(QString(node->getString64(0)));
                ui->nodeDataArea->setTabEnabled(1, false);
                ui->nodeDataArea->setTabEnabled(2, true);
                ui->nodeDataArea->setTabEnabled(3, false);
                connect(ui->textNodeEdit, SIGNAL(textChanged()), this, SLOT(editNodeText()));
                break;
            case plVault::kNodeSDL:
                {
                    if(sdlEdit) break; /* if the editor made the change, we don't need to reset the model */
                    plVaultBlob blob = node->getBlob(0);
                    if(blob.getSize() > 0) {
                        QAbstractItemModel* oldModel = ui->SDLTreeView->model();
                        qtSDLTreeModel* sdlModel = new qtSDLTreeModel(ui->SDLTreeView, blob, sdlmgr, resmgr);
                        ui->SDLTreeView->setEnabled(true);
                        ui->SDLTreeView->setModel(sdlModel);
                        ui->SDLTreeView->expand(sdlModel->index(0, 0, QModelIndex()));
                        disconnect(this, SLOT(editSDL(plStateDataRecord*)));
                        connect(sdlModel, SIGNAL(sdlEdited(plStateDataRecord*)), this, SLOT(editSDL(plStateDataRecord*)));
                        if(oldModel)
                            delete oldModel;
                    }else{
                        QAbstractItemModel* oldModel = ui->SDLTreeView->model();
                        ui->SDLTreeView->setModel(0);
                        if(oldModel)
                            delete oldModel;
                        ui->SDLTreeView->setEnabled(false);
                    }
                    ui->nodeDataArea->setTabEnabled(1, false);
                    ui->nodeDataArea->setTabEnabled(2, false);
                    ui->nodeDataArea->setTabEnabled(3, true);
                    break;
                }
            default:
                ui->nodeDataArea->setTabEnabled(1, false);
                ui->nodeDataArea->setTabEnabled(2, false);
                ui->nodeDataArea->setTabEnabled(3, false);
                break;
        }
        node->unlockNode();
    }else{
        ui->nodeData->setEnabled(false);
    }
    connect(ui->nodeData, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(dataRowChanged(QTableWidgetItem*)));
}

qtVaultNode* qtNodeEdit::getNode() {
    return node;
}
