#include "qtSDLTreeModel.h"
#include "SDL/plStateDataRecord.h"
#include "SDL/plStateVariable.h"

qtSDLTreeModel::qtSDLTreeModel(plStateDataRecord* sdl) :
    QAbstractItemModel(0), sdl(sdl)
{
}

QModelIndex qtSDLTreeModel::ICreateIndex(int row, int column, QModelIndex parent, void *ptr, ItemType type) {
    SDLModelIndex index;
    index.ptr = ptr;
    index.type = type;
    index.parent = parent;
    indices.append(index);
    return createIndex(row, column, indices.count() - 1);
}

QModelIndex qtSDLTreeModel::parent(const QModelIndex &child) const {
    return indices[child.internalId()].parent;
}

QModelIndex qtSDLTreeModel::index(int row, int column, const QModelIndex& parent) const {
    if(parent.isValid()) {
        SDLModelIndex myParent = indices[parent.internalId()];
        switch(myParent.type) {
        case kSDR:
            return ICreateIndex(row, column, parent, myParent.sdr->get(row), kVar);
        case kVar:
            if(myParent.sv->getDescriptor()->getType() == plVarDescriptor::kStateDescriptor)
                return ICreateIndex(row, column, parent, ((plSDStateVariable*)myParent.sv)->Record(row), kSDR);
            else
                return ICreateIndex(row, column, parent, myParent.sv, kVal);
        case kVal:
            return QModelIndex(); // this is not valid, values have no children
        }
    }else{
        return ICreateIndex(row, column, parent, sdl, kSDR);
    }
}

int qtSDLTreeModel::rowCount(const QModelIndex& parent) const {
    if(parent.isValid()) {
        SDLModelIndex myParent = indicies[parent.internalId()];
        switch(myParent.type) {
        case kSDR:
            return myParent.sdr->getNumVars();
        case kVar:
            return myParent.sv->getCount();
        case kVal:
            return 0;
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
        SDLModelIndex myIndex = indices[index.internalId()];
        switch(myIndex.type) {
        case kSDR:
            return QVariant(myIndex.sdr->getDescriptor()->getName().cstr());
        case kVar:
            return QVariant(myIndex.sv->getDescriptor()->getName().cstr());
        case kVal:
            {
                plSimpleStateVariable* var = myIndex.sv;
                switch(var->getDescriptor()->getType()) {
                case plVarDescriptor::kBool:
                    return QVariant(var->Bool(index.row()) ? "True" : "False");
                case plVarDescriptor::kInt:
                    return QVariant(plString::Format("%d", var->Int(index.row())).cstr());
                default:
                    return QVariant("Unhandled SDL Var Type");
                }
            }
        }
    }
    return QVariant("Requested invalid index");
}
