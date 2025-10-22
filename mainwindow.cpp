#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QTableView>
#include <QAbstractItemView>
#include <QStandardItemModel>
#include <QToolBar>
#include <QInputDialog>
#include <QDate>
#include <QIcon>
#include <QStatusBar>
#include <QFileInfo>
#include <QHeaderView>
#include <QScrollArea>
#include <QDockWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QSet>
#include <QDateTime>
#include <QMenuBar>
#include <QMenu>
#include <QAction>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , adminMode_(false)
    , currentUser_("")
    , searchEdit_(nullptr)
    , searchButton_(nullptr)
    , isDarkMode_(false)
    , themeToggleButton_(nullptr)
{
    ui->setupUi(this);
    setupTable();
    setupMenuBar();
    setupActions();
    setupSearchBar();
    setupThemeToggle();
    initializeSampleBooks();
    setupStyles();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupTable()
{
    model_ = new QStandardItemModel(this);
    model_->setHorizontalHeaderLabels({
        QStringLiteral("索引号"), QStringLiteral("名称"), QStringLiteral("馆藏地址"),
        QStringLiteral("类别"), QStringLiteral("数量"), QStringLiteral("价格"),
        QStringLiteral("入库日期"), QStringLiteral("归还日期"), QStringLiteral("借阅次数"),
        QStringLiteral("状态")
    });
    
    // 创建表格视图
    tableView_ = new QTableView(this);
    tableView_->setModel(model_);
    tableView_->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    
    // 设置表格自适应列宽
    tableView_->horizontalHeader()->setStretchLastSection(true);
    tableView_->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    tableView_->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch); // 名称列自适应
    tableView_->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch); // 馆藏地址列自适应
    
    // 设置表格自适应行高
    tableView_->verticalHeader()->setDefaultSectionSize(40);
    tableView_->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    
    // 设置表格基本属性
    tableView_->setAlternatingRowColors(true);
    
    // 将表格添加到中央布局
    ui->centralLayout->addWidget(tableView_);
}

void MainWindow::refreshTable(const QVector<Book> &books)
{
    model_->removeRows(0, model_->rowCount());
    model_->setRowCount(books.size());
    for (int row = 0; row < books.size(); ++row) {
        const Book &b = books[row];
        model_->setItem(row, 0, new QStandardItem(b.indexId));
        model_->setItem(row, 1, new QStandardItem(b.name));
        model_->setItem(row, 2, new QStandardItem(b.location));
        model_->setItem(row, 3, new QStandardItem(b.category));
        model_->setItem(row, 4, new QStandardItem(QString::number(b.quantity)));
        model_->setItem(row, 5, new QStandardItem(QString::number(b.price, 'f', 2)));
        model_->setItem(row, 6, new QStandardItem(b.inDate.isValid() ? b.inDate.toString(Qt::ISODate) : QString()));
        model_->setItem(row, 7, new QStandardItem(b.returnDate.isValid() ? b.returnDate.toString(Qt::ISODate) : QString()));
        model_->setItem(row, 8, new QStandardItem(QString::number(b.borrowCount)));
        model_->setItem(row, 9, new QStandardItem(b.available ? QStringLiteral("✅ 可借") : QStringLiteral("❌ 不可借")));
    }
    
    // 更新状态栏信息
    int totalBooks = books.size();
    int availableBooks = 0;
    int borrowedBooks = 0;
    for (const auto &book : books) {
        if (book.available) {
            availableBooks++;
        } else {
            borrowedBooks++;
        }
    }
    
    QString statusText = QStringLiteral("📊 总计: %1 本图书 | ✅ 可借: %2 本 | ❌ 已借出: %3 本")
                        .arg(totalBooks).arg(availableBooks).arg(borrowedBooks);
    statusBar()->showMessage(statusText);
}

void MainWindow::setupActions()
{
    // 创建左侧工具栏
    auto *bar = addToolBar(QStringLiteral("操作"));
    bar->setMovable(false);
    bar->setFloatable(false);
    bar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    bar->setOrientation(Qt::Vertical);  // 设置为垂直方向
    bar->setAllowedAreas(Qt::LeftToolBarArea);  // 限制在左侧
    
    // 创建滚动区域来包装工具栏
    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setWidget(bar);
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setMaximumWidth(180);  // 限制最大宽度
    scrollArea->setMinimumWidth(130);  // 设置最小宽度
    
    // 滚动区域样式由主题系统控制
    
    // 将滚动区域添加到左侧停靠区域
    addDockWidget(Qt::LeftDockWidgetArea, createDockWidgetFromScrollArea(scrollArea));
    
    // 使用Unicode图标美化工具栏按钮
    auto addAct = bar->addAction(QStringLiteral("📚 新增"));
    auto editAct = bar->addAction(QStringLiteral("✏️ 编辑"));
    auto delAct = bar->addAction(QStringLiteral("🗑️ 删除"));
    bar->addSeparator();
    auto borrowAct = bar->addAction(QStringLiteral("📖 借书"));
    auto returnAct = bar->addAction(QStringLiteral("📤 还书"));
    bar->addSeparator();
    auto dueAct = bar->addAction(QStringLiteral("⏰ 到期(3天内)"));
    auto sortAct = bar->addAction(QStringLiteral("📊 按借阅次数排序"));
    bar->addSeparator();
    auto openAct = bar->addAction(QStringLiteral("📂 打开"));
    auto saveAct = bar->addAction(QStringLiteral("💾 保存"));
    auto allAct = bar->addAction(QStringLiteral("📋 显示全部"));
    bar->addSeparator();
    auto switchAct = bar->addAction(QStringLiteral("🔄 切换模式"));
    
    // 新增实用功能按钮
    bar->addSeparator();
    auto filterCategoryAct = bar->addAction(QStringLiteral("📂 按分类筛选"));
    auto filterLocationAct = bar->addAction(QStringLiteral("📍 按位置筛选"));
    auto availableAct = bar->addAction(QStringLiteral("✅ 可借图书"));
    auto borrowedAct = bar->addAction(QStringLiteral("📖 已借图书"));
    auto topBorrowedAct = bar->addAction(QStringLiteral("🔥 热门图书"));
    auto recentAct = bar->addAction(QStringLiteral("🆕 最新图书"));
    auto expensiveAct = bar->addAction(QStringLiteral("💰 高价图书"));
    auto cheapAct = bar->addAction(QStringLiteral("💸 低价图书"));
    auto statisticsAct = bar->addAction(QStringLiteral("📊 统计信息"));
    
    bar->addSeparator();
    auto sortNameAct = bar->addAction(QStringLiteral("🔤 按名称排序"));
    auto sortCategoryAct = bar->addAction(QStringLiteral("📚 按分类排序"));
    auto sortLocationAct = bar->addAction(QStringLiteral("📍 按位置排序"));
    auto sortPriceAct = bar->addAction(QStringLiteral("💵 按价格排序"));
    auto sortDateAct = bar->addAction(QStringLiteral("📅 按日期排序"));
    auto sortBorrowAct = bar->addAction(QStringLiteral("📈 按借阅排序"));
    
    bar->addSeparator();
    auto advancedSearchAct = bar->addAction(QStringLiteral("🔍 高级搜索"));
    auto exportAct = bar->addAction(QStringLiteral("📤 导出数据"));
    auto importAct = bar->addAction(QStringLiteral("📥 导入数据"));
    auto backupAct = bar->addAction(QStringLiteral("💾 备份数据"));
    auto restoreAct = bar->addAction(QStringLiteral("🔄 恢复数据"));

    connect(addAct, &QAction::triggered, this, &MainWindow::onAdd);
    connect(editAct, &QAction::triggered, this, &MainWindow::onEdit);
    connect(delAct, &QAction::triggered, this, &MainWindow::onRemove);
    connect(borrowAct, &QAction::triggered, this, &MainWindow::onBorrow);
    connect(returnAct, &QAction::triggered, this, &MainWindow::onReturn);
    connect(dueAct, &QAction::triggered, this, &MainWindow::onShowDue);
    connect(sortAct, &QAction::triggered, this, &MainWindow::onSortByBorrow);
    connect(openAct, &QAction::triggered, this, &MainWindow::onOpen);
    connect(saveAct, &QAction::triggered, this, &MainWindow::onSave);
    connect(allAct, &QAction::triggered, this, &MainWindow::onShowAll);
    connect(switchAct, &QAction::triggered, this, &MainWindow::onSwitchMode);
    
    // 连接新增功能
    connect(filterCategoryAct, &QAction::triggered, this, &MainWindow::onFilterByCategory);
    connect(filterLocationAct, &QAction::triggered, this, &MainWindow::onFilterByLocation);
    connect(availableAct, &QAction::triggered, this, &MainWindow::onShowAvailable);
    connect(borrowedAct, &QAction::triggered, this, &MainWindow::onShowBorrowed);
    connect(topBorrowedAct, &QAction::triggered, this, &MainWindow::onShowTopBorrowed);
    connect(recentAct, &QAction::triggered, this, &MainWindow::onShowRecentlyAdded);
    connect(expensiveAct, &QAction::triggered, this, &MainWindow::onShowExpensiveBooks);
    connect(cheapAct, &QAction::triggered, this, &MainWindow::onShowCheapBooks);
    connect(statisticsAct, &QAction::triggered, this, &MainWindow::onShowStatistics);
    connect(sortNameAct, &QAction::triggered, this, &MainWindow::onSortByName);
    connect(sortCategoryAct, &QAction::triggered, this, &MainWindow::onSortByCategory);
    connect(sortLocationAct, &QAction::triggered, this, &MainWindow::onSortByLocation);
    connect(sortPriceAct, &QAction::triggered, this, &MainWindow::onSortByPrice);
    connect(sortDateAct, &QAction::triggered, this, &MainWindow::onSortByDate);
    connect(sortBorrowAct, &QAction::triggered, this, &MainWindow::onSortByBorrowCount);
    connect(advancedSearchAct, &QAction::triggered, this, &MainWindow::onAdvancedSearch);
    connect(exportAct, &QAction::triggered, this, &MainWindow::onExportData);
    connect(importAct, &QAction::triggered, this, &MainWindow::onImportData);
    connect(backupAct, &QAction::triggered, this, &MainWindow::onBackupData);
    connect(restoreAct, &QAction::triggered, this, &MainWindow::onRestoreData);
}

