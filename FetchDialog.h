#ifndef FETCHDIALOG_H
#define FETCHDIALOG_H

#include <QtGui/QDialog>

typedef unsigned int hsUint32;

namespace Ui {
    class FetchDialog;
}

class FetchDialog : public QDialog {
    Q_OBJECT
    Q_DISABLE_COPY(FetchDialog)
public:
    explicit FetchDialog(QWidget *parent = 0);
    virtual ~FetchDialog();

public slots:
    void fetchSlot();

signals:
    void fetchNode(hsUint32 idx);

protected:
    virtual void changeEvent(QEvent *e);

private:
    Ui::FetchDialog *m_ui;
};

#endif // FETCHDIALOG_H
