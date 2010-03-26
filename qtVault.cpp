#include "qtVault.h"

const char* qtVaultNode::TypeNames[] = {
    "Invalid",
    "VMgrLow",
    "Player",
    "Age",
    "GameServer",
    "Admin",
    "VaultServer",
    "CCR",
    "", "", "", "", "", "", "",
    "", "", "", "", "", "",
    "VMgrHigh",
    "Folder",
    "PlayerInfo",
    "System",
    "Image",
    "TextNote",
    "SDL",
    "AgeLink",
    "Chronicle",
    "PlayerInfoList",
    "UNUSED",
    "Marker",
    "AgeInfo",
    "AgeInfoList",
    "MarkerList",
    "NumTypes"
};

const char* qtVaultNode::FolderTypes[] = {
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
};

const char* qtVaultNode::FieldNames[] = {
    "NodeIdx",
    "CreateTime",
    "ModifyTime",
    "CreateAgeName",
    "CreateAgeGuid",
    "CreatorUuid",
    "CreatorIdx",
    "NodeType",
    "Int32_1",
    "Int32_2",
    "Int32_3",
    "Int32_4",
    "UInt32_1",
    "UInt32_2",
    "UInt32_3",
    "UInt32_4",
    "Uuid_1",
    "Uuid_2",
    "Uuid_3",
    "Uuid_4",
    "String64_1",
    "String64_2",
    "String64_3",
    "String64_4",
    "String64_5",
    "String64_6",
    "IString64_1",
    "IString64_2",
    "Text_1",
    "Text_2",
    "Blob_1",
    "Blob_2"
};

qtVault::qtVault() {
    qRegisterMetaType<hsUint32>("hsUint32");
}

qtVault::~qtVault() {
}

void qtVault::addNode(const pnVaultNode& node) {
    // vault is locked during node adds, to prevent writing processes from
    // accessing a node while it is being copied and reffed
    vaultMutex.lock();
    if(nodes.contains(node.getNodeIdx())) {
        // only copy the node data
        nodes[node.getNodeIdx()].pnVaultNode::copy(node);
        emit updatedNode(node.getNodeIdx());
    }else{
        nodes.insert(node.getNodeIdx(), qtVaultNode(node));
    }
    // check to see if there are any queued refs referencing the newly added node
    foreach(pnVaultNodeRef ref, refQueue) {
        if(ref.fChild == node.getNodeIdx() || ref.fParent == node.getNodeIdx()) {
            // if a ref is found, re-add it, this will resolve it, it possible
            // if not, when the other node arrives, the ref will be resolved
            addRef(ref);
        }
    }
    vaultMutex.unlock();
    // send signals for vault events triggered by this nodeAdd
    if(rootQueue.contains(node.getNodeIdx())) {
        rootQueue.removeAll(node.getNodeIdx());
        emit gotRootNode(node.getNodeIdx());
    }else if(refQueue.count() == 0) {
        // if we just got a root, then the tree fetch just started :P
        qWarning("Tree fetch complete");
        emit fetchComplete();
    }
}

void qtVault::addRef(const pnVaultNodeRef& ref) {
    if(!refList.contains(ref))
        // add the ref to the permanent list if it's not already there
        refList.append(ref);
    if(nodes.contains(ref.fChild) && nodes.contains(ref.fParent)) {
        // if we have both the parent and child, the ref is resolved, and can be dequeued
        if(nodes[ref.fParent].addChild(&nodes[ref.fChild])) {
            // we only emit the addedNode signal if the ref was not already resolved
            emit addedNode(ref.fParent, ref.fChild);
        }
        refQueue.removeAll(ref);
    }else if(!refQueue.contains(ref)) {
        // if the ref was not resolved and it's not queued already, it should be queued
        refQueue.append(ref);
    }
}

void qtVault::removeRef(hsUint32 parent, hsUint32 child) {
    if(nodes.contains(parent) && nodes.contains(child)) {
        nodes[parent].removeChild(&nodes[child]);
        // remove pending refs
        pnVaultNodeRef ref;
        ref.fParent = parent;
        ref.fChild = child;
        refQueue.removeAll(ref);
        refList.removeAll(ref);
        emit removedNode(parent, child);
    }
}

void qtVault::queueRoot(hsUint32 idx) {
    rootQueue.append(idx);
}

qtVaultNode* qtVault::getNode(hsUint32 idx) {
    return &nodes[idx];
}