void MainWindow::onAdd()
{
    // 检查权限
    if (!adminMode_) {
        QMessageBox::warning(this, QStringLiteral("❌ 权限不足"), QStringLiteral("读者模式无法添加图书，请切换到管理员模式"));
        return;
    }
    
    BookDialog dlg(this);
    if (dlg.exec() == QDialog::Accepted) {
        QString err;
        Book b = dlg.getBook();
        if (!library_.addBook(b, &err)) {
            QMessageBox::warning(this, QStringLiteral("❌ 新增失败"), err);
            return;
        }
        refreshTable(library_.getAll());
        statusBar()->showMessage(QStringLiteral("✅ 成功添加图书: %1").arg(b.name), 3000);
    }
}

void MainWindow::onEdit()
{
    // 检查权限
    if (!adminMode_) {
        QMessageBox::warning(this, QStringLiteral("❌ 权限不足"), QStringLiteral("读者模式无法编辑图书，请切换到管理员模式"));
        return;
    }
    
    if (!tableView_) return;
    const auto idx = tableView_->currentIndex();
    if (!idx.isValid()) {
        QMessageBox::information(this, QStringLiteral("ℹ️ 提示"), QStringLiteral("请先选择要编辑的图书"));
        return;
    }
    const QString indexId = model_->item(idx.row(), 0)->text();
    const auto all = library_.getAll();
    int pos = -1;
    for (int i = 0; i < all.size(); ++i) if (all[i].indexId == indexId) { pos = i; break; }
    if (pos < 0) return;
    BookDialog dlg(this);
    dlg.setBook(all[pos]);
    if (dlg.exec() == QDialog::Accepted) {
        QString err;
        if (!library_.updateBook(indexId, dlg.getBook(), &err)) {
            QMessageBox::warning(this, QStringLiteral("❌ 编辑失败"), err);
            return;
        }
        refreshTable(library_.getAll());
        statusBar()->showMessage(QStringLiteral("✅ 成功编辑图书: %1").arg(dlg.getBook().name), 3000);
    }
}

void MainWindow::onRemove()
{
    // 检查权限
    if (!adminMode_) {
        QMessageBox::warning(this, QStringLiteral("❌ 权限不足"), QStringLiteral("读者模式无法删除图书，请切换到管理员模式"));
        return;
    }
    
    if (!tableView_) return;
    const auto idx = tableView_->currentIndex();
    if (!idx.isValid()) {
        QMessageBox::information(this, QStringLiteral("ℹ️ 提示"), QStringLiteral("请先选择要删除的图书"));
        return;
    }
    const QString indexId = model_->item(idx.row(), 0)->text();
    const QString bookName = model_->item(idx.row(), 1)->text();
    
    auto reply = QMessageBox::question(this, QStringLiteral("⚠️ 确认删除"), 
                                      QStringLiteral("确定要删除图书 \"%1\" 吗？").arg(bookName),
                                      QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        if (library_.removeBookByIndexId(indexId)) {
            refreshTable(library_.getAll());
            statusBar()->showMessage(QStringLiteral("✅ 成功删除图书: %1").arg(bookName), 3000);
        } else {
            QMessageBox::warning(this, QStringLiteral("❌ 删除失败"), QStringLiteral("删除图书时发生错误"));
        }
    }
}

void MainWindow::onBorrow()
{
    if (!tableView_) return;
    const auto idx = tableView_->currentIndex();
    if (!idx.isValid()) {
        QMessageBox::information(this, QStringLiteral("ℹ️ 提示"), QStringLiteral("请先选择要借阅的图书"));
        return;
    }
    const QString indexId = model_->item(idx.row(), 0)->text();
    const QString bookName = model_->item(idx.row(), 1)->text();
    
    // 检查图书是否可借
    if (model_->item(idx.row(), 9)->text().contains("❌")) {
        QMessageBox::warning(this, QStringLiteral("❌ 借书失败"), QStringLiteral("该图书已被借出，无法再次借阅"));
        return;
    }
    
    const QString dueStr = QInputDialog::getText(this, QStringLiteral("📖 借书"), 
                                                QStringLiteral("请输入归还日期 (yyyy-MM-dd)\n图书: %1").arg(bookName))
            .trimmed();
    if (dueStr.isEmpty()) return;
    
    QDate dueDate = QDate::fromString(dueStr, Qt::ISODate);
    if (!dueDate.isValid()) {
        QMessageBox::warning(this, QStringLiteral("❌ 日期无效"), QStringLiteral("请按 yyyy-MM-dd 格式输入日期"));
        return;
    }
    
    QString err;
    if (!library_.borrowBook(indexId, dueDate, &err)) {
        QMessageBox::warning(this, QStringLiteral("❌ 借书失败"), err);
        return;
    }
    refreshTable(library_.getAll());
    statusBar()->showMessage(QStringLiteral("✅ 成功借阅图书: %1，归还日期: %2").arg(bookName, dueStr), 3000);
}

