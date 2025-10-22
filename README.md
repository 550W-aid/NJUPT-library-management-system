# 图书管理系统 - 程序结构分析

## 📖 项目简介

这是一个基于Qt框架开发的图书管理系统，采用MVC架构模式，提供完整的图书管理功能，包括图书增删改查、借阅归还、数据统计、主题切换等功能。

## 🏗️ 程序架构

### 整体架构图
```
📱 用户界面层 (View)
├── MainWindow - 主窗口界面
├── BookDialog - 图书编辑对话框  
├── LoginDialog - 登录对话框
└── SplashScreen - 启动画面

🎮 控制逻辑层 (Controller)  
└── MainWindow - 处理用户交互和业务逻辑

💾 数据模型层 (Model)
├── Book - 图书数据结构
└── LibraryManager - 图书数据管理
```

## 📁 文件结构

```
├── main.cpp                 # 程序入口
├── mainwindow.h/cpp         # 主窗口类
├── librarymanager.h/cpp     # 图书管理核心类
├── book.h                   # 图书数据结构
├── bookdialog.h/cpp        # 图书编辑对话框
├── logindialog.h/cpp       # 登录对话框
├── splashscreen.h/cpp      # 启动画面
├── mainwindow.ui           # 主窗口UI设计文件
├── resources.qrc           # 资源文件
└── icons/                  # 图标资源
```

## 🔧 核心功能模块

### 1. 数据管理模块 (LibraryManager)

**文件操作**
- `loadFromFile()` - 从JSON文件加载图书数据
- `saveToFile()` - 保存图书数据到JSON文件

**基本CRUD操作**
- `addBook()` - 添加新图书
- `removeBookByIndexId()` - 根据索引号删除图书
- `updateBook()` - 更新图书信息
- `findByName()` - 根据名称查找图书

**业务逻辑**
- `borrowBook()` - 借阅图书
- `returnBook()` - 归还图书

**查询筛选**
- `getByCategory()` - 按分类筛选
- `getByLocation()` - 按位置筛选
- `getAvailable()` - 获取可借图书
- `getBorrowed()` - 获取已借图书
- `searchBooks()` - 关键词搜索

**排序功能**
- `sortByName()` - 按名称排序
- `sortByCategory()` - 按分类排序
- `sortByPrice()` - 按价格排序
- `sortByBorrowCount()` - 按借阅次数排序

**统计功能**
- `getTotalBooks()` - 获取图书总数
- `getTotalValue()` - 计算图书总价值
- `getMostPopularCategory()` - 获取最热门分类

### 2. 界面控制模块 (MainWindow)

**界面初始化**
- `setupTable()` - 设置数据表格
- `setupActions()` - 设置工具栏按钮
- `setupMenuBar()` - 设置菜单栏
- `setupSearchBar()` - 设置搜索栏
- `setupThemeToggle()` - 设置主题切换

**用户交互处理**
- `onAdd()` - 处理添加图书
- `onEdit()` - 处理编辑图书
- `onRemove()` - 处理删除图书
- `onBorrow()` - 处理借阅操作
- `onReturn()` - 处理归还操作
- `onSearch()` - 处理搜索操作

**权限管理**
- `setUserMode()` - 设置用户模式（管理员/读者）
- `updateUIForUserMode()` - 根据用户模式更新界面权限

**主题系统**
- `toggleTheme()` - 切换深浅色主题
- `applyTheme()` - 应用主题样式
- `getThemeStyles()` - 获取主题样式字符串

### 3. 对话框模块

**BookDialog（图书编辑）**
- `setBook()` - 设置要编辑的图书数据
- `getBook()` - 获取编辑后的图书数据

**LoginDialog（登录验证）**
- `onLogin()` - 处理登录验证
- `validateCredentials()` - 验证用户凭据
- `validateAdminPassword()` - 验证管理员密码

**SplashScreen（启动画面）**
- `showSplash()` - 显示启动画面
- `fadeIn()` - 淡入动画
- `fadeOut()` - 淡出动画

## 🔄 程序执行流程

### 启动流程
```
main() 
├── 创建SplashScreen → showSplash() → fadeIn()
├── 创建MainWindow → 初始化界面组件
├── 显示LoginDialog → 用户登录验证
└── 显示MainWindow → 进入主程序
```

### 图书操作流程
```
用户点击按钮 → MainWindow::onXxx() 
├── 权限检查（管理员模式）
├── 调用LibraryManager::xxx() 
├── 更新数据
└── refreshTable() → 刷新界面显示
```

### 数据持久化流程
```
用户操作 → LibraryManager修改内存数据 
→ saveToFile() → JSON序列化 → 文件保存
```

## 🎯 设计特点

### 1. 分层架构
- **界面层**：负责用户交互和界面展示
- **逻辑层**：处理业务逻辑和用户操作
- **数据层**：管理图书数据和文件操作

### 2. 权限控制
- **管理员模式**：拥有所有功能权限
- **读者模式**：只能查看和借阅，无法编辑删除

### 3. 主题系统
- 支持深浅色主题切换
- iOS风格界面设计
- 响应式布局适配

### 4. 数据管理
- JSON格式数据存储
- 支持数据导入导出
- 自动备份功能

### 5. 功能丰富
- 多种搜索筛选方式
- 多种排序选项
- 统计报表功能
- 到期提醒功能

## 🚀 技术栈

- **框架**：Qt 6.9.3
- **语言**：C++
- **UI设计**：Qt Designer
- **数据格式**：JSON
- **编译器**：MinGW 64-bit

## 👥 开发团队

- **设计者**：房睿，张邵奕，茅大鸿
- **学校**：南京邮电大学

## 📝 使用说明

1. **启动程序**：运行 `untitled.exe`
2. **登录系统**：输入用户名密码，选择管理员模式需要密码"1234"
3. **管理图书**：管理员可以增删改图书，读者只能查看和借阅
4. **主题切换**：点击🌙/☀️按钮切换深浅色主题
5. **数据操作**：支持导入导出JSON格式数据

---

*这是一个功能完整、界面美观的图书管理系统，适合学习Qt开发和桌面应用程序架构设计。*
