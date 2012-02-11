#ifndef QTSDLTREEMODEL_H
#define QTSDLTREEMODEL_H

#include "SDL/plStateDataRecord.h"
#include <QAbstractItemModel>

class qtSDLTreeModel : public QAbstractItemModel
{
    Q_OBJECT
private:
    plStateDataRecord* sdl;

public:
    explicit qtSDLTreeModel(plStateDataRecord* sdl);
    QModelIndex index(int row, int column, const QModelIndex& parent) const;
    int rowCount(const QModelIndex& parent) const;
    int columnCount(const QModelIndex& parent) const;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

signals:
    void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);

public slots:

};

#endif // QTSDLTREEMODEL_H
