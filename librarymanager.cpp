#include "librarymanager.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QMap>
#include <algorithm>

LibraryManager::LibraryManager(QObject *parent)
    : QObject(parent)
{
}

bool LibraryManager::loadFromFile(const QString &filePath, QString *errorMessage)
{
    QFile f(filePath);
    if (!f.open(QIODevice::ReadOnly)) {
        if (errorMessage) *errorMessage = QString::fromLatin1("无法打开文件: ") + filePath;
        return false;
    }
    const QByteArray data = f.readAll();
    f.close();
    QJsonParseError pe;
    const QJsonDocument doc = QJsonDocument::fromJson(data, &pe);
    if (pe.error != QJsonParseError::NoError || !doc.isArray()) {
        if (errorMessage) *errorMessage = QString::fromLatin1("JSON 解析失败");
        return false;
    }
    books_.clear();
    const QJsonArray arr = doc.array();
    books_.reserve(arr.size());
    for (const QJsonValue &v : arr) {
        if (!v.isObject()) continue;
        books_.append(fromJson(v.toObject()));
    }
    return true;
}

bool LibraryManager::saveToFile(const QString &filePath, QString *errorMessage) const
{
    QJsonArray arr;
    for (const Book &b : books_) {
        QJsonObject obj;
        toJson(obj, b);
        arr.append(obj);
    }
    const QJsonDocument doc(arr);
    QFile f(filePath);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        if (errorMessage) *errorMessage = QString::fromLatin1("无法写入文件: ") + filePath;
        return false;
    }
    f.write(doc.toJson(QJsonDocument::Indented));
    f.close();
    return true;
}

bool LibraryManager::addBook(const Book &book, QString *errorMessage)
{
    if (book.indexId.trimmed().isEmpty()) {
        if (errorMessage) *errorMessage = QString::fromLatin1("索引号不能为空");
        return false;
    }
    if (findIndexById(book.indexId) != -1) {
        if (errorMessage) *errorMessage = QString::fromLatin1("索引号已存在");
        return false;
    }
    books_.append(book);
    return true;
}

bool LibraryManager::removeBookByIndexId(const QString &indexId)
{
    const int pos = findIndexById(indexId);
    if (pos < 0) return false;
    books_.removeAt(pos);
    return true;
}

bool LibraryManager::updateBook(const QString &indexId, const Book &updated, QString *errorMessage)
{
    const int pos = findIndexById(indexId);
    if (pos < 0) {
        if (errorMessage) *errorMessage = QString::fromLatin1("未找到该索引号");
        return false;
    }
    if (updated.indexId != indexId && findIndexById(updated.indexId) != -1) {
        if (errorMessage) *errorMessage = QString::fromLatin1("新索引号已存在");
        return false;
    }
    books_[pos] = updated;
    return true;
}

Book *LibraryManager::findByName(const QString &name)
{
    for (Book &b : books_) {
        if (b.name.compare(name, Qt::CaseInsensitive) == 0) return &b;
    }
    return nullptr;
}

const Book *LibraryManager::findByName(const QString &name) const
{
    for (const Book &b : books_) {
        if (b.name.compare(name, Qt::CaseInsensitive) == 0) return &b;
    }
    return nullptr;
}

bool LibraryManager::borrowBook(const QString &indexId, QDate dueDate, QString *errorMessage)
{
    const int pos = findIndexById(indexId);
    if (pos < 0) {
        if (errorMessage) *errorMessage = QString::fromLatin1("未找到该图书");
        return false;
    }
    Book &b = books_[pos];
    if (!b.available || b.quantity <= 0) {
        if (errorMessage) *errorMessage = QString::fromLatin1("不可借或库存不足");
        return false;
    }
    b.quantity -= 1;
    b.borrowCount += 1;
    b.available = b.quantity > 0;
    b.returnDate = dueDate;
    return true;
}

bool LibraryManager::returnBook(const QString &indexId, QString *errorMessage)
{
    const int pos = findIndexById(indexId);
    if (pos < 0) {
        if (errorMessage) *errorMessage = QString::fromLatin1("未找到该图书");
        return false;
    }
    Book &b = books_[pos];
    b.quantity += 1;
    b.available = b.quantity > 0;
    b.returnDate = QDate();
    return true;
}

QVector<Book> LibraryManager::getAll() const
{
    return books_;
}

QVector<Book> LibraryManager::getDueInDays(int days) const
{
    QVector<Book> res;
    const QDate today = QDate::currentDate();
    for (const Book &b : books_) {
        if (b.returnDate.isValid()) {
            const int diff = today.daysTo(b.returnDate);
            if (diff >= 0 && diff <= days) res.append(b);
        }
    }
    return res;
}

void LibraryManager::sortByBorrowCountDesc()
{
    std::sort(books_.begin(), books_.end(), [](const Book &a, const Book &b){
        return a.borrowCount > b.borrowCount;
    });
}

int LibraryManager::findIndexById(const QString &indexId) const
{
    for (int i = 0; i < books_.size(); ++i) {
        if (books_[i].indexId == indexId) return i;
    }
    return -1;
}

// 新增实用功能实现
QVector<Book> LibraryManager::getByCategory(const QString &category) const
{
    QVector<Book> result;
    for (const Book &book : books_) {
        if (book.category.contains(category, Qt::CaseInsensitive)) {
            result.append(book);
        }
    }
    return result;
}

