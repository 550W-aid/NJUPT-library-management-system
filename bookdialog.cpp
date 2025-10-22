#include "bookdialog.h"

#include <QIntValidator>
#include <QDoubleValidator>

BookDialog::BookDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(QStringLiteral("📚 图书信息"));
    setFixedSize(500, 600);
    setModal(true);

    indexIdEdit_ = new QLineEdit(this);
    nameEdit_ = new QLineEdit(this);
    locationEdit_ = new QComboBox(this);
    categoryEdit_ = new QComboBox(this);
    quantityEdit_ = new QLineEdit(this);
    priceEdit_ = new QLineEdit(this);
    inDateEdit_ = new QDateEdit(this);
    returnDateEdit_ = new QDateEdit(this);
    borrowCountEdit_ = new QLineEdit(this);
    availableCheck_ = new QCheckBox(QStringLiteral("可借"), this);

    quantityEdit_->setValidator(new QIntValidator(0, 1000000, this));
    borrowCountEdit_->setValidator(new QIntValidator(0, 1000000, this));
    auto *priceValidator = new QDoubleValidator(0.0, 1e9, 2, this);
    priceValidator->setNotation(QDoubleValidator::StandardNotation);
    priceEdit_->setValidator(priceValidator);

    inDateEdit_->setCalendarPopup(true);
    inDateEdit_->setDisplayFormat("yyyy-MM-dd");
    returnDateEdit_->setCalendarPopup(true);
    returnDateEdit_->setDisplayFormat("yyyy-MM-dd");
    
    // 设置馆藏地址下拉框选项
    locationEdit_->addItem(QStringLiteral("三牌楼"));
    locationEdit_->addItem(QStringLiteral("仙林"));
    locationEdit_->setCurrentIndex(0); // 默认选择三牌楼
    
    // 设置类别下拉框选项
    categoryEdit_->addItem(QStringLiteral("人文"));
    categoryEdit_->addItem(QStringLiteral("科技"));
    categoryEdit_->addItem(QStringLiteral("外语"));
    categoryEdit_->addItem(QStringLiteral("艺术"));
    categoryEdit_->addItem(QStringLiteral("历史"));
    categoryEdit_->addItem(QStringLiteral("哲学"));
    categoryEdit_->addItem(QStringLiteral("经济"));
    categoryEdit_->addItem(QStringLiteral("管理"));
    categoryEdit_->addItem(QStringLiteral("法律"));
    categoryEdit_->addItem(QStringLiteral("医学"));
    categoryEdit_->addItem(QStringLiteral("工程"));
    categoryEdit_->addItem(QStringLiteral("其他"));
    categoryEdit_->setCurrentIndex(0); // 默认选择人文
    categoryEdit_->setEditable(true); // 允许用户输入自定义类别

    // 美化输入框样式
    QString inputStyle = 
        "QLineEdit, QDateEdit, QComboBox {"
        "    padding: 10px;"
        "    border: 2px solid #e9ecef;"
        "    border-radius: 8px;"
        "    background-color: #ffffff;"
        "    font-size: 13px;"
        "    color: #495057;"
        "}"
        "QLineEdit:focus, QDateEdit:focus, QComboBox:focus {"
        "    border-color: #007bff;"
        "    background-color: #f8f9ff;"
        "}"
        "QComboBox::drop-down {"
        "    border: none;"
        "    width: 20px;"
        "}"
        "QComboBox::down-arrow {"
        "    image: url(data:image/svg+xml;base64,PHN2ZyB3aWR0aD0iMTIiIGhlaWdodD0iOCIgdmlld0JveD0iMCAwIDEyIDgiIGZpbGw9Im5vbmUiIHhtbG5zPSJodHRwOi8vd3d3LnczLm9yZy8yMDAwL3N2ZyI+CjxwYXRoIGQ9Ik0xIDFMNiA2TDExIDEiIHN0cm9rZT0iIzQ5NTA1NyIgc3Ryb2tlLXdpZHRoPSIyIiBzdHJva2UtbGluZWNhcD0icm91bmQiIHN0cm9rZS1saW5lam9pbj0icm91bmQiLz4KPC9zdmc+Cg==);"
        "    width: 12px;"
        "    height: 8px;"
        "}"
        "QComboBox QAbstractItemView {"
        "    border: 2px solid #e9ecef;"
        "    border-radius: 8px;"
        "    background-color: #ffffff;"
        "    selection-background-color: #007bff;"
        "    selection-color: white;"
        "    padding: 5px;"
        "}"
        "QCheckBox {"
        "    font-size: 13px;"
        "    color: #495057;"
        "    spacing: 8px;"
        "}"
        "QCheckBox::indicator {"
        "    width: 18px;"
        "    height: 18px;"
        "    border: 2px solid #e9ecef;"
        "    border-radius: 4px;"
        "    background-color: #ffffff;"
        "}"
        "QCheckBox::indicator:checked {"
        "    background-color: #007bff;"
        "    border-color: #007bff;"
        "    image: url(data:image/svg+xml;base64,PHN2ZyB3aWR0aD0iMTIiIGhlaWdodD0iOSIgdmlld0JveD0iMCAwIDEyIDkiIGZpbGw9Im5vbmUiIHhtbG5zPSJodHRwOi8vd3d3LnczLm9yZy8yMDAwL3N2ZyI+CjxwYXRoIGQ9Ik0xIDQuNUw0LjUgOEwxMSAxIiBzdHJva2U9IndoaXRlIiBzdHJva2Utd2lkdGg9IjIiIHN0cm9rZS1saW5lY2FwPSJyb3VuZCIgc3Ryb2tlLWxpbmVqb2luPSJyb3VuZCIvPgo8L3N2Zz4K);"
        "}";

    indexIdEdit_->setStyleSheet(inputStyle);
    nameEdit_->setStyleSheet(inputStyle);
    locationEdit_->setStyleSheet(inputStyle);
    categoryEdit_->setStyleSheet(inputStyle);
    quantityEdit_->setStyleSheet(inputStyle);
    priceEdit_->setStyleSheet(inputStyle);
    inDateEdit_->setStyleSheet(inputStyle);
    returnDateEdit_->setStyleSheet(inputStyle);
    borrowCountEdit_->setStyleSheet(inputStyle);
    availableCheck_->setStyleSheet(inputStyle);

    auto *form = new QFormLayout(this);
    form->setSpacing(15);
    form->setContentsMargins(30, 30, 30, 30);
    
    // 添加图标美化标签
    form->addRow(QStringLiteral("🔢 索引号"), indexIdEdit_);
    form->addRow(QStringLiteral("📖 名称"), nameEdit_);
    form->addRow(QStringLiteral("📍 馆藏地址"), locationEdit_);
    form->addRow(QStringLiteral("📂 类别"), categoryEdit_);
    form->addRow(QStringLiteral("🔢 数量"), quantityEdit_);
    form->addRow(QStringLiteral("💰 价格"), priceEdit_);
    form->addRow(QStringLiteral("📅 入库日期"), inDateEdit_);
    form->addRow(QStringLiteral("📅 归还日期"), returnDateEdit_);
    form->addRow(QStringLiteral("📊 借阅次数"), borrowCountEdit_);
    form->addRow(QStringLiteral("✅ 状态"), availableCheck_);

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    buttons->setStyleSheet(
        "QPushButton {"
        "    background-color: #007bff;"
        "    color: white;"
        "    border: none;"
        "    border-radius: 6px;"
        "    padding: 10px 20px;"
        "    font-size: 13px;"
        "    font-weight: bold;"
        "    min-width: 80px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #0056b3;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #004085;"
        "}"
        "QPushButton[text=\"Cancel\"] {"
        "    background-color: #6c757d;"
        "}"
        "QPushButton[text=\"Cancel\"]:hover {"
        "    background-color: #545b62;"
        "}"
    );
    
    form->addRow(buttons);
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    
    // 设置对话框样式
    setStyleSheet(
        "QDialog {"
        "    background-color: #f8f9fa;"
        "    border: 1px solid #dee2e6;"
        "    border-radius: 12px;"
        "}"
        "QLabel {"
        "    font-size: 13px;"
        "    font-weight: bold;"
        "    color: #495057;"
        "    margin-bottom: 5px;"
        "}"
    );
}