void MainWindow::onReturn()
{
    if (!tableView_) return;
    const auto idx = tableView_->currentIndex();
    if (!idx.isValid()) {
        QMessageBox::information(this, QStringLiteral("ℹ️ 提示"), QStringLiteral("请先选择要归还的图书"));
        return;
    }
    const QString indexId = model_->item(idx.row(), 0)->text();
    const QString bookName = model_->item(idx.row(), 1)->text();
    
    // 检查图书是否已借出
    if (model_->item(idx.row(), 9)->text().contains("✅")) {
        QMessageBox::warning(this, QStringLiteral("❌ 还书失败"), QStringLiteral("该图书未被借出，无需归还"));
        return;
    }
    
    auto reply = QMessageBox::question(this, QStringLiteral("📤 确认还书"), 
                                      QStringLiteral("确定要归还图书 \"%1\" 吗？").arg(bookName),
                                      QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        QString err;
        if (!library_.returnBook(indexId, &err)) {
            QMessageBox::warning(this, QStringLiteral("❌ 还书失败"), err);
            return;
        }
        refreshTable(library_.getAll());
        statusBar()->showMessage(QStringLiteral("✅ 成功归还图书: %1").arg(bookName), 3000);
    }
}

void MainWindow::onSearch()
{
    if (!searchEdit_) return;
    
    const QString name = searchEdit_->text().trimmed();
    if (name.isEmpty()) {
        QMessageBox::information(this, QStringLiteral("ℹ️ 提示"), QStringLiteral("请输入要搜索的图书名称"));
        return;
    }
    
    const Book *b = library_.findByName(name);
    if (!b) {
        QMessageBox::information(this, QStringLiteral("ℹ️ 未找到"), 
                                QStringLiteral("没有找到名称为 \"%1\" 的图书").arg(name));
        return;
    }
    refreshTable(QVector<Book>{ *b });
    statusBar()->showMessage(QStringLiteral("🔍 搜索到图书: %1").arg(b->name), 3000);
}

void MainWindow::onShowDue()
{
    auto dueBooks = library_.getDueInDays(3);
    refreshTable(dueBooks);
    if (dueBooks.isEmpty()) {
        statusBar()->showMessage(QStringLiteral("ℹ️ 没有3天内到期的图书"), 3000);
    } else {
        statusBar()->showMessage(QStringLiteral("⏰ 显示 %1 本3天内到期的图书").arg(dueBooks.size()), 3000);
    }
}

void MainWindow::onSortByBorrow()
{
    library_.sortByBorrowCountDesc();
    refreshTable(library_.getAll());
    statusBar()->showMessage(QStringLiteral("📊 已按借阅次数降序排列"), 3000);
}

void MainWindow::onOpen()
{
    const QString path = QFileDialog::getOpenFileName(this, QStringLiteral("📂 打开文件"), QString(), 
                                                     QStringLiteral("JSON 文件 (*.json);;所有文件 (*.*)"));
    if (path.isEmpty()) return;
    QString err;
    if (!library_.loadFromFile(path, &err)) {
        QMessageBox::warning(this, QStringLiteral("❌ 打开失败"), err);
        return;
    }
    refreshTable(library_.getAll());
    statusBar()->showMessage(QStringLiteral("✅ 成功打开文件: %1").arg(QFileInfo(path).fileName()), 3000);
}

void MainWindow::onSave()
{
    const QString path = QFileDialog::getSaveFileName(this, QStringLiteral("💾 保存文件"), QString(), 
                                                     QStringLiteral("JSON 文件 (*.json);;所有文件 (*.*)"));
    if (path.isEmpty()) return;
    QString err;
    if (!library_.saveToFile(path, &err)) {
        QMessageBox::warning(this, QStringLiteral("❌ 保存失败"), err);
        return;
    }
    statusBar()->showMessage(QStringLiteral("✅ 成功保存文件: %1").arg(QFileInfo(path).fileName()), 3000);
}

void MainWindow::onShowAll()
{
    refreshTable(library_.getAll());
    statusBar()->showMessage(QStringLiteral("📋 显示所有图书"), 3000);
}

void MainWindow::setupStyles()
{
    // 应用默认主题（浅色模式）
    applyTheme(false);
    
    // 设置窗口标题和图标
    setWindowTitle(QStringLiteral("图书管理系统"));
    setWindowIcon(QIcon(":/icons/library.svg"));
    
    // 设置状态栏
    statusBar()->setMinimumHeight(28);
    updateStatusBar();
}

// 用户模式管理方法
void MainWindow::setUserMode(bool isAdmin)
{
    adminMode_ = isAdmin;
    updateUIForUserMode();
    updateStatusBar();
}

void MainWindow::setCurrentUser(const QString &username)
{
    currentUser_ = username;
    updateStatusBar();
}

bool MainWindow::isAdminMode() const
{
    return adminMode_;
}

QString MainWindow::getCurrentUser() const
{
    return currentUser_;
}

void MainWindow::updateUIForUserMode()
{
    // 获取所有工具栏
    QList<QToolBar*> toolBars = findChildren<QToolBar*>();
    
    for (QToolBar *toolBar : toolBars) {
        // 获取所有工具栏按钮
        QList<QAction*> actions = toolBar->actions();
        
        // 根据用户模式启用/禁用功能
        for (QAction *action : actions) {
            QString actionText = action->text();
            
            if (adminMode_) {
                // 管理员模式：启用所有功能
                action->setEnabled(true);
            } else {
                // 读者模式：禁用删除和编辑功能
                if (actionText.contains("🗑️ 删除") || actionText.contains("✏️ 编辑")) {
                    action->setEnabled(false);
                } else {
                    action->setEnabled(true);
                }
            }
        }
    }
    
    // 更新菜单栏权限
    if (bookMenu_) {
        QList<QAction*> bookActions = bookMenu_->actions();
        for (QAction *action : bookActions) {
            QString actionText = action->text();
            if (adminMode_) {
                action->setEnabled(true);
            } else {
                // 读者模式：禁用编辑和删除功能
                if (actionText.contains("✏️ 编辑图书") || actionText.contains("🗑️ 删除图书")) {
                    action->setEnabled(false);
                } else {
                    action->setEnabled(true);
                }
            }
        }
    }
}

void MainWindow::updateStatusBar()
{
    QString modeText = adminMode_ ? "管理员模式" : "读者模式";
    QString userText = currentUser_.isEmpty() ? "未登录" : currentUser_;
    QString statusText = QStringLiteral("👤 用户: %1 | 🔐 模式: %2").arg(userText, modeText);
    statusBar()->showMessage(statusText);
    
    // 确保状态栏有足够的高度显示内容
    statusBar()->setMinimumHeight(28);
}

void MainWindow::onSwitchMode()
{
    // 切换模式需要重新登录
    QMessageBox::information(this, "切换模式", "请重新启动程序以切换用户模式");
}

QDockWidget* MainWindow::createDockWidgetFromScrollArea(QScrollArea *scrollArea)
{
    QDockWidget *dockWidget = new QDockWidget("功能栏", this);
    dockWidget->setWidget(scrollArea);
    dockWidget->setFeatures(QDockWidget::NoDockWidgetFeatures);  // 禁用停靠功能
    dockWidget->setTitleBarWidget(new QWidget());  // 隐藏标题栏
    dockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    
    // 停靠窗口样式由主题系统控制
    
    return dockWidget;
}

