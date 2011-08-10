#ifndef REFDIALOG_H
#define REFDIALOG_H

#include <QtGui/QDialog>

typedef unsigned int uint32_t;

namespace Ui {
    class RefDialog;
}

class RefDialog : public QDialog {
    Q_OBJECT
    Q_DISABLE_COPY(RefDialog)
public:
    explicit RefDialog(QWidget *parent = 0);
    virtual ~RefDialog();

    void setupRefBox(uint32_t owner, uint32_t parent=0);

public slots:
    void sendRef();

signals:
    void addRef(uint32_t parent, uint32_t child, uint32_t owner);

protected:
    virtual void changeEvent(QEvent *e);

private:
    Ui::RefDialog *m_ui;
    uint32_t owner;
};

#endif // REFDIALOG_H
