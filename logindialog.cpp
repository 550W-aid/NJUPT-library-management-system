#include "logindialog.h"
#include <QApplication>
#include <QScreen>
#include <QPainter>
#include <QLinearGradient>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QTimer>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>
#include <QGraphicsDropShadowEffect>
#include <QFont>
#include <QPainterPath>
#include <cmath>

LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent)
    , adminMode(false)
{
    setupUI();
    setupAnimations();
    setupStyles();
}

LoginDialog::~LoginDialog()
{
}

void LoginDialog::setupUI()
{
    // 设置窗口属性
    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setFixedSize(400, 500);
    setModal(true);
    
    // 居中显示
    QScreen *screen = QApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    int x = (screenGeometry.width() - width()) / 2;
    int y = (screenGeometry.height() - height()) / 2;
    move(x, y);
    
    // 创建主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(30, 30, 30, 30);
    mainLayout->setSpacing(20);
    
    // 标题标签
    titleLabel = new QLabel("图书管理系统", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet(
        "QLabel {"
        "    color: #1C1C1E;"
        "    font-size: 24px;"
        "    font-weight: 600;"
        "    background: transparent;"
        "    font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif;"
        "}"
    );
    
    // 副标题标签
    subtitleLabel = new QLabel("请登录以继续", this);
    subtitleLabel->setAlignment(Qt::AlignCenter);
    subtitleLabel->setStyleSheet(
        "QLabel {"
        "    color: #8E8E93;"
        "    font-size: 16px;"
        "    font-weight: 400;"
        "    background: transparent;"
        "    font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif;"
        "}"
    );
    
    // 创建表单布局
    QFormLayout *formLayout = new QFormLayout();
    formLayout->setSpacing(15);
    formLayout->setLabelAlignment(Qt::AlignRight);
    
    // 用户名输入框
    usernameEdit = new QLineEdit(this);
    usernameEdit->setPlaceholderText("请输入用户名");
    usernameEdit->setStyleSheet(
        "QLineEdit {"
        "    background-color: #F2F2F7;"
        "    border: 2px solid #E5E5EA;"
        "    border-radius: 12px;"
        "    padding: 12px 16px;"
        "    font-size: 16px;"
        "    color: #1C1C1E;"
        "    font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif;"
        "}"
        "QLineEdit:focus {"
        "    border-color: #007AFF;"
        "    background-color: #FFFFFF;"
        "}"
    );
    
    // 密码输入框
    passwordEdit = new QLineEdit(this);
    passwordEdit->setPlaceholderText("请输入密码");
    passwordEdit->setEchoMode(QLineEdit::Password);
    passwordEdit->setStyleSheet(
        "QLineEdit {"
        "    background-color: #F2F2F7;"
        "    border: 2px solid #E5E5EA;"
        "    border-radius: 12px;"
        "    padding: 12px 16px;"
        "    font-size: 16px;"
        "    color: #1C1C1E;"
        "    font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif;"
        "}"
        "QLineEdit:focus {"
        "    border-color: #007AFF;"
        "    background-color: #FFFFFF;"
        "}"
    );
    
    // 添加标签和输入框到表单
    formLayout->addRow("👤 用户名:", usernameEdit);
    formLayout->addRow("🔒 密码:", passwordEdit);
    
    // 按钮布局
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(12);
    
    // 登录按钮
    loginButton = new QPushButton("登录", this);
    loginButton->setStyleSheet(
        "QPushButton {"
        "    background-color: #007AFF;"
        "    color: #FFFFFF;"
        "    border: none;"
        "    border-radius: 12px;"
        "    padding: 12px 24px;"
        "    font-size: 16px;"
        "    font-weight: 600;"
        "    font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif;"
        "    min-width: 100px;"
        "    min-height: 44px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #0051D5;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #0041B8;"
        "}"
    );
    
    // 取消按钮
    cancelButton = new QPushButton("取消", this);
    cancelButton->setStyleSheet(
        "QPushButton {"
        "    background-color: #F2F2F7;"
        "    color: #1C1C1E;"
        "    border: 2px solid #E5E5EA;"
        "    border-radius: 12px;"
        "    padding: 12px 24px;"
        "    font-size: 16px;"
        "    font-weight: 500;"
        "    font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif;"
        "    min-width: 100px;"
        "    min-height: 44px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #E5E5EA;"
        "    border-color: #C7C7CC;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #D1D1D6;"
        "}"
    );
    
    // 管理员模式按钮
    adminModeButton = new QPushButton("管理员模式", this);
    adminModeButton->setCheckable(true);
    adminModeButton->setStyleSheet(
        "QPushButton {"
        "    background-color: #F2F2F7;"
        "    color: #8E8E93;"
        "    border: 2px solid #E5E5EA;"
        "    border-radius: 12px;"
        "    padding: 8px 16px;"
        "    font-size: 14px;"
        "    font-weight: 500;"
        "    font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif;"
        "    min-height: 36px;"
        "}"
        "QPushButton:checked {"
        "    background-color: #FF3B30;"
        "    color: #FFFFFF;"
        "    border-color: #FF3B30;"
        "}"
        "QPushButton:hover {"
        "    background-color: #E5E5EA;"
        "    border-color: #C7C7CC;"
        "}"
        "QPushButton:checked:hover {"
        "    background-color: #E6342A;"
        "    border-color: #E6342A;"
        "}"
    );
    
    buttonLayout->addWidget(loginButton);
    buttonLayout->addWidget(cancelButton);
    
    // 添加到主布局
    mainLayout->addWidget(titleLabel);
    mainLayout->addWidget(subtitleLabel);
    mainLayout->addLayout(formLayout);
    mainLayout->addWidget(adminModeButton);
    mainLayout->addLayout(buttonLayout);
    
    // 设置透明度效果
    opacityEffect = new QGraphicsOpacityEffect(this);
    setGraphicsEffect(opacityEffect);
    opacityEffect->setOpacity(1.0);  // 直接设置为可见
    
    // 添加阴影效果
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(20);
    shadow->setColor(QColor(0, 0, 0, 30));
    shadow->setOffset(0, 4);
    
    // 连接信号
    connect(loginButton, &QPushButton::clicked, this, &LoginDialog::onLogin);
    connect(cancelButton, &QPushButton::clicked, this, &LoginDialog::onCancel);
    connect(adminModeButton, &QPushButton::toggled, this, &LoginDialog::onAdminModeToggle);
    
    // 设置回车键登录
    connect(usernameEdit, &QLineEdit::returnPressed, this, &LoginDialog::onLogin);
    connect(passwordEdit, &QLineEdit::returnPressed, this, &LoginDialog::onLogin);
}

void LoginDialog::setupAnimations()
{
    // 简化动画设置，避免阻塞
    fadeInAnimation = new QPropertyAnimation(opacityEffect, "opacity", this);
    fadeInAnimation->setDuration(300);
    fadeInAnimation->setStartValue(0.0);
    fadeInAnimation->setEndValue(1.0);
    fadeInAnimation->setEasingCurve(QEasingCurve::OutQuad);
}

void LoginDialog::setupStyles()
{
    // 设置窗口样式
    setStyleSheet(
        "QDialog {"
        "    background-color: transparent;"
        "}"
    );
}

void LoginDialog::paintEvent(QPaintEvent * /* event */)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // iOS风格背景
    QRect rect = this->rect().adjusted(10, 10, -10, -10);
    painter.setBrush(QBrush(QColor(255, 255, 255, 250)));
    painter.setPen(QPen(QColor(200, 200, 200, 100), 1));
    painter.drawRoundedRect(rect, 20, 20);
}

