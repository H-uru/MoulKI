#ifndef QTVAULT_H
#define QTVAULT_H

#include <QMap>
#include <QFile>
#include <QObject>
#include <QMetaType>
#include <QTreeWidgetItem>
#include <net/auth/pnVaultNode.h>

Q_DECLARE_METATYPE(hsUint32)

class qtVaultNode : public pnVaultNode
{
private:
    static const char* TypeNames[];
    static const char* FolderTypes[];
    static const char* FieldNames[];

public:
    qtVaultNode();
    qtVaultNode(const pnVaultNode& node);

    plString displayName();
    plString fieldName(size_t field);
    plString getFieldAsString(size_t field);
    void setFieldFromString(size_t field, plString string);

    QTreeWidgetItem* newItem();
    void removeItem(QTreeWidgetItem* item);

    QList<qtVaultNode*> children;
    QList<QTreeWidgetItem*> items;
};
Q_DECLARE_METATYPE(qtVaultNode*)

class qtVault : public QObject
{
    Q_OBJECT
private:
    QMap<hsUint32, qtVaultNode> nodes;
    QList<pnVaultNodeRef> refQueue;
    QList<hsUint32> rootQueue;

public:
    qtVault();
    ~qtVault();
    void addNode(const pnVaultNode& node);
    bool addRef(const pnVaultNodeRef& ref);
    void removeRef(hsUint32 parent, hsUint32 child);
    void queueRoot(hsUint32 idx);
    qtVaultNode* getNode(hsUint32 idx);

    void writeVault(hsFileStream& file);
    void readVault(hsFileStream& file);

signals:
    void addedNode(hsUint32 parent, hsUint32 child);
    void removedNode(hsUint32 parent, hsUint32 child);
    void updatedNode(hsUint32 idx);
    void gotRootNode(hsUint32 idx);
    void fetchComplete();
};

#endif // QTVAULT_H
