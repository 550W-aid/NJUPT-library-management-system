#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QMessageBox>
#include <QGraphicsDropShadowEffect>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();

    QString getUsername() const;
    QString getPassword() const;
    bool isAdminMode() const;

private slots:
    void onLogin();
    void onCancel();
    void onAdminModeToggle(bool checked);

private:
    void setupUI();
    void setupAnimations();
    void setupStyles();
    void showEvent(QShowEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    bool validateCredentials(const QString &username, const QString &password);
    bool validateAdminPassword(const QString &password);

    QLabel *titleLabel;
    QLabel *subtitleLabel;
    QLineEdit *usernameEdit;
    QLineEdit *passwordEdit;
    QPushButton *loginButton;
    QPushButton *cancelButton;
    QPushButton *adminModeButton;
    
    QGraphicsOpacityEffect *opacityEffect;
    QPropertyAnimation *fadeInAnimation;
    
    bool adminMode;
    QString currentUsername;
    QString currentPassword;
};

#endif // LOGINDIALOG_H