void MainWindow::setupSearchBar()
{
    // 创建搜索框容器
    QWidget *searchWidget = new QWidget();
    QHBoxLayout *searchLayout = new QHBoxLayout(searchWidget);
    searchLayout->setContentsMargins(16, 8, 16, 8);
    searchLayout->setSpacing(8);
    
    // 创建搜索输入框
    searchEdit_ = new QLineEdit();
    searchEdit_->setPlaceholderText("🔍 搜索图书名称...");
    
    // 创建搜索按钮
    searchButton_ = new QPushButton("搜索");
    
    // 创建主题切换按钮
    themeToggleButton_ = new QPushButton("🌙");
    themeToggleButton_->setToolTip("切换深浅色模式");
    
    // 添加到布局
    searchLayout->addWidget(searchEdit_);
    searchLayout->addWidget(searchButton_);
    searchLayout->addWidget(themeToggleButton_);
    
    // 将搜索栏添加到顶部工具栏
    QToolBar *searchToolBar = addToolBar("搜索");
    searchToolBar->setMovable(false);
    searchToolBar->setFloatable(false);
    searchToolBar->addWidget(searchWidget);
    searchToolBar->setAllowedAreas(Qt::TopToolBarArea);
    
    // 搜索工具栏样式由主题系统控制
    
    // 连接信号
    connect(searchButton_, &QPushButton::clicked, this, &MainWindow::onSearch);
    connect(searchEdit_, &QLineEdit::returnPressed, this, &MainWindow::onSearch);
    connect(themeToggleButton_, &QPushButton::clicked, this, &MainWindow::toggleTheme);
}

void MainWindow::setupThemeToggle()
{
    // 初始化主题状态（默认为浅色模式）
    isDarkMode_ = false;
}

void MainWindow::toggleTheme()
{
    isDarkMode_ = !isDarkMode_;
    applyTheme(isDarkMode_);
    
    // 更新按钮图标
    if (themeToggleButton_) {
        themeToggleButton_->setText(isDarkMode_ ? "☀️" : "🌙");
        themeToggleButton_->setToolTip(isDarkMode_ ? "切换到浅色模式" : "切换到深色模式");
    }
}

void MainWindow::applyTheme(bool isDark)
{
    QString styles = getThemeStyles(isDark);
    setStyleSheet(styles);
}

QString MainWindow::getThemeStyles(bool isDark)
{
    if (isDark) {
        // 深色主题样式
        return QStringLiteral(
            "QMainWindow {"
            "    background-color: #1C1C1E;"
            "    color: #FFFFFF;"
            "    font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif;"
            "}"
            "QToolBar {"
            "    background-color: #2C2C2E;"
            "    border: none;"
            "    border-right: 1px solid #3A3A3C;"
            "    border-bottom: 1px solid #3A3A3C;"
            "    spacing: 8px;"
            "    padding: 12px 8px;"
            "}"
            "QToolButton {"
            "    background-color: #3A3A3C;"
            "    color: #FFFFFF;"
            "    border: 1px solid #48484A;"
            "    border-radius: 12px;"
            "    padding: 10px 6px;"
            "    margin: 2px;"
            "    font-size: 13px;"
            "    font-weight: 600;"
            "    min-width: 110px;"
            "    min-height: 45px;"
            "    max-width: 150px;"
            "    font-family: 'Microsoft YaHei UI', 'PingFang SC', 'Hiragino Sans GB', 'WenQuanYi Micro Hei', sans-serif;"
            "    text-align: center;"
            "}"
            "QToolButton:hover {"
            "    background-color: #48484A;"
            "    border-color: #007AFF;"
            "    color: #007AFF;"
            "}"
            "QToolButton:pressed {"
            "    background-color: #007AFF;"
            "    color: #FFFFFF;"
            "    border-color: #0051D5;"
            "}"
            "QToolButton:disabled {"
            "    background-color: #2C2C2E;"
            "    color: #8E8E93;"
            "    border-color: #3A3A3C;"
            "}"
            "QStatusBar {"
            "    background-color: #2C2C2E;"
            "    color: #8E8E93;"
            "    border-top: 1px solid #3A3A3C;"
            "    padding: 6px 16px;"
            "    font-size: 14px;"
            "    font-family: 'Microsoft YaHei UI', 'PingFang SC', 'Hiragino Sans GB', 'WenQuanYi Micro Hei', sans-serif;"
            "    min-height: 28px;"
            "    height: 28px;"
            "    line-height: 1.4;"
            "}"
            "QTableView {"
            "    background-color: #1C1C1E;"
            "    alternate-background-color: #2C2C2E;"
            "    selection-background-color: #007AFF;"
            "    selection-color: #FFFFFF;"
            "    gridline-color: #3A3A3C;"
            "    border: 1px solid #3A3A3C;"
            "    border-radius: 12px;"
            "    gridline-width: 1px;"
            "    font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif;"
            "}"
            "QTableView::item {"
            "    padding: 12px 16px;"
            "    border: none;"
            "    min-height: 44px;"
            "    font-size: 15px;"
            "    color: #FFFFFF;"
            "    font-family: 'Microsoft YaHei UI', 'PingFang SC', 'Hiragino Sans GB', 'WenQuanYi Micro Hei', sans-serif;"
            "}"
            "QTableView::item:selected {"
            "    background-color: #007AFF;"
            "    color: #FFFFFF;"
            "}"
            "QTableView::item:hover {"
            "    background-color: #2C2C2E;"
            "}"
            "QHeaderView::section {"
            "    background-color: #2C2C2E;"
            "    color: #FFFFFF;"
            "    padding: 16px 12px;"
            "    border: none;"
            "    font-weight: 600;"
            "    font-size: 15px;"
            "    min-height: 44px;"
            "    border-bottom: 1px solid #3A3A3C;"
            "    font-family: 'Microsoft YaHei UI', 'PingFang SC', 'Hiragino Sans GB', 'WenQuanYi Micro Hei', sans-serif;"
            "}"
            "QHeaderView::section:hover {"
            "    background-color: #3A3A3C;"
            "}"
            "QLineEdit {"
            "    background-color: #3A3A3C;"
            "    border: 2px solid #48484A;"
            "    border-radius: 20px;"
            "    padding: 8px 16px;"
            "    font-size: 14px;"
            "    color: #FFFFFF;"
            "    font-family: 'Microsoft YaHei UI', 'PingFang SC', 'Hiragino Sans GB', 'WenQuanYi Micro Hei', sans-serif;"
            "    min-height: 20px;"
            "}"
            "QLineEdit:focus {"
            "    border-color: #007AFF;"
            "    background-color: #48484A;"
            "}"
            "QLineEdit:hover {"
            "    border-color: #5A5A5C;"
            "}"
            "QPushButton {"
            "    background-color: #007AFF;"
            "    color: #FFFFFF;"
            "    border: none;"
            "    border-radius: 20px;"
            "    padding: 8px 20px;"
            "    font-size: 14px;"
            "    font-weight: 600;"
            "    font-family: 'Microsoft YaHei UI', 'PingFang SC', 'Hiragino Sans GB', 'WenQuanYi Micro Hei', sans-serif;"
            "    min-width: 60px;"
            "    min-height: 20px;"
            "}"
            "QPushButton:hover {"
            "    background-color: #0051D5;"
            "}"
            "QPushButton:pressed {"
            "    background-color: #0041B8;"
            "}"
            "QDockWidget {"
            "    background-color: #2C2C2E;"
            "    border: none;"
            "    border-right: 1px solid #3A3A3C;"
            "}"
            "QScrollArea {"
            "    background-color: #2C2C2E;"
            "    border: none;"
            "}"
            "QScrollBar:vertical {"
            "    background-color: #3A3A3C;"
            "    width: 8px;"
            "    border-radius: 4px;"
            "}"
            "QScrollBar::handle:vertical {"
            "    background-color: #5A5A5C;"
            "    border-radius: 4px;"
            "    min-height: 20px;"
            "}"
            "QScrollBar::handle:vertical:hover {"
            "    background-color: #6A6A6C;"
            "}"
        );
    } else {
        // 浅色主题样式
        return QStringLiteral(
            "QMainWindow {"
            "    background-color: #F2F2F7;"
            "    font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif;"
            "}"
            "QToolBar {"
            "    background-color: #F2F2F7;"
            "    border: none;"
            "    border-right: 1px solid #E5E5EA;"
            "    border-bottom: 1px solid #E5E5EA;"
            "    spacing: 8px;"
            "    padding: 12px 8px;"
            "}"
            "QToolButton {"
            "    background-color: #FFFFFF;"
            "    border: 1px solid #E5E5EA;"
            "    border-radius: 12px;"
            "    padding: 10px 6px;"
            "    margin: 2px;"
            "    font-size: 13px;"
            "    font-weight: 600;"
            "    color: #1C1C1E;"
            "    min-width: 110px;"
            "    min-height: 45px;"
            "    max-width: 150px;"
            "    font-family: 'Microsoft YaHei UI', 'PingFang SC', 'Hiragino Sans GB', 'WenQuanYi Micro Hei', sans-serif;"
            "    text-align: center;"
            "}"
            "QToolButton:hover {"
            "    background-color: #F2F2F7;"
            "    border-color: #007AFF;"
            "    color: #007AFF;"
            "}"
            "QToolButton:pressed {"
            "    background-color: #007AFF;"
            "    color: #FFFFFF;"
            "    border-color: #0051D5;"
            "}"
            "QToolButton:disabled {"
            "    background-color: #F2F2F7;"
            "    color: #8E8E93;"
            "    border-color: #E5E5EA;"
            "}"
            "QStatusBar {"
            "    background-color: #F2F2F7;"
            "    color: #8E8E93;"
            "    border-top: 1px solid #E5E5EA;"
            "    padding: 6px 16px;"
            "    font-size: 14px;"
            "    font-family: 'Microsoft YaHei UI', 'PingFang SC', 'Hiragino Sans GB', 'WenQuanYi Micro Hei', sans-serif;"
            "    min-height: 28px;"
            "    height: 28px;"
            "    line-height: 1.4;"
            "}"
            "QTableView {"
            "    background-color: #FFFFFF;"
            "    alternate-background-color: #F8F9FA;"
            "    selection-background-color: #007AFF;"
            "    selection-color: #FFFFFF;"
            "    gridline-color: #E5E5EA;"
            "    border: 1px solid #E5E5EA;"
            "    border-radius: 12px;"
            "    gridline-width: 1px;"
            "    font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif;"
            "}"
            "QTableView::item {"
            "    padding: 12px 16px;"
            "    border: none;"
            "    min-height: 44px;"
            "    font-size: 15px;"
            "    font-family: 'Microsoft YaHei UI', 'PingFang SC', 'Hiragino Sans GB', 'WenQuanYi Micro Hei', sans-serif;"
            "}"
            "QTableView::item:selected {"
            "    background-color: #007AFF;"
            "    color: #FFFFFF;"
            "}"
            "QTableView::item:hover {"
            "    background-color: #F2F2F7;"
            "}"
            "QHeaderView::section {"
            "    background-color: #F8F9FA;"
            "    color: #1C1C1E;"
            "    padding: 16px 12px;"
            "    border: none;"
            "    font-weight: 600;"
            "    font-size: 15px;"
            "    min-height: 44px;"
            "    border-bottom: 1px solid #E5E5EA;"
            "    font-family: 'Microsoft YaHei UI', 'PingFang SC', 'Hiragino Sans GB', 'WenQuanYi Micro Hei', sans-serif;"
            "}"
            "QHeaderView::section:hover {"
            "    background-color: #F2F2F7;"
            "}"
            "QLineEdit {"
            "    background-color: #FFFFFF;"
            "    border: 2px solid #E5E5EA;"
            "    border-radius: 20px;"
            "    padding: 8px 16px;"
            "    font-size: 14px;"
            "    color: #1C1C1E;"
            "    font-family: 'Microsoft YaHei UI', 'PingFang SC', 'Hiragino Sans GB', 'WenQuanYi Micro Hei', sans-serif;"
            "    min-height: 20px;"
            "}"
            "QLineEdit:focus {"
            "    border-color: #007AFF;"
            "    background-color: #FFFFFF;"
            "}"
            "QLineEdit:hover {"
            "    border-color: #C7C7CC;"
            "}"
            "QPushButton {"
            "    background-color: #007AFF;"
            "    color: #FFFFFF;"
            "    border: none;"
            "    border-radius: 20px;"
            "    padding: 8px 20px;"
            "    font-size: 14px;"
            "    font-weight: 600;"
            "    font-family: 'Microsoft YaHei UI', 'PingFang SC', 'Hiragino Sans GB', 'WenQuanYi Micro Hei', sans-serif;"
            "    min-width: 60px;"
            "    min-height: 20px;"
            "}"
            "QPushButton:hover {"
            "    background-color: #0051D5;"
            "}"
            "QPushButton:pressed {"
            "    background-color: #0041B8;"
            "}"
            "QDockWidget {"
            "    background-color: #F2F2F7;"
            "    border: none;"
            "    border-right: 1px solid #E5E5EA;"
            "}"
            "QScrollArea {"
            "    background-color: #F2F2F7;"
            "    border: none;"
            "}"
            "QScrollBar:vertical {"
            "    background-color: #E5E5EA;"
            "    width: 8px;"
            "    border-radius: 4px;"
            "}"
            "QScrollBar::handle:vertical {"
            "    background-color: #C7C7CC;"
            "    border-radius: 4px;"
            "    min-height: 20px;"
            "}"
            "QScrollBar::handle:vertical:hover {"
            "    background-color: #AEAEB2;"
            "}"
        );
    }
}

