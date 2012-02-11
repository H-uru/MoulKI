#include "qtSDLTreeModel.h"

qtSDLTreeModel::qtSDLTreeModel(plStateDataRecord* sdl) :
    QAbstractItemModel(NULL), sdl(sdl)
{

}

QModelIndex qtSDLTreeModel::index(int row, int column, const QModelIndex& parent) const {
    if(parent.isValid()) {

    }else{
        return createIndex(row, column, sdl->get(row));
    }
}

int qtSDLTreeModel::rowCount(const QModelIndex& parent) const {
    if(parent.isValid()) {
        plStateVariable* var = parent.internalPointer();
        plVarDescriptor* desc = var->getDescriptor();
        if(desc->getType() == plVarDescriptor::kStateDescriptor) {
            return ((plSDStateVariable*)var)->getCount();
        }else{
            return ((plSimpleStateVariable*)var)->getCount();
        }
    }else{
        return sdl->getNumVars();
    }
}

int qtSDLTreeModel::columnCount(const QModelIndex& parent) const {
    return 1;
}

QVariant qtSDLTreeModel::data(const QModelIndex& index, int role = Qt::DisplayRole) const {
    if(index.isValid()) {
        plStateVariable* var = index.internalPointer();
        plVarDescriptor* desc = var->getDescriptor();
        return desc->getName();
    }
}