void qtVault::writeVault(hsFileStream& file) {
    vaultMutex.lock();
    file.writeInt(refList.count());
    foreach(pnVaultNodeRef ref, refList) {
        file.write(sizeof(ref), &ref);
    }
    file.writeInt(nodes.count());
    foreach(qtVaultNode node, nodes) {
        node.allDirty();
        size_t size = node.bufferSize();
        hsUbyte* buffer = new hsUbyte[size];
        file.writeInt(size);
        node.write(buffer, size);
        file.write(size, buffer);
        delete[] buffer;
    }
    vaultMutex.unlock();
}

void qtVault::readVault(hsFileStream& file) {
    hsUint32 refCount = file.readInt();
    for(hsUint32 i = 0; i < refCount; i++) {
        pnVaultNodeRef ref;
        file.read(sizeof(ref), &ref);
        addRef(ref);
    }
    hsUint32 nodeCount = file.readInt();
    for(hsUint32 i = 0; i < nodeCount; i++) {
        pnVaultNode node;
        size_t size = file.readInt();
        hsUbyte* buffer = new hsUbyte[size];
        file.read(size, buffer);
        node.read((const hsUbyte*)buffer, size);
        addNode(node);
        delete[] buffer;
    }
}

qtVaultNode::qtVaultNode() {
}

qtVaultNode::qtVaultNode(const pnVaultNode& node) : pnVaultNode(node) {
}

qtVaultNode::qtVaultNode(const qtVaultNode &node) {
    copy(node);
}

void qtVaultNode::operator =(const qtVaultNode& node) {
    copy(node);
}

void qtVaultNode::copy(const qtVaultNode &init) {
    pnVaultNode::copy(init);
    children = init.children;
    items = init.items;
}

plString qtVaultNode::displayName() {
    switch(getNodeType()) {
        case 2: // Player
        case 23: // PlayerInfo
            if(hasField(kIString64_1))
                return getIString64(k_1);
            break;
        case 3: // Age
        case 25: // Image
        case 26: // TextNote
        case 27: // SDL
        case 29: // Chronicle
            if(hasField(kString64_1))
                return getString64(k_1);
            break;
        case 22: // Folder
            if(hasField(kInt32_1) && (getInt32(k_1) == 8 || getInt32(k_1) == 0))
                // AgeTypeJournalFolder || UserDefinedNode
                return getString64(k_1);
        case 30: // PlayerInfoList
        case 34: // AgeInfoList
            if(hasField(kInt32_1))
                return plString(FolderTypes[getInt32(k_1)]);
            break;
        case 33: // AgeInfo
        case 35: // MarkerList
            if(hasField(kText_1))
                return getText(k_1);
            break;
    }
    return plString(TypeNames[getNodeType()]);
}

QIcon qtVaultNode::getIcon() {
    QImage iconImage = QImage(plString::Format(":/%s", TypeNames[getNodeType()]).cstr());
    bool shift = false;
    if(getNodeType() == 23 && getInt32(k_1) == 1) { // PlayerInfo
        shift = true;
    }else if(getNodeType() == 2) { // Player
        foreach(qtVaultNode* child, getChildren()) {
            if(child->getNodeType() == 23 && child->getInt32(k_1) == 1) {
                shift = true;
                break;
            }
        }
    }
    if(shift) {
        // manipulate it to make it green
        for(int i = 0; i < iconImage.colorCount(); i++) {
            if(iconImage.color(i) == 0xFFFF0000)
                iconImage.setColor(i, 0xFF00FF00);
            if(iconImage.color(i) == 0xFF800000)
                iconImage.setColor(i, 0xFF008000);
        }
    }
    return QIcon(QPixmap::fromImage(iconImage));
}

QTreeWidgetItem* qtVaultNode::newItem() {
    QTreeWidgetItem* item = new QTreeWidgetItem();
    plString name = displayName();
    item->setText(0, QString(name.cstr()));
    item->setIcon(0, getIcon());
    QVariant data;
    data.setValue(this);
    item->setData(0, Qt::UserRole, data);
    items.append(item);
    return item;
}

plString qtVaultNode::fieldName(size_t field) {
    return plString(FieldNames[field]);
}

