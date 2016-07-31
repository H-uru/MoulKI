#ifndef QTNODEEDIT_H
#define QTNODEEDIT_H

#include <QWidget>
#include <QTableWidgetItem>
#include <QStyledItemDelegate>
#include "qtVault.h"

namespace Ui {
    class qtNodeEdit;
}

class plSDLMgr;
class plResManager;
class plStateDataRecord;

class NodeTypeDelegate : public QStyledItemDelegate {
    Q_OBJECT
public:
    NodeTypeDelegate(QObject* parent = 0);
    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const Q_DECL_OVERRIDE;
    void setEditorData(QWidget* editor, const QModelIndex &index) const Q_DECL_OVERRIDE;
    void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex &index) const Q_DECL_OVERRIDE;
    void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const Q_DECL_OVERRIDE;
};

class qtNodeEdit : public QWidget {
    Q_OBJECT
public:
    qtNodeEdit(QWidget *parent = 0);
    ~qtNodeEdit();

    void setNode(qtVaultNode* pn);
    void setMgrs(plSDLMgr* sdl, plResManager* res);
    qtVaultNode* getNode();
    void update(bool sdlEdit = false);

signals:
    void isDirty(bool);

protected:
    void changeEvent(QEvent *e);

private:
    Ui::qtNodeEdit *ui;
    qtVaultNode* node;
    plSDLMgr* sdlmgr;
    plResManager* resmgr;
    bool titleEdited;
    bool textEdited;
    QString newTitle;

    NodeTypeDelegate* typeBox;

private slots:
    void dataRowChanged(QTableWidgetItem* item);
    void editNodeTitle(QString title);
    void editNodeText();
    void loadNodeImage();
    void saveNodeImage();
    void editSDL(plStateDataRecord* sdl);
    void tabActivated(int tab);

};

#endif // QTNODEEDIT_H