void BookDialog::setBook(const Book &b)
{
    indexIdEdit_->setText(b.indexId);
    nameEdit_->setText(b.name);
    
    // 设置馆藏地址下拉框
    int locationIndex = locationEdit_->findText(b.location);
    if (locationIndex >= 0) {
        locationEdit_->setCurrentIndex(locationIndex);
    } else {
        // 如果找不到匹配项，默认选择三牌楼
        locationEdit_->setCurrentIndex(0);
    }
    
    // 设置类别下拉框
    int categoryIndex = categoryEdit_->findText(b.category);
    if (categoryIndex >= 0) {
        categoryEdit_->setCurrentIndex(categoryIndex);
    } else {
        // 如果找不到匹配项，设置为当前文本
        categoryEdit_->setCurrentText(b.category);
    }
    quantityEdit_->setText(QString::number(b.quantity));
    priceEdit_->setText(QString::number(b.price, 'f', 2));
    inDateEdit_->setDate(b.inDate.isValid() ? b.inDate : QDate::currentDate());
    returnDateEdit_->setDate(b.returnDate.isValid() ? b.returnDate : QDate::currentDate());
    returnDateEdit_->setSpecialValueText(QString());
    borrowCountEdit_->setText(QString::number(b.borrowCount));
    availableCheck_->setChecked(b.available);
}

Book BookDialog::getBook() const
{
    Book b;
    b.indexId = indexIdEdit_->text().trimmed();
    b.name = nameEdit_->text().trimmed();
    b.location = locationEdit_->currentText(); // 获取下拉框当前选中的文本
    b.category = categoryEdit_->currentText().trimmed();
    b.quantity = quantityEdit_->text().toInt();
    b.price = priceEdit_->text().toDouble();
    b.inDate = inDateEdit_->date();
    b.returnDate = returnDateEdit_->date();
    b.borrowCount = borrowCountEdit_->text().toInt();
    b.available = availableCheck_->isChecked();
    return b;
}


