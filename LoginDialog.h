#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QtGui/QDialog>
#include <QSettings>

namespace Ui {
    class LoginDialog;
}

class LoginDialog : public QDialog {
    Q_OBJECT
    Q_DISABLE_COPY(LoginDialog)
public:
    explicit LoginDialog(QWidget *parent = 0);
    virtual ~LoginDialog();

public slots:
    void sendLogin();

signals:
    void login(QString user, QString pass);

protected:
    virtual void changeEvent(QEvent *e);

private:
    Ui::LoginDialog *m_ui;
    QSettings settings;
};

#endif // LOGINDIALOG_H