void LoginDialog::onLogin()
{
    QString username = usernameEdit->text().trimmed();
    QString password = passwordEdit->text().trimmed();
    
    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "输入错误", "请输入用户名和密码");
        return;
    }
    
    // 验证管理员密码
    if (adminMode) {
        if (!validateAdminPassword(password)) {
            QMessageBox::warning(this, "密码错误", "管理员密码不正确");
            return;
        }
    } else {
        // 普通用户验证（这里简化处理，实际项目中应该连接数据库）
        if (!validateCredentials(username, password)) {
            QMessageBox::warning(this, "登录失败", "用户名或密码错误");
            return;
        }
    }
    
    currentUsername = username;
    currentPassword = password;
    accept();
}

void LoginDialog::onCancel()
{
    reject();
}

void LoginDialog::onAdminModeToggle(bool checked)
{
    adminMode = checked;
    if (checked) {
        subtitleLabel->setText("管理员模式 - 请输入管理员密码");
        passwordEdit->setPlaceholderText("请输入管理员密码");
    } else {
        subtitleLabel->setText("请登录以继续");
        passwordEdit->setPlaceholderText("请输入密码");
    }
}

QString LoginDialog::getUsername() const
{
    return currentUsername;
}

QString LoginDialog::getPassword() const
{
    return currentPassword;
}

bool LoginDialog::isAdminMode() const
{
    return adminMode;
}

bool LoginDialog::validateCredentials(const QString &username, const QString &password)
{
    // 简化的用户验证，实际项目中应该连接数据库
    // 这里允许任何非空用户名和密码登录
    return !username.isEmpty() && !password.isEmpty();
}

bool LoginDialog::validateAdminPassword(const QString &password)
{
    return password == "1234";
}

void LoginDialog::showEvent(QShowEvent *event)
{
    QDialog::showEvent(event);
    // 确保对话框可见
    raise();
    activateWindow();
}
