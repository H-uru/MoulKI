#include "qtSDLTreeModel.h"
#include "SDL/plStateDataRecord.h"
#include "SDL/plStateVariable.h"
#include <Stream/hsRAMStream.h>
#include <QStringList>

qtSDLTreeModel::qtSDLTreeModel(plStateDataRecord* sdl) :
    QAbstractItemModel(0), sdl(sdl)
{
}

qtSDLTreeModel::~qtSDLTreeModel() {}

QModelIndex qtSDLTreeModel::ICreateIndex(int row, int column, const QModelIndex& parent, void *ptr, ItemType type) const {
    SDLModelIndex index;
    index.ptr.raw = ptr;
    index.type = type;
    index.parent = parent;
    index.row = row;
    index.column = column;
    if(indices.contains(index)) {
        return createIndex(row, column, indices.indexOf(index));
    }else{
        indices.append(index);
        return createIndex(row, column, indices.count() - 1);
    }
}

QModelIndex qtSDLTreeModel::parent(const QModelIndex &child) const {
    return indices[child.internalId()].parent;
}

QModelIndex qtSDLTreeModel::index(int row, int column, const QModelIndex& parent) const {
    if(parent.isValid()) {
        SDLModelIndex myParent = indices[parent.internalId()];
        switch(myParent.type) {
        case kSDR:
            return ICreateIndex(row, column, parent, myParent.ptr.sdr->get(row), kVar);
        case kVar:
            if(myParent.ptr.sv->getDescriptor()->getType() == plVarDescriptor::kStateDescriptor)
                return ICreateIndex(row, column, parent, ((plSDStateVariable*)myParent.ptr.sv)->Record(row), kSDR);
            else
                return ICreateIndex(row, column, parent, myParent.ptr.sv, kVal);
        case kVal:
            return QModelIndex(); // this is not valid, values have no children
        default:
            return QModelIndex(); // satisfy the compiler
        }
    }else{
        return ICreateIndex(row, column, parent, sdl, kSDR);
    }
}

int qtSDLTreeModel::rowCount(const QModelIndex& parent) const {
    if(parent.isValid()) {
        SDLModelIndex myParent = indices[parent.internalId()];
        switch(myParent.type) {
        case kSDR:
            return myParent.ptr.sdr->getNumVars();
        case kVar:
            return myParent.ptr.sv->getCount();
        case kVal:
            return 0;
        default:
            return 0;
        }
    }else{
        return 1; // only one root
    }
}

int qtSDLTreeModel::columnCount(const QModelIndex&) const {
    return 1;
}

// gets a prc string for any pointer
// to an object with a prcWrite method
template <class T>
QString getPrc(T v) {
    // make a prc object and write to it
    hsRAMStream s(PlasmaVer::pvMoul);
    pfPrcHelper prc(&s);
    v->prcWrite(&prc);
    // extract the contents of the stream
    char* str = new char[s.size() + 1];
    str[s.size()] = 0;
    s.copyTo(str, s.size());
    QString out(str);
    delete[] str;
    // strip off the xml header
    QStringList lst = out.split('\n');
    out = QString();
    for(int i = 2; i < lst.count(); i++)
        out.append(lst[i]);
    return out;
}

QVariant qtSDLTreeModel::data(const QModelIndex& index, int role) const {
    if(role != Qt::DisplayRole)
        return QVariant();
    if(index.isValid()) {
        SDLModelIndex myIndex = indices[index.internalId()];
        switch(myIndex.type) {
        case kSDR:
            return QVariant(myIndex.ptr.sdr->getDescriptor()->getName().cstr());
        case kVar:
            return QVariant(myIndex.ptr.sv->getDescriptor()->getName().cstr());
        case kVal:
            {
                plSimpleStateVariable* var = (plSimpleStateVariable*)myIndex.ptr.sv;
                switch(var->getDescriptor()->getType()) {
                case plVarDescriptor::kBool:
                    return QVariant(var->Bool(index.row()) ? "True" : "False");
                case plVarDescriptor::kInt:
                    return QVariant(plString::Format("%d", var->Int(index.row())).cstr());
                case plVarDescriptor::kByte:
                    return QVariant(plString::Format("%d", var->Byte(index.row())).cstr());
                case plVarDescriptor::kFloat:
                    return QVariant(plString::Format("%f", var->Float(index.row())).cstr());
                case plVarDescriptor::kDouble:
                    return QVariant(plString::Format("%f", var->Double(index.row())).cstr());
                case plVarDescriptor::kString:
                    return QVariant(var->String(index.row()).cstr());
                case plVarDescriptor::kChar:
                    return QVariant(QString(var->Char(index.row())));
                case plVarDescriptor::kKey:
                    return QVariant(getPrc<plUoid*>(&var->Uoid(index.row())));
                case plVarDescriptor::kCreatable:
                    return QVariant(getPrc<plCreatable*>(var->Creatable(index.row())));
                case plVarDescriptor::kRGB8:
                case plVarDescriptor::kRGBA8:
                    return QVariant(getPrc<hsColor32*>(&var->Color32(index.row())));
                case plVarDescriptor::kRGB:
                case plVarDescriptor::kRGBA:
                    return QVariant(getPrc<hsColorRGBA*>(&var->ColorRGBA(index.row())));
                case plVarDescriptor::kPoint3:
                case plVarDescriptor::kVector3:
                    return QVariant(getPrc<hsVector3*>(&var->Vector(index.row())));
                case plVarDescriptor::kQuaternion:
                    return QVariant(getPrc<hsQuat*>(&var->Quat(index.row())));
                default:
                    return QVariant(plString::Format("Unhandled SDL Var Type (%d)", var->getDescriptor()->getType()).cstr());
                }
            }
        }
    }
    return QVariant("Requested invalid index");
}
