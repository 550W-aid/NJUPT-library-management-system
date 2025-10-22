#ifndef SPLASHSCREEN_H
#define SPLASHSCREEN_H

#include <QWidget>
#include <QLabel>
#include <QProgressBar>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTimer>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QFont>
#include <QPainter>
#include <QLinearGradient>

class SplashScreen : public QWidget
{
    Q_OBJECT

public:
    explicit SplashScreen(QWidget *parent = nullptr);
    ~SplashScreen();

    void showSplash();
    void hideSplash();

signals:
    void splashFinished();

private slots:
    void updateProgress();
    void fadeIn();
    void fadeOut();

private:
    void setupUI();
    void setupAnimations();
    void paintEvent(QPaintEvent *event) override;

    QLabel *titleLabel;
    QLabel *subtitleLabel;
    QLabel *designerLabel;
    QLabel *universityLabel;
    QTimer *fadeTimer;
    QTimer *progressTimer;
    
    QPropertyAnimation *fadeInAnimation;
    QPropertyAnimation *fadeOutAnimation;
    QGraphicsOpacityEffect *opacityEffect;
    
    bool isFadingOut;
    int progressValue;
};

#endif // SPLASHSCREEN_H