void MainWindow::initializeSampleBooks()
{
    // 检查是否已有图书数据，如果有则不添加示例数据
    if (!library_.getAll().isEmpty()) {
        return;
    }
    
    // 创建示例图书数据
    QVector<Book> sampleBooks = {
        // 计算机类图书
        Book{"CS001", "C++程序设计教程", "仙林图书馆", "计算机科学", 5, 45.80, QDate(2023, 1, 15), QDate(), 12, true},
        Book{"CS002", "数据结构与算法分析", "三牌楼图书馆", "计算机科学", 3, 68.50, QDate(2023, 2, 20), QDate(), 8, true},
        Book{"CS003", "操作系统概念", "仙林图书馆", "计算机科学", 4, 89.00, QDate(2023, 3, 10), QDate(), 15, true},
        Book{"CS004", "计算机网络", "三牌楼图书馆", "计算机科学", 6, 76.20, QDate(2023, 1, 25), QDate(), 9, true},
        Book{"CS005", "数据库系统概论", "仙林图书馆", "计算机科学", 2, 92.50, QDate(2023, 4, 5), QDate(), 6, true},
        
        // 文学类图书
        Book{"LIT001", "红楼梦", "三牌楼图书馆", "文学", 8, 35.60, QDate(2023, 1, 10), QDate(), 25, true},
        Book{"LIT002", "百年孤独", "仙林图书馆", "文学", 4, 42.80, QDate(2023, 2, 15), QDate(), 18, true},
        Book{"LIT003", "活着", "三牌楼图书馆", "文学", 6, 28.90, QDate(2023, 3, 1), QDate(), 22, true},
        Book{"LIT004", "平凡的世界", "仙林图书馆", "文学", 5, 55.00, QDate(2023, 1, 20), QDate(), 16, true},
        Book{"LIT005", "围城", "三牌楼图书馆", "文学", 3, 38.50, QDate(2023, 2, 28), QDate(), 14, true},
        
        // 历史类图书
        Book{"HIS001", "中国通史", "仙林图书馆", "历史", 4, 78.00, QDate(2023, 1, 5), QDate(), 11, true},
        Book{"HIS002", "世界文明史", "三牌楼图书馆", "历史", 3, 85.50, QDate(2023, 3, 15), QDate(), 7, true},
        Book{"HIS003", "明朝那些事儿", "仙林图书馆", "历史", 6, 48.80, QDate(2023, 2, 10), QDate(), 20, true},
        Book{"HIS004", "人类简史", "三牌楼图书馆", "历史", 5, 65.20, QDate(2023, 4, 1), QDate(), 13, true},
        
        // 科学类图书
        Book{"SCI001", "时间简史", "仙林图书馆", "科学", 3, 52.00, QDate(2023, 1, 30), QDate(), 9, true},
        Book{"SCI002", "物种起源", "三牌楼图书馆", "科学", 2, 68.80, QDate(2023, 3, 20), QDate(), 5, true},
        Book{"SCI003", "相对论", "仙林图书馆", "科学", 1, 75.50, QDate(2023, 2, 25), QDate(), 3, true},
        Book{"SCI004", "量子力学原理", "三牌楼图书馆", "科学", 2, 88.00, QDate(2023, 4, 10), QDate(), 4, true},
        
        // 外语类图书
        Book{"ENG001", "新概念英语", "仙林图书馆", "外语", 10, 32.50, QDate(2023, 1, 12), QDate(), 35, true},
        Book{"ENG002", "托福词汇精选", "三牌楼图书馆", "外语", 8, 45.80, QDate(2023, 2, 18), QDate(), 28, true},
        Book{"ENG003", "雅思考试指南", "仙林图书馆", "外语", 6, 58.20, QDate(2023, 3, 8), QDate(), 19, true},
        Book{"ENG004", "商务英语", "三牌楼图书馆", "外语", 4, 42.00, QDate(2023, 1, 28), QDate(), 12, true},
        
        // 艺术类图书
        Book{"ART001", "西方美术史", "仙林图书馆", "艺术", 3, 72.50, QDate(2023, 2, 5), QDate(), 8, true},
        Book{"ART002", "中国书法艺术", "三牌楼图书馆", "艺术", 2, 55.80, QDate(2023, 3, 12), QDate(), 6, true},
        Book{"ART003", "音乐理论基础", "仙林图书馆", "艺术", 4, 48.00, QDate(2023, 1, 18), QDate(), 10, true},
        
        // 哲学类图书
        Book{"PHI001", "论语", "三牌楼图书馆", "哲学", 5, 25.80, QDate(2023, 1, 8), QDate(), 17, true},
        Book{"PHI002", "道德经", "仙林图书馆", "哲学", 4, 22.50, QDate(2023, 2, 22), QDate(), 14, true},
        Book{"PHI003", "苏菲的世界", "三牌楼图书馆", "哲学", 3, 38.80, QDate(2023, 3, 25), QDate(), 11, true},
        
        // 一些已借出的图书
        Book{"CS006", "人工智能导论", "仙林图书馆", "计算机科学", 2, 95.00, QDate(2023, 4, 15), QDate(2024, 1, 15), 3, false},
        Book{"LIT006", "1984", "三牌楼图书馆", "文学", 3, 36.50, QDate(2023, 2, 8), QDate(2024, 1, 20), 7, false},
        Book{"ENG005", "英语语法大全", "仙林图书馆", "外语", 5, 52.80, QDate(2023, 3, 18), QDate(2024, 1, 25), 9, false},
        Book{"SCI005", "宇宙的奥秘", "三牌楼图书馆", "科学", 2, 68.00, QDate(2023, 1, 22), QDate(2024, 1, 30), 5, false}
    };
    
    // 添加示例图书到图书馆
    for (const Book &book : sampleBooks) {
        QString error;
        library_.addBook(book, &error);
    }
    
    // 刷新表格显示
    refreshTable(library_.getAll());
    
    // 更新状态栏
    statusBar()->showMessage(QStringLiteral("📚 已加载 %1 本示例图书").arg(sampleBooks.size()), 3000);
}

