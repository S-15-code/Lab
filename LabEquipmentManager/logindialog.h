#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>
#include <QKeyEvent>
#include <QFont>

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    bool isLoginSuccessful() const { return loginSuccess; }

protected:
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void onLoginClicked();
    void onCancelClicked();

private:
    void setupUI();
    bool validateUser(const QString &username, const QString &password);

    QLineEdit *usernameEdit;
    QLineEdit *passwordEdit;
    QPushButton *loginButton;
    QPushButton *cancelButton;
    bool loginSuccess;
};

#endif // LOGINDIALOG_H 