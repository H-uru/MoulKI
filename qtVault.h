#ifndef QTVAULT_H
#define QTVAULT_H

#include <QMap>
#include <QFile>
#include <QMutex>
#include <QObject>
#include <QMetaType>
#include <QTreeWidgetItem>
#include <auth/pnVaultNode.h>

class qtVaultNode : public pnVaultNode
{
private:
    static const char* TypeNames[];
    static const char* FolderTypes[];
    static const char* FieldNames[];

    QList<qtVaultNode*> children;
    QList<QTreeWidgetItem*> items;
    QMutex nodeMutex;

public:
    qtVaultNode();
    qtVaultNode(const pnVaultNode& node);
    qtVaultNode(const qtVaultNode& node);
    void operator=(const qtVaultNode& node);
    void copy(const qtVaultNode &init);

    plString displayName();
    plString fieldName(size_t field);
    plString getFieldAsString(size_t field);
    QIcon getIcon();
    void setFieldFromString(size_t field, plString string);
    const QList<qtVaultNode*> getChildren();
    bool addChild(qtVaultNode* child);
    void removeChild(qtVaultNode* child);

    // special search functions
    qtVaultNode* getBuddiesFolder();
    qtVaultNode* getNeighborsFolder();
    qtVaultNode* getAgeInfoNode();

    QTreeWidgetItem* newItem();
    void removeItem(QTreeWidgetItem* item);
    const QList<QTreeWidgetItem*> getItems();

    void lockNode();
    void unlockNode();
    bool tryLock();
};
Q_DECLARE_METATYPE(qtVaultNode*)

class qtVault : public QObject
{
    Q_OBJECT
private:
    QMap<uint32_t, qtVaultNode> nodes;
    QList<pnVaultNodeRef> refQueue;
    QList<pnVaultNodeRef> refList;
    QList<uint32_t> rootQueue;
    QMutex vaultMutex;

public:
    qtVault();
    ~qtVault();
    void addNode(const pnVaultNode& node);
    void addRef(const pnVaultNodeRef& ref);
    void removeRef(uint32_t parent, uint32_t child);
    void queueRoot(uint32_t idx);
    qtVaultNode* getNode(uint32_t idx);
    bool hasNode(uint32_t idx);

    void writeVault(hsFileStream& file);
    void readVault(hsFileStream& file);

signals:
    void addedNode(uint32_t parent, uint32_t child);
    void removedNode(uint32_t parent, uint32_t child);
    void updatedNode(uint32_t idx);
    void gotRootNode(uint32_t idx);
    void fetchComplete();
};

#endif // QTVAULT_H