plString qtVaultNode::getFieldAsString(size_t field) {
    if(!hasField(field))
        return plString();
    switch(field) {
        case kNodeIdx:
            return plString::Format("%u", getNodeIdx());
        case kCreateTime:
            return plString::Format("%u", getCreateTime());
        case kModifyTime:
            return plString::Format("%u", getModifyTime());
        case kCreateAgeName:
            return getCreateAgeName();
        case kCreateAgeUuid:
            return getCreateAgeUuid().toString();
        case kCreatorUuid:
            return getCreatorUuid().toString();
        case kCreatorIdx:
            return plString::Format("%u", getCreatorIdx());
        case kNodeType:
            return plString(TypeNames[getNodeType()]);
        case kInt32_1:
        case kInt32_2:
        case kInt32_3:
        case kInt32_4:
            return plString::Format("%d", getInt32(field - kInt32_1));
        case kUint32_1:
        case kUint32_2:
        case kUint32_3:
        case kUint32_4:
            return plString::Format("%u", getUint32(field - kUint32_1));
        case kUuid_1:
        case kUuid_2:
        case kUuid_3:
        case kUuid_4:
            return getUuid(field - kUuid_1).toString();
        case kString64_1:
        case kString64_2:
        case kString64_3:
        case kString64_4:
        case kString64_5:
        case kString64_6:
            return getString64(field - kString64_1);
        case kIString64_1:
        case kIString64_2:
            return getIString64(field - kIString64_1);
        case kText_1:
        case kText_2:
            return getText(field - kText_1);
        case kBlob_1:
        case kBlob_2:
            {
                plVaultBlob blob = getBlob(field - kBlob_1);
                if(blob.getSize() > 180)
                    return plString("<Blob Data>");
                return plString((const char*)blob.getData());
            }
        default:
            return plString("Bad Field Id (%u)", field);
    }
}

void qtVaultNode::setFieldFromString(size_t field, plString string) {
    switch(field) {
        case kNodeIdx:
            setNodeIdx(string.toUint(10));
            break;
        case kCreateTime:
            setCreateTime(string.toUint(10));
            break;
        case kModifyTime:
            setModifyTime(string.toUint(10));
            break;
        case kCreateAgeName:
            setCreateAgeName(string);
            break;
        case kCreateAgeUuid:
            setCreateAgeUuid(plUuid(string.cstr()));
            break;
        case kCreatorUuid:
            setCreatorUuid(plUuid(string.cstr()));
            break;
        case kCreatorIdx:
            setCreatorIdx(string.toUint(10));
            break;
        case kNodeType:
            if(string.empty())
                break;
            for(unsigned int i = 0; i < sizeof(TypeNames) / sizeof(const char*); i++) {
                if(string.compareTo(TypeNames[i], true) == 0) {
                    setNodeType(i);
                    break;
                }
            }
            break;
        case kInt32_1:
        case kInt32_2:
        case kInt32_3:
        case kInt32_4:
            setInt32(field - kInt32_1, string.toInt(10));
            break;
        case kUint32_1:
        case kUint32_2:
        case kUint32_3:
        case kUint32_4:
            setUInt32(field - kUint32_1, string.toUint(10));
            break;
        case kUuid_1:
        case kUuid_2:
        case kUuid_3:
        case kUuid_4:
            {
                try {
                    plUuid uuid(string.cstr());
                    setUuid(field - kUuid_1, uuid);
                } catch (hsException e) {
                    qWarning("qtVaultNode::setFieldFromString(): Bad Uuid String (%s)", e.what());
                }
            }
            break;
        case kString64_1:
        case kString64_2:
        case kString64_3:
        case kString64_4:
        case kString64_5:
        case kString64_6:
            setString64(field - kString64_1, string);
            break;
        case kIString64_1:
        case kIString64_2:
            setIString64(field - kIString64_1, string);
            break;
        case kText_1:
        case kText_2:
            setText(field - kText_1, string);
            break;
        case kBlob_1:
        case kBlob_2:
            {
                plVaultBlob blob;
                if(string.len())
                    blob.setData(string.len(), (const unsigned char*)string.cstr());
                setBlob(field - kBlob_1, blob);
            }
            break;
        default:
            qWarning("qtVaultNode::setFieldFromString(): Bad Field ID (Field: %u Node: %u)", field, getNodeIdx());
    }
}

void qtVaultNode::removeItem(QTreeWidgetItem* item) {
    items.removeAll(item);
}

const QList<QTreeWidgetItem*> qtVaultNode::getItems() {
    return items;
}

const QList<qtVaultNode*> qtVaultNode::getChildren() {
    return children;
}

bool qtVaultNode::addChild(qtVaultNode *child) {
    if(children.contains(child)) {
        return false;
    }else{
        // the idea here is that additions to the child list will block
        // if the list is currently being read by the GUI thread
        lockNode();
        children.append(child);
        unlockNode();
        return true;
    }
}

void qtVaultNode::removeChild(qtVaultNode *child) {
    children.removeAll(child);
}

void qtVaultNode::lockNode() {
    nodeMutex.lock();
}

void qtVaultNode::unlockNode() {
    nodeMutex.unlock();
}

bool qtVaultNode::tryLock() {
    return nodeMutex.tryLock();
    return true;
}