void MainWindow::setupMenuBar()
{
    // 创建菜单栏
    menuBar_ = menuBar();
    menuBar_->setStyleSheet(
        "QMenuBar {"
        "    background-color: #F8F9FA;"
        "    color: #1C1C1E;"
        "    border-bottom: 1px solid #E5E5EA;"
        "    font-size: 14px;"
        "    font-family: 'Microsoft YaHei UI', 'PingFang SC', 'Hiragino Sans GB', 'WenQuanYi Micro Hei', sans-serif;"
        "    padding: 4px 8px;"
        "}"
        "QMenuBar::item {"
        "    background-color: transparent;"
        "    padding: 6px 12px;"
        "    border-radius: 6px;"
        "    margin: 2px;"
        "}"
        "QMenuBar::item:selected {"
        "    background-color: #E5E5EA;"
        "    color: #007AFF;"
        "}"
        "QMenuBar::item:pressed {"
        "    background-color: #D1D1D6;"
        "}"
        "QMenu {"
        "    background-color: #FFFFFF;"
        "    border: 1px solid #E5E5EA;"
        "    border-radius: 8px;"
        "    padding: 4px;"
        "    font-size: 14px;"
        "    font-family: 'Microsoft YaHei UI', 'PingFang SC', 'Hiragino Sans GB', 'WenQuanYi Micro Hei', sans-serif;"
        "}"
        "QMenu::item {"
        "    padding: 8px 16px;"
        "    border-radius: 4px;"
        "    margin: 1px;"
        "    min-width: 120px;"
        "}"
        "QMenu::item:selected {"
        "    background-color: #007AFF;"
        "    color: #FFFFFF;"
        "}"
        "QMenu::separator {"
        "    height: 1px;"
        "    background-color: #E5E5EA;"
        "    margin: 4px 8px;"
        "}"
    );
    
    // 1. 图书管理菜单
    bookMenu_ = menuBar_->addMenu("📚 图书管理");
    
    QAction *addBookAction = bookMenu_->addAction("📖 新增图书");
    QAction *editBookAction = bookMenu_->addAction("✏️ 编辑图书");
    QAction *deleteBookAction = bookMenu_->addAction("🗑️ 删除图书");
    bookMenu_->addSeparator();
    QAction *borrowBookAction = bookMenu_->addAction("📖 借阅图书");
    QAction *returnBookAction = bookMenu_->addAction("📤 归还图书");
    bookMenu_->addSeparator();
    QAction *showAllAction = bookMenu_->addAction("📋 显示全部");
    
    // 连接信号
    connect(addBookAction, &QAction::triggered, this, &MainWindow::onAdd);
    connect(editBookAction, &QAction::triggered, this, &MainWindow::onEdit);
    connect(deleteBookAction, &QAction::triggered, this, &MainWindow::onRemove);
    connect(borrowBookAction, &QAction::triggered, this, &MainWindow::onBorrow);
    connect(returnBookAction, &QAction::triggered, this, &MainWindow::onReturn);
    connect(showAllAction, &QAction::triggered, this, &MainWindow::onShowAll);
    
    // 2. 查询筛选菜单
    queryMenu_ = menuBar_->addMenu("🔍 查询筛选");
    
    QAction *searchAction = queryMenu_->addAction("🔍 搜索图书");
    QAction *filterCategoryAction = queryMenu_->addAction("📂 按分类筛选");
    QAction *filterLocationAction = queryMenu_->addAction("📍 按位置筛选");
    queryMenu_->addSeparator();
    QAction *showAvailableAction = queryMenu_->addAction("✅ 可借图书");
    QAction *showBorrowedAction = queryMenu_->addAction("📖 已借图书");
    QAction *showTopBorrowedAction = queryMenu_->addAction("🔥 热门图书");
    QAction *showRecentAction = queryMenu_->addAction("🆕 最新图书");
    queryMenu_->addSeparator();
    QAction *showExpensiveAction = queryMenu_->addAction("💰 高价图书");
    QAction *showCheapAction = queryMenu_->addAction("💸 低价图书");
    QAction *showDueAction = queryMenu_->addAction("⏰ 到期提醒");
    queryMenu_->addSeparator();
    QAction *advancedSearchAction = queryMenu_->addAction("🔍 高级搜索");
    
    // 连接信号
    connect(searchAction, &QAction::triggered, this, &MainWindow::onSearch);
    connect(filterCategoryAction, &QAction::triggered, this, &MainWindow::onFilterByCategory);
    connect(filterLocationAction, &QAction::triggered, this, &MainWindow::onFilterByLocation);
    connect(showAvailableAction, &QAction::triggered, this, &MainWindow::onShowAvailable);
    connect(showBorrowedAction, &QAction::triggered, this, &MainWindow::onShowBorrowed);
    connect(showTopBorrowedAction, &QAction::triggered, this, &MainWindow::onShowTopBorrowed);
    connect(showRecentAction, &QAction::triggered, this, &MainWindow::onShowRecentlyAdded);
    connect(showExpensiveAction, &QAction::triggered, this, &MainWindow::onShowExpensiveBooks);
    connect(showCheapAction, &QAction::triggered, this, &MainWindow::onShowCheapBooks);
    connect(showDueAction, &QAction::triggered, this, &MainWindow::onShowDue);
    connect(advancedSearchAction, &QAction::triggered, this, &MainWindow::onAdvancedSearch);
    
    // 3. 排序功能菜单
    sortMenu_ = menuBar_->addMenu("📊 排序功能");
    
    QAction *sortByNameAction = sortMenu_->addAction("🔤 按名称排序");
    QAction *sortByCategoryAction = sortMenu_->addAction("📚 按分类排序");
    QAction *sortByLocationAction = sortMenu_->addAction("📍 按位置排序");
    QAction *sortByPriceAction = sortMenu_->addAction("💵 按价格排序");
    QAction *sortByDateAction = sortMenu_->addAction("📅 按日期排序");
    QAction *sortByBorrowAction = sortMenu_->addAction("📈 按借阅排序");
    
    // 连接信号
    connect(sortByNameAction, &QAction::triggered, this, &MainWindow::onSortByName);
    connect(sortByCategoryAction, &QAction::triggered, this, &MainWindow::onSortByCategory);
    connect(sortByLocationAction, &QAction::triggered, this, &MainWindow::onSortByLocation);
    connect(sortByPriceAction, &QAction::triggered, this, &MainWindow::onSortByPrice);
    connect(sortByDateAction, &QAction::triggered, this, &MainWindow::onSortByDate);
    connect(sortByBorrowAction, &QAction::triggered, this, &MainWindow::onSortByBorrowCount);
    
    // 4. 数据管理菜单
    dataMenu_ = menuBar_->addMenu("💾 数据管理");
    
    QAction *openFileAction = dataMenu_->addAction("📂 打开文件");
    QAction *saveFileAction = dataMenu_->addAction("💾 保存文件");
    dataMenu_->addSeparator();
    QAction *exportDataAction = dataMenu_->addAction("📤 导出数据");
    QAction *importDataAction = dataMenu_->addAction("📥 导入数据");
    dataMenu_->addSeparator();
    QAction *backupDataAction = dataMenu_->addAction("💾 备份数据");
    QAction *restoreDataAction = dataMenu_->addAction("🔄 恢复数据");
    dataMenu_->addSeparator();
    QAction *statisticsAction = dataMenu_->addAction("📊 统计信息");
    
    // 连接信号
    connect(openFileAction, &QAction::triggered, this, &MainWindow::onOpen);
    connect(saveFileAction, &QAction::triggered, this, &MainWindow::onSave);
    connect(exportDataAction, &QAction::triggered, this, &MainWindow::onExportData);
    connect(importDataAction, &QAction::triggered, this, &MainWindow::onImportData);
    connect(backupDataAction, &QAction::triggered, this, &MainWindow::onBackupData);
    connect(restoreDataAction, &QAction::triggered, this, &MainWindow::onRestoreData);
    connect(statisticsAction, &QAction::triggered, this, &MainWindow::onShowStatistics);
    
    // 5. 系统设置菜单
    systemMenu_ = menuBar_->addMenu("⚙️ 系统设置");
    
    QAction *switchModeAction = systemMenu_->addAction("🔄 切换模式");
    QAction *toggleThemeAction = systemMenu_->addAction("🌙 切换主题");
    systemMenu_->addSeparator();
    QAction *aboutAction = systemMenu_->addAction("ℹ️ 关于系统");
    
    // 连接信号
    connect(switchModeAction, &QAction::triggered, this, &MainWindow::onSwitchMode);
    connect(toggleThemeAction, &QAction::triggered, this, &MainWindow::toggleTheme);
    connect(aboutAction, &QAction::triggered, this, [this]() {
        QMessageBox::about(this, "关于图书管理系统", 
                          "📚 图书管理系统 v2.0\n\n"
                          "功能特点：\n"
                          "• 完整的图书管理功能\n"
                          "• 智能查询和筛选\n"
                          "• 多种排序方式\n"
                          "• 数据导入导出\n"
                          "• 统计报表功能\n"
                          "• 深色/浅色主题\n\n"
                          "设计者：房睿，张邵奕，茅大鸿\n"
                          "南京邮电大学");
    });
}

