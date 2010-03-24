#ifndef REFDIALOG_H
#define REFDIALOG_H

#include <QtGui/QDialog>

typedef unsigned int hsUint32;

namespace Ui {
    class RefDialog;
}

class RefDialog : public QDialog {
    Q_OBJECT
    Q_DISABLE_COPY(RefDialog)
public:
    explicit RefDialog(QWidget *parent = 0);
    virtual ~RefDialog();

    void setupRefBox(hsUint32 owner, hsUint32 parent=0);

public slots:
    void sendRef();

signals:
    void addRef(hsUint32 parent, hsUint32 child, hsUint32 owner);

protected:
    virtual void changeEvent(QEvent *e);

private:
    Ui::RefDialog *m_ui;
    hsUint32 owner;
};

#endif // REFDIALOG_H
