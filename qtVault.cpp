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
    if(nodes.contains(node.getNodeIdx())) {
        nodes[node.getNodeIdx()].copy(node);
        emit updatedNode(node.getNodeIdx());
    }else{
        nodes.insert(node.getNodeIdx(), qtVaultNode(node));
    }
    if(rootQueue.contains(node.getNodeIdx())) {
        rootQueue.removeAll(node.getNodeIdx());
        emit gotRootNode(node.getNodeIdx());
    }
    // check to see if we can handle some refs
    // the never delete cached refs method
    foreach(pnVaultNodeRef ref, refQueue) {
        if(ref.fChild == node.getNodeIdx() || ref.fParent == node.getNodeIdx()) {
            addRef(ref);
        }
    }
    /*
    // the clean the cache method
    QList<pnVaultNodeRef> resolvedRefs;
    foreach(pnVaultNodeRef ref, refQueue) {
        if(addRef(ref)) {
            resolvedRefs.append(ref);
        }
    }
    foreach(pnVaultNodeRef ref, resolvedRefs) {
        refQueue.removeAll(ref);
    }
    */
    if(refQueue.count() == 0)
        emit fetchComplete();
}

bool qtVault::addRef(const pnVaultNodeRef& ref) {
    if(nodes.contains(ref.fChild) && nodes.contains(ref.fParent)) {
        if(!nodes[ref.fParent].children.contains(&nodes[ref.fChild])) {
            nodes[ref.fParent].children.append(&nodes[ref.fChild]);
            emit addedNode(ref.fParent, ref.fChild);
        }
        return true;
    }else if(!refQueue.contains(ref)) {
        refQueue.append(ref);
    }
    return false;
}

void qtVault::removeRef(hsUint32 parent, hsUint32 child) {
    if(nodes.contains(parent) && nodes.contains(child)) {
        nodes[parent].children.removeAll(&nodes[child]);
        emit removedNode(parent, child);
        // remove pending refs
        pnVaultNodeRef ref;
        ref.fParent = parent;
        ref.fChild = child;
        refQueue.removeAll(ref);
    }
}

void qtVault::queueRoot(hsUint32 idx) {
    rootQueue.append(idx);
}

qtVaultNode* qtVault::getNode(hsUint32 idx) {
    return &nodes[idx];
}

void qtVault::writeVault(hsFileStream& file) {
    file.writeInt(refQueue.count());
    foreach(pnVaultNodeRef ref, refQueue) {
        file.write(sizeof(ref), &ref);
    }
    file.writeInt(nodes.count());
    foreach(qtVaultNode node, nodes) {
        node.allDirty();
        size_t size = node.bufferSize();
        hsUbyte* buffer = new hsUbyte[size];
        qWarning("Writing Node (%u) %s size: %u", node.getNodeIdx(), node.displayName().cstr(), size);
        file.writeInt(size);
        node.write(buffer, size);
        file.write(size, buffer);
        delete[] buffer;
    }
}

void qtVault::readVault(hsFileStream& file) {
    hsUint32 refCount = file.readInt();
    for(hsUint32 i = 0; i < refCount; i++) {
        pnVaultNodeRef ref;
        file.read(sizeof(ref), &ref);
        qWarning("Read Ref: {%u -> %u}", ref.fParent, ref.fChild);
        addRef(ref);
    }
    hsUint32 nodeCount = file.readInt();
    for(hsUint32 i = 0; i < nodeCount; i++) {
        pnVaultNode node;
        size_t size = file.readInt();
        hsUbyte* buffer = new hsUbyte[size];
        file.read(size, buffer);
        node.read((const hsUbyte*)buffer, size);
        qWarning("Read Node (%u) %s size: %u", node.getNodeIdx(), qtVaultNode(node).displayName().cstr(), size);
        addNode(node);
        delete[] buffer;
    }
}

qtVaultNode::qtVaultNode() {
}

qtVaultNode::qtVaultNode(const pnVaultNode& node) : pnVaultNode(node) {
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

QTreeWidgetItem* qtVaultNode::newItem() {
    QTreeWidgetItem* item = new QTreeWidgetItem();
    plString name = displayName();
    item->setText(0, QString(name.cstr()));
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
            return plString::Format("%u", getInt32(field - kUint32_1));
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
            setUuid(field - kUuid_1, plUuid(string.cstr()));
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
