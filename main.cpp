#include "mainwindow.h"
#include "splashscreen.h"
#include "logindialog.h"

#include <QApplication>
#include <QTimer>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
    // 创建启动界面
    SplashScreen splash;
    splash.showSplash();
    
    // 创建主窗口
    MainWindow w;
    
    // 连接启动界面完成信号到登录界面显示
    QObject::connect(&splash, &SplashScreen::splashFinished, [&w]() {
        // 使用QTimer确保启动动画完全结束后再显示登录界面
        QTimer::singleShot(100, [&w]() {
            // 创建登录对话框
            LoginDialog loginDialog;
            loginDialog.show();
            loginDialog.raise();
            loginDialog.activateWindow();
            
            if (loginDialog.exec() == QDialog::Accepted) {
                // 设置用户模式
                w.setUserMode(loginDialog.isAdminMode());
                w.setCurrentUser(loginDialog.getUsername());
                
                // 显示主窗口
                w.show();
                w.raise();
                w.activateWindow();
            } else {
                // 用户取消登录，退出程序
                QApplication::quit();
            }
        });
    });
    
    return a.exec();
}
