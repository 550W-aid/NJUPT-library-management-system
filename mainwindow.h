#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <QTableView>
#include "librarymanager.h"
#include "bookdialog.h"

class QScrollArea;
class QDockWidget;
class QLineEdit;
class QPushButton;

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    
    // 用户模式管理
    void setUserMode(bool isAdmin);
    void setCurrentUser(const QString &username);
    bool isAdminMode() const;
    QString getCurrentUser() const;

private:
    Ui::MainWindow *ui;
    LibraryManager library_;
    QStandardItemModel *model_ = nullptr;
    QTableView *tableView_ = nullptr;
    
    // 用户模式相关
    bool adminMode_;
    QString currentUser_;
    
    // 搜索框相关
    QLineEdit *searchEdit_;
    QPushButton *searchButton_;
    
    // 主题相关
    bool isDarkMode_;
    QPushButton *themeToggleButton_;
    
    // 菜单栏相关
    QMenuBar *menuBar_;
    QMenu *bookMenu_;
    QMenu *queryMenu_;
    QMenu *sortMenu_;
    QMenu *dataMenu_;
    QMenu *systemMenu_;
    
    // 字体相关
    QFont currentFont_;
    int fontSize_;

private:
    void setupTable();
    void refreshTable(const QVector<Book> &books);
    void setupActions();
    void setupStyles();
    void updateUIForUserMode();
    void updateStatusBar();
    QDockWidget* createDockWidgetFromScrollArea(QScrollArea *scrollArea);
    void setupSearchBar();
    void setupThemeToggle();
    void toggleTheme();
    void applyTheme(bool isDark);
    QString getThemeStyles(bool isDark);
    void initializeSampleBooks();
    void setupMenuBar();
    void setupFontSettings();
    void applyFontSettings();

private slots:
    void onAdd();
    void onEdit();
    void onRemove();
    void onBorrow();
    void onReturn();
    void onSearch();
    void onShowDue();
    void onSortByBorrow();
    void onOpen();
    void onSave();
    void onShowAll();
    void onSwitchMode();
    
    // 新增实用功能
    void onFilterByCategory();
    void onFilterByLocation();
    void onShowAvailable();
    void onShowBorrowed();
    void onShowTopBorrowed();
    void onShowRecentlyAdded();
    void onShowExpensiveBooks();
    void onShowCheapBooks();
    void onShowStatistics();
    void onSortByName();
    void onSortByCategory();
    void onSortByLocation();
    void onSortByPrice();
    void onSortByDate();
    void onSortByBorrowCount();
    void onAdvancedSearch();
    void onExportData();
    void onImportData();
    void onBackupData();
    void onRestoreData();
    void onFontSettings();
    void onFontSizeChanged(int size);
    void onFontFamilyChanged(const QString &family);
};
#endif // MAINWINDOW_H