// 新增实用功能实现
void MainWindow::onFilterByCategory()
{
    QStringList categories;
    QVector<Book> allBooks = library_.getAll();
    QSet<QString> uniqueCategories;
    
    for (const Book &book : allBooks) {
        uniqueCategories.insert(book.category);
    }
    
    for (const QString &category : uniqueCategories) {
        categories << category;
    }
    categories.sort();
    
    bool ok;
    QString category = QInputDialog::getItem(this, QStringLiteral("📂 按分类筛选"), 
                                            QStringLiteral("请选择要筛选的分类:"), 
                                            categories, 0, false, &ok);
    if (ok && !category.isEmpty()) {
        QVector<Book> filteredBooks = library_.getByCategory(category);
        refreshTable(filteredBooks);
        statusBar()->showMessage(QStringLiteral("📂 显示分类 '%1' 的图书，共 %2 本").arg(category).arg(filteredBooks.size()), 3000);
    }
}

void MainWindow::onFilterByLocation()
{
    QStringList locations;
    QVector<Book> allBooks = library_.getAll();
    QSet<QString> uniqueLocations;
    
    for (const Book &book : allBooks) {
        uniqueLocations.insert(book.location);
    }
    
    for (const QString &location : uniqueLocations) {
        locations << location;
    }
    locations.sort();
    
    bool ok;
    QString location = QInputDialog::getItem(this, QStringLiteral("📍 按位置筛选"), 
                                            QStringLiteral("请选择要筛选的位置:"), 
                                            locations, 0, false, &ok);
    if (ok && !location.isEmpty()) {
        QVector<Book> filteredBooks = library_.getByLocation(location);
        refreshTable(filteredBooks);
        statusBar()->showMessage(QStringLiteral("📍 显示位置 '%1' 的图书，共 %2 本").arg(location).arg(filteredBooks.size()), 3000);
    }
}

void MainWindow::onShowAvailable()
{
    QVector<Book> availableBooks = library_.getAvailable();
    refreshTable(availableBooks);
    statusBar()->showMessage(QStringLiteral("✅ 显示可借图书，共 %1 本").arg(availableBooks.size()), 3000);
}

void MainWindow::onShowBorrowed()
{
    QVector<Book> borrowedBooks = library_.getBorrowed();
    refreshTable(borrowedBooks);
    statusBar()->showMessage(QStringLiteral("📖 显示已借图书，共 %1 本").arg(borrowedBooks.size()), 3000);
}

