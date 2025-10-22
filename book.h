#ifndef BOOK_H
#define BOOK_H

#include <QString>
#include <QDate>
#include <QJsonObject>

// 图书信息数据结构
struct Book {
    QString indexId;          // 索引号
    QString name;             // 名称
    QString location;         // 馆藏地址：三牌楼/仙林
    QString category;         // 类别：人文/科技/外语等
    int quantity = 0;         // 数量
    double price = 0.0;       // 价格
    QDate inDate;             // 入库日期
    QDate returnDate;         // 归还日期（若未借出可为空）
    int borrowCount = 0;      // 借阅次数
    bool available = true;    // 借阅状态：可借/不可借
};

inline void toJson(QJsonObject &obj, const Book &b)
{
    obj["indexId"] = b.indexId;
    obj["name"] = b.name;
    obj["location"] = b.location;
    obj["category"] = b.category;
    obj["quantity"] = b.quantity;
    obj["price"] = b.price;
    obj["inDate"] = b.inDate.toString(Qt::ISODate);
    obj["returnDate"] = b.returnDate.isValid() ? b.returnDate.toString(Qt::ISODate) : QString();
    obj["borrowCount"] = b.borrowCount;
    obj["available"] = b.available;
}

inline Book fromJson(const QJsonObject &obj)
{
    Book b;
    b.indexId = obj.value("indexId").toString();
    b.name = obj.value("name").toString();
    b.location = obj.value("location").toString();
    b.category = obj.value("category").toString();
    b.quantity = obj.value("quantity").toInt();
    b.price = obj.value("price").toDouble();
    b.inDate = QDate::fromString(obj.value("inDate").toString(), Qt::ISODate);
    const QString ret = obj.value("returnDate").toString();
    b.returnDate = ret.isEmpty() ? QDate() : QDate::fromString(ret, Qt::ISODate);
    b.borrowCount = obj.value("borrowCount").toInt();
    b.available = obj.value("available").toBool(true);
    return b;
}

#endif // BOOK_H


