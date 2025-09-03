#pragma once
#include "Date.h"
#include <string>

class Notification {
public:
    enum Type {
        RESERVATION_AVAILABLE,
        RESERVATION_EXPIRED,
        DUE_SOON,           // 图书即将到期
        OVERDUE_FINE        // 有未交罚款
    };

private:
    Type type;
    std::string bookIsbn;
    std::string bookTitle;
    Date expiryDate;
    double fineAmount;      // 罚款金额
    bool isRead;            // 是否已读
    Date creationDate;      // 通知创建日期

public:
    // 构造函数 - 预约相关通知
    Notification(Type type, const std::string& isbn, const std::string& title, const Date& expiry)
        : type(type), bookIsbn(isbn), bookTitle(title), expiryDate(expiry),
        fineAmount(0.0), isRead(false), creationDate(Date::currentdate()) {
    }

    // 构造函数 - 罚款通知
    Notification(Type type, const std::string& isbn, const std::string& title, double fine)
        : type(type), bookIsbn(isbn), bookTitle(title), expiryDate(Date::currentdate()),
        fineAmount(fine), isRead(false), creationDate(Date::currentdate()) {
    }

    // Getters
    Type getType() const { return type; }
    std::string getBookIsbn() const { return bookIsbn; }
    std::string getBookTitle() const { return bookTitle; }
    Date getExpiryDate() const { return expiryDate; }
    double getFineAmount() const { return fineAmount; }
    bool getIsRead() const { return isRead; }
    Date getCreationDate() const { return creationDate; }

    // 标记为已读
    void markAsRead() {
        
            isRead = true;
        
    }

    // 检查是否应该显示（未读或罚款通知）
    bool shouldDisplay() const {
        return !isRead;
    }

    std::string getMessage() const {
        switch (type) {
        case RESERVATION_AVAILABLE:
            return "预约图书《" + bookTitle + "》已可用，请于" + expiryDate.toString() + "前确认借阅";
        case RESERVATION_EXPIRED:
            return "预约图书《" + bookTitle + "》已过期";
        case DUE_SOON:
            return "图书《" + bookTitle + "》将于" + expiryDate.toString() + "到期，请及时归还";
        case OVERDUE_FINE:
            return "图书《" + bookTitle + "》有未交罚款 " + std::to_string(fineAmount) + " 元，请及时缴纳";
        default:
            return "未知通知";
        }
    }
};