void MainWindow::onShowTopBorrowed()
{
    QVector<Book> topBooks = library_.getTopBorrowed(10);
    refreshTable(topBooks);
    statusBar()->showMessage(QStringLiteral("🔥 显示热门图书前10名，共 %1 本").arg(topBooks.size()), 3000);
}

void MainWindow::onShowRecentlyAdded()
{
    QVector<Book> recentBooks = library_.getRecentlyAdded(30);
    refreshTable(recentBooks);
    statusBar()->showMessage(QStringLiteral("🆕 显示最近30天新增图书，共 %1 本").arg(recentBooks.size()), 3000);
}

void MainWindow::onShowExpensiveBooks()
{
    bool ok;
    double minPrice = QInputDialog::getDouble(this, QStringLiteral("💰 高价图书筛选"), 
                                            QStringLiteral("请输入最低价格:"), 50.0, 0.0, 10000.0, 2, &ok);
    if (ok) {
        QVector<Book> expensiveBooks = library_.getExpensiveBooks(minPrice);
        refreshTable(expensiveBooks);
        statusBar()->showMessage(QStringLiteral("💰 显示价格 ≥ %1 元的图书，共 %2 本").arg(minPrice).arg(expensiveBooks.size()), 3000);
    }
}

void MainWindow::onShowCheapBooks()
{
    bool ok;
    double maxPrice = QInputDialog::getDouble(this, QStringLiteral("💸 低价图书筛选"), 
                                            QStringLiteral("请输入最高价格:"), 30.0, 0.0, 10000.0, 2, &ok);
    if (ok) {
        QVector<Book> cheapBooks = library_.getCheapBooks(maxPrice);
        refreshTable(cheapBooks);
        statusBar()->showMessage(QStringLiteral("💸 显示价格 ≤ %1 元的图书，共 %2 本").arg(maxPrice).arg(cheapBooks.size()), 3000);
    }
}

void MainWindow::onShowStatistics()
{
    int totalBooks = library_.getTotalBooks();
    int availableBooks = library_.getAvailableBooks();
    int borrowedBooks = library_.getBorrowedBooks();
    double totalValue = library_.getTotalValue();
    QString mostPopularCategory = library_.getMostPopularCategory();
    QString mostPopularLocation = library_.getMostPopularLocation();
    
    QString stats = QStringLiteral(
        "📊 图书统计信息\n\n"
        "📚 总图书数量: %1 本\n"
        "✅ 可借图书: %2 本\n"
        "📖 已借图书: %3 本\n"
        "💰 图书总价值: %4 元\n"
        "🏆 最热门分类: %5\n"
        "📍 最热门位置: %6\n"
        "📈 借阅率: %7%"
    ).arg(totalBooks)
     .arg(availableBooks)
     .arg(borrowedBooks)
     .arg(QString::number(totalValue, 'f', 2))
     .arg(mostPopularCategory)
     .arg(mostPopularLocation)
     .arg(totalBooks > 0 ? QString::number((double)borrowedBooks / totalBooks * 100, 'f', 1) : "0");
    
    QMessageBox::information(this, QStringLiteral("📊 统计信息"), stats);
}

void MainWindow::onSortByName()
{
    library_.sortByName();
    refreshTable(library_.getAll());
    statusBar()->showMessage(QStringLiteral("🔤 已按名称排序"), 3000);
}

void MainWindow::onSortByCategory()
{
    library_.sortByCategory();
    refreshTable(library_.getAll());
    statusBar()->showMessage(QStringLiteral("📚 已按分类排序"), 3000);
}

void MainWindow::onSortByLocation()
{
    library_.sortByLocation();
    refreshTable(library_.getAll());
    statusBar()->showMessage(QStringLiteral("📍 已按位置排序"), 3000);
}

void MainWindow::onSortByPrice()
{
    library_.sortByPrice();
    refreshTable(library_.getAll());
    statusBar()->showMessage(QStringLiteral("💵 已按价格排序（高到低）"), 3000);
}

void MainWindow::onSortByDate()
{
    library_.sortByDate();
    refreshTable(library_.getAll());
    statusBar()->showMessage(QStringLiteral("📅 已按入库日期排序（新到旧）"), 3000);
}

void MainWindow::onSortByBorrowCount()
{
    library_.sortByBorrowCount();
    refreshTable(library_.getAll());
    statusBar()->showMessage(QStringLiteral("📈 已按借阅次数排序（高到低）"), 3000);
}

void MainWindow::onAdvancedSearch()
{
    bool ok;
    QString keyword = QInputDialog::getText(this, QStringLiteral("🔍 高级搜索"), 
                                           QStringLiteral("请输入搜索关键词（支持书名、分类、位置、索引号）:"), 
                                           QLineEdit::Normal, "", &ok);
    if (ok && !keyword.trimmed().isEmpty()) {
        QVector<Book> searchResults = library_.searchBooks(keyword.trimmed());
        refreshTable(searchResults);
        statusBar()->showMessage(QStringLiteral("🔍 搜索到 %1 本相关图书").arg(searchResults.size()), 3000);
    }
}

void MainWindow::onExportData()
{
    const QString path = QFileDialog::getSaveFileName(this, QStringLiteral("📤 导出数据"), 
                                                     QStringLiteral("library_export.json"), 
                                                     QStringLiteral("JSON 文件 (*.json);;所有文件 (*.*)"));
    if (!path.isEmpty()) {
        QString err;
        if (library_.saveToFile(path, &err)) {
            statusBar()->showMessage(QStringLiteral("✅ 数据导出成功: %1").arg(QFileInfo(path).fileName()), 3000);
        } else {
            QMessageBox::warning(this, QStringLiteral("❌ 导出失败"), err);
        }
    }
}

void MainWindow::onImportData()
{
    const QString path = QFileDialog::getOpenFileName(this, QStringLiteral("📥 导入数据"), QString(), 
                                                     QStringLiteral("JSON 文件 (*.json);;所有文件 (*.*)"));
    if (!path.isEmpty()) {
        QString err;
        if (library_.loadFromFile(path, &err)) {
            refreshTable(library_.getAll());
            statusBar()->showMessage(QStringLiteral("✅ 数据导入成功: %1").arg(QFileInfo(path).fileName()), 3000);
        } else {
            QMessageBox::warning(this, QStringLiteral("❌ 导入失败"), err);
        }
    }
}

void MainWindow::onBackupData()
{
    QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss");
    QString backupPath = QStringLiteral("backup_%1.json").arg(timestamp);
    
    QString err;
    if (library_.saveToFile(backupPath, &err)) {
        statusBar()->showMessage(QStringLiteral("💾 数据备份成功: %1").arg(backupPath), 3000);
    } else {
        QMessageBox::warning(this, QStringLiteral("❌ 备份失败"), err);
    }
}

void MainWindow::onRestoreData()
{
    const QString path = QFileDialog::getOpenFileName(this, QStringLiteral("🔄 恢复数据"), QString(), 
                                                     QStringLiteral("JSON 文件 (*.json);;所有文件 (*.*)"));
    if (!path.isEmpty()) {
        auto reply = QMessageBox::question(this, QStringLiteral("⚠️ 确认恢复"), 
                                          QStringLiteral("确定要恢复数据吗？这将覆盖当前所有数据！"),
                                          QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::Yes) {
            QString err;
            if (library_.loadFromFile(path, &err)) {
                refreshTable(library_.getAll());
                statusBar()->showMessage(QStringLiteral("✅ 数据恢复成功: %1").arg(QFileInfo(path).fileName()), 3000);
            } else {
                QMessageBox::warning(this, QStringLiteral("❌ 恢复失败"), err);
            }
        }
    }
}