QVector<Book> LibraryManager::getByLocation(const QString &location) const
{
    QVector<Book> result;
    for (const Book &book : books_) {
        if (book.location.contains(location, Qt::CaseInsensitive)) {
            result.append(book);
        }
    }
    return result;
}

QVector<Book> LibraryManager::getAvailable() const
{
    QVector<Book> result;
    for (const Book &book : books_) {
        if (book.available && book.quantity > 0) {
            result.append(book);
        }
    }
    return result;
}

QVector<Book> LibraryManager::getBorrowed() const
{
    QVector<Book> result;
    for (const Book &book : books_) {
        if (!book.available || book.quantity == 0) {
            result.append(book);
        }
    }
    return result;
}

QVector<Book> LibraryManager::searchBooks(const QString &keyword) const
{
    QVector<Book> result;
    QString lowerKeyword = keyword.toLower();
    for (const Book &book : books_) {
        if (book.name.toLower().contains(lowerKeyword) ||
            book.category.toLower().contains(lowerKeyword) ||
            book.location.toLower().contains(lowerKeyword) ||
            book.indexId.toLower().contains(lowerKeyword)) {
            result.append(book);
        }
    }
    return result;
}

QVector<Book> LibraryManager::getTopBorrowed(int limit) const
{
    QVector<Book> result = books_;
    std::sort(result.begin(), result.end(), [](const Book &a, const Book &b){
        return a.borrowCount > b.borrowCount;
    });
    if (limit > 0 && limit < result.size()) {
        result.resize(limit);
    }
    return result;
}

QVector<Book> LibraryManager::getRecentlyAdded(int days) const
{
    QVector<Book> result;
    QDate cutoffDate = QDate::currentDate().addDays(-days);
    for (const Book &book : books_) {
        if (book.inDate >= cutoffDate) {
            result.append(book);
        }
    }
    return result;
}

QVector<Book> LibraryManager::getExpensiveBooks(double minPrice) const
{
    QVector<Book> result;
    for (const Book &book : books_) {
        if (book.price >= minPrice) {
            result.append(book);
        }
    }
    return result;
}

QVector<Book> LibraryManager::getCheapBooks(double maxPrice) const
{
    QVector<Book> result;
    for (const Book &book : books_) {
        if (book.price <= maxPrice) {
            result.append(book);
        }
    }
    return result;
}

// 统计功能实现
int LibraryManager::getTotalBooks() const
{
    return books_.size();
}

int LibraryManager::getAvailableBooks() const
{
    int count = 0;
    for (const Book &book : books_) {
        if (book.available && book.quantity > 0) {
            count++;
        }
    }
    return count;
}

int LibraryManager::getBorrowedBooks() const
{
    int count = 0;
    for (const Book &book : books_) {
        if (!book.available || book.quantity == 0) {
            count++;
        }
    }
    return count;
}

int LibraryManager::getBooksByCategory(const QString &category) const
{
    int count = 0;
    for (const Book &book : books_) {
        if (book.category.contains(category, Qt::CaseInsensitive)) {
            count++;
        }
    }
    return count;
}

double LibraryManager::getTotalValue() const
{
    double total = 0.0;
    for (const Book &book : books_) {
        total += book.price * book.quantity;
    }
    return total;
}

QString LibraryManager::getMostPopularCategory() const
{
    QMap<QString, int> categoryCount;
    for (const Book &book : books_) {
        categoryCount[book.category]++;
    }
    
    QString mostPopular;
    int maxCount = 0;
    for (auto it = categoryCount.begin(); it != categoryCount.end(); ++it) {
        if (it.value() > maxCount) {
            maxCount = it.value();
            mostPopular = it.key();
        }
    }
    return mostPopular;
}

QString LibraryManager::getMostPopularLocation() const
{
    QMap<QString, int> locationCount;
    for (const Book &book : books_) {
        locationCount[book.location]++;
    }
    
    QString mostPopular;
    int maxCount = 0;
    for (auto it = locationCount.begin(); it != locationCount.end(); ++it) {
        if (it.value() > maxCount) {
            maxCount = it.value();
            mostPopular = it.key();
        }
    }
    return mostPopular;
}

// 排序功能实现
void LibraryManager::sortByName()
{
    std::sort(books_.begin(), books_.end(), [](const Book &a, const Book &b){
        return a.name < b.name;
    });
}

void LibraryManager::sortByCategory()
{
    std::sort(books_.begin(), books_.end(), [](const Book &a, const Book &b){
        return a.category < b.category;
    });
}

void LibraryManager::sortByLocation()
{
    std::sort(books_.begin(), books_.end(), [](const Book &a, const Book &b){
        return a.location < b.location;
    });
}

void LibraryManager::sortByPrice()
{
    std::sort(books_.begin(), books_.end(), [](const Book &a, const Book &b){
        return a.price > b.price;
    });
}

void LibraryManager::sortByDate()
{
    std::sort(books_.begin(), books_.end(), [](const Book &a, const Book &b){
        return a.inDate > b.inDate;
    });
}

void LibraryManager::sortByBorrowCount()
{
    std::sort(books_.begin(), books_.end(), [](const Book &a, const Book &b){
        return a.borrowCount > b.borrowCount;
    });
}


