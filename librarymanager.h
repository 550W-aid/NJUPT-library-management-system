#ifndef LIBRARYMANAGER_H
#define LIBRARYMANAGER_H

#include <QObject>
#include <QVector>
#include <QString>
#include <QDate>

#include "book.h"

// 负责内存中的图书集合与文件持久化
class LibraryManager : public QObject {
    Q_OBJECT
public:
    explicit LibraryManager(QObject *parent = nullptr);

    // 文件 I/O
    bool loadFromFile(const QString &filePath, QString *errorMessage = nullptr);
    bool saveToFile(const QString &filePath, QString *errorMessage = nullptr) const;

    // 基本操作
    bool addBook(const Book &book, QString *errorMessage = nullptr);
    bool removeBookByIndexId(const QString &indexId);
    bool updateBook(const QString &indexId, const Book &updated, QString *errorMessage = nullptr);
    Book *findByName(const QString &name);
    const Book *findByName(const QString &name) const;

    // 业务逻辑
    bool borrowBook(const QString &indexId, QDate dueDate, QString *errorMessage = nullptr);
    bool returnBook(const QString &indexId, QString *errorMessage = nullptr);

    // 查询
    QVector<Book> getAll() const;
    QVector<Book> getDueInDays(int days) const;
    void sortByBorrowCountDesc();
    
    // 新增实用功能
    QVector<Book> getByCategory(const QString &category) const;
    QVector<Book> getByLocation(const QString &location) const;
    QVector<Book> getAvailable() const;
    QVector<Book> getBorrowed() const;
    QVector<Book> searchBooks(const QString &keyword) const;
    QVector<Book> getTopBorrowed(int limit = 10) const;
    QVector<Book> getRecentlyAdded(int days = 30) const;
    QVector<Book> getExpensiveBooks(double minPrice) const;
    QVector<Book> getCheapBooks(double maxPrice) const;
    
    // 统计功能
    int getTotalBooks() const;
    int getAvailableBooks() const;
    int getBorrowedBooks() const;
    int getBooksByCategory(const QString &category) const;
    double getTotalValue() const;
    QString getMostPopularCategory() const;
    QString getMostPopularLocation() const;
    
    // 排序功能
    void sortByName();
    void sortByCategory();
    void sortByLocation();
    void sortByPrice();
    void sortByDate();
    void sortByBorrowCount();

private:
    int findIndexById(const QString &indexId) const;

private:
    QVector<Book> books_;
};

#endif // LIBRARYMANAGER_H


