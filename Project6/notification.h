#pragma once
#include "Date.h"
#include <string>

class Notification {
public:
    enum Type {
        RESERVATION_AVAILABLE,
        RESERVATION_EXPIRED,
        DUE_SOON,           // ͼ�鼴������
        OVERDUE_FINE        // ��δ������
    };

private:
    Type type;
    std::string bookIsbn;
    std::string bookTitle;
    Date expiryDate;
    double fineAmount;      // ������
    bool isRead;            // �Ƿ��Ѷ�
    Date creationDate;      // ֪ͨ��������

public:
    // ���캯�� - ԤԼ���֪ͨ
    Notification(Type type, const std::string& isbn, const std::string& title, const Date& expiry)
        : type(type), bookIsbn(isbn), bookTitle(title), expiryDate(expiry),
        fineAmount(0.0), isRead(false), creationDate(Date::currentdate()) {
    }

    // ���캯�� - ����֪ͨ
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

    // ���Ϊ�Ѷ�
    void markAsRead() {
        
            isRead = true;
        
    }

    // ����Ƿ�Ӧ����ʾ��δ���򷣿�֪ͨ��
    bool shouldDisplay() const {
        return !isRead;
    }

    std::string getMessage() const {
        switch (type) {
        case RESERVATION_AVAILABLE:
            return "ԤԼͼ�顶" + bookTitle + "���ѿ��ã�����" + expiryDate.toString() + "ǰȷ�Ͻ���";
        case RESERVATION_EXPIRED:
            return "ԤԼͼ�顶" + bookTitle + "���ѹ���";
        case DUE_SOON:
            return "ͼ�顶" + bookTitle + "������" + expiryDate.toString() + "���ڣ��뼰ʱ�黹";
        case OVERDUE_FINE:
            return "ͼ�顶" + bookTitle + "����δ������ " + std::to_string(fineAmount) + " Ԫ���뼰ʱ����";
        default:
            return "δ֪֪ͨ";
        }
    }
};