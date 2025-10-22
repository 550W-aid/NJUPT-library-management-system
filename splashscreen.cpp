#include "splashscreen.h"
#include <QApplication>
#include <QScreen>
#include <QPainter>
#include <QLinearGradient>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QTimer>
#include <QVBoxLayout>
#include <QLabel>
#include <QProgressBar>
#include <QTime>
#include <QPainterPath>
#include <cmath>

SplashScreen::SplashScreen(QWidget *parent)
    : QWidget(parent)
    , progressValue(0)
    , isFadingOut(false)
{
    setupUI();
    setupAnimations();
}

SplashScreen::~SplashScreen()
{
}

void SplashScreen::setupUI()
{
    // 设置窗口属性
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setFixedSize(400, 300);
    
    // 居中显示
    QScreen *screen = QApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    int x = (screenGeometry.width() - width()) / 2;
    int y = (screenGeometry.height() - height()) / 2;
    move(x, y);
    
    // 创建主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(30, 30, 30, 30);
    mainLayout->setSpacing(15);
    
    // 主标题标签 - iOS风格
    titleLabel = new QLabel("图书管理系统", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet(
        "QLabel {"
        "    color: #000000;"
        "    font-size: 28px;"
        "    font-weight: 600;"
        "    background: transparent;"
        "    font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif;"
        "}"
    );
    
    // 副标题标签
    subtitleLabel = new QLabel("Library Management System", this);
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
    
    // 大学名称标签
    universityLabel = new QLabel("南京邮电大学", this);
    universityLabel->setAlignment(Qt::AlignCenter);
    universityLabel->setStyleSheet(
        "QLabel {"
        "    color: #1C1C1E;"
        "    font-size: 18px;"
        "    font-weight: 500;"
        "    background: transparent;"
        "    font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif;"
        "}"
    );
    
    // 设计者信息标签
    designerLabel = new QLabel("设计者：房睿，张邵奕，茅大鸿", this);
    designerLabel->setAlignment(Qt::AlignCenter);
    designerLabel->setStyleSheet(
        "QLabel {"
        "    color: #8E8E93;"
        "    font-size: 14px;"
        "    font-weight: 400;"
        "    background: transparent;"
        "    font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif;"
        "}"
    );
    
    // 使用自定义环形进度条，无需传统进度条
    
    // 添加到布局
    mainLayout->addStretch();
    mainLayout->addWidget(titleLabel);
    mainLayout->addWidget(subtitleLabel);
    mainLayout->addWidget(universityLabel);
    mainLayout->addWidget(designerLabel);
    mainLayout->addStretch();
    
    // 设置透明度效果
    opacityEffect = new QGraphicsOpacityEffect(this);
    setGraphicsEffect(opacityEffect);
    opacityEffect->setOpacity(0.0);
}

void SplashScreen::setupAnimations()
{
    // iOS风格淡入动画
    fadeInAnimation = new QPropertyAnimation(opacityEffect, "opacity", this);
    fadeInAnimation->setDuration(800);
    fadeInAnimation->setStartValue(0.0);
    fadeInAnimation->setEndValue(1.0);
    fadeInAnimation->setEasingCurve(QEasingCurve::OutCubic);
    
    // iOS风格淡出动画
    fadeOutAnimation = new QPropertyAnimation(opacityEffect, "opacity", this);
    fadeOutAnimation->setDuration(600);
    fadeOutAnimation->setStartValue(1.0);
    fadeOutAnimation->setEndValue(0.0);
    fadeOutAnimation->setEasingCurve(QEasingCurve::InCubic);
    
    // 环形进度条定时器
    progressTimer = new QTimer(this);
    progressTimer->setInterval(50);
    connect(progressTimer, &QTimer::timeout, this, &SplashScreen::updateProgress);
    
    // 淡出定时器 - 2秒后开始淡出
    fadeTimer = new QTimer(this);
    fadeTimer->setSingleShot(true);
    fadeTimer->setInterval(2000);
    connect(fadeTimer, &QTimer::timeout, this, &SplashScreen::fadeOut);
    
    // 连接动画完成信号
    connect(fadeInAnimation, &QPropertyAnimation::finished, this, [this]() {
        fadeTimer->start();
    });
    
    connect(fadeOutAnimation, &QPropertyAnimation::finished, this, [this]() {
        hide();
        emit splashFinished();
    });
}

void SplashScreen::paintEvent(QPaintEvent * /* event */)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // iOS风格背景 - 简洁的白色背景
    QRect rect = this->rect().adjusted(20, 20, -20, -20);
    painter.setBrush(QBrush(QColor(255, 255, 255, 250)));
    painter.setPen(QPen(QColor(200, 200, 200, 100), 1));
    painter.drawRoundedRect(rect, 20, 20);
    
    // 去除环形进度条，保持简洁的启动画面
}

void SplashScreen::showSplash()
{
    show();
    fadeIn();
}

void SplashScreen::hideSplash()
{
    if (!isFadingOut) {
        isFadingOut = true;
        fadeOut();
    }
}

void SplashScreen::updateProgress()
{
    // 进度条功能已移除，此函数保留以避免编译错误
}

void SplashScreen::fadeIn()
{
    fadeInAnimation->start();
}

void SplashScreen::fadeOut()
{
    isFadingOut = true;
    fadeOutAnimation->start();
}
