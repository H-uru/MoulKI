#ifndef QTSDLTREEMODEL_H
#define QTSDLTREEMODEL_H

#include <QVector>
#include <QAbstractItemModel>
#include <auth/pnVaultNode.h>
#include <SDL/plSDLMgr.h>

class plStateDataRecord;
class plStateVariable;

class qtSDLTreeModel : public QAbstractItemModel
{
    Q_OBJECT
private:
    plStateDataRecord* sdl;
    enum ItemType { kSDR, kVar, kVal };
    struct SDLModelIndex {
        QModelIndex parent;
        ItemType type;
        int row, column;
        union {
            plStateDataRecord* sdr;
            plStateVariable* sv;
            void* raw;
        } ptr;

        bool operator==(const SDLModelIndex& other) const {
            return parent == other.parent && type == other.type && row == other.row && column == other.column && ptr.raw == other.ptr.raw;
        }
    };
    mutable QVector<SDLModelIndex> indices;

    QModelIndex ICreateIndex(int row, int column, const QModelIndex& parent, void* ptr, ItemType type) const;

public:
    explicit qtSDLTreeModel(QObject* parent, plVaultBlob blob, plSDLMgr* sdlmgr, plResManager* resmgr);
    ~qtSDLTreeModel();
    QModelIndex index(int row, int column, const QModelIndex& parent) const;
    QModelIndex parent(const QModelIndex &child) const;
    int rowCount(const QModelIndex& parent) const;
    int columnCount(const QModelIndex& parent) const;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);

signals:
    void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);
    void sdlEdited(plStateDataRecord* sdl);

public slots:

};

#endif // QTSDLTREEMODEL_H
