#ifndef BOOKDIALOG_H
#define BOOKDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QDateEdit>
#include <QCheckBox>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QComboBox>
#include "book.h"

class BookDialog : public QDialog {
    Q_OBJECT
public:
    explicit BookDialog(QWidget *parent = nullptr);

    void setBook(const Book &book);
    Book getBook() const;

private:
    QLineEdit *indexIdEdit_ = nullptr;
    QLineEdit *nameEdit_ = nullptr;
    QComboBox *locationEdit_ = nullptr;
    QComboBox *categoryEdit_ = nullptr;
    QLineEdit *quantityEdit_ = nullptr;
    QLineEdit *priceEdit_ = nullptr;
    QDateEdit *inDateEdit_ = nullptr;
    QDateEdit *returnDateEdit_ = nullptr;
    QLineEdit *borrowCountEdit_ = nullptr;
    QCheckBox *availableCheck_ = nullptr;
};

#endif // BOOKDIALOG_H


