#pragma once
#include "User.h"
#include "BorrowInfo.h"
#include "Book.h"
#include "reader.h"
#include <vector>
#include <algorithm>

class NotificationManager {
private:
    std::vector<BorrowInfo>& borrowInfos;
    std::vector<Book>& books;
    std::vector<Reader*>& users;

    // 根据ISBN查找书名
    std::string findBookTitle(const std::string& isbn) const {
        auto it = std::find_if(books.begin(), books.end(),
            [&isbn](const Book& b) { return b.getISBN() == isbn; });

        if (it != books.end()) {
            return it->getTitle();
        }
        return "未知图书";
    }

    // 查找用户
    User* findUser(const std::string& username) const {
        auto it = std::find_if(users.begin(), users.end(),
            [&username](Reader* reader) {
                return reader != nullptr && reader->getUsername() == username;
            });

        if (it != users.end()) {
            return static_cast<User*>(*it);
        }
        return nullptr;
    }

public:
    NotificationManager(std::vector<BorrowInfo>& borrowInfos,
        std::vector<Book>& books,
        std::vector<Reader*>& users)
        : borrowInfos(borrowInfos), books(books), users(users) {
    }

    // 生成即将到期通知
    void generateDueSoonNotifications() {
        Date currentDate = Date::currentdate();
        Date threeDaysLater = currentDate;
        threeDaysLater.addDays(3);

        std::cout << "检查即将到期的图书..." << std::endl;

        for (const auto& info : borrowInfos) {
            // 跳过已归还、预约记录
            if (info.isReturned() || info.is_Reservation()) {
                continue;
            }

            Date dueDate = info.getDueDate();

            // 检查是否在3天内到期
            if (dueDate > currentDate-1 && dueDate < threeDaysLater) {
                std::cout << "发现即将到期图书: " << info.getBookISBN()
                    << " 用户: " << info.getReaderUsername()
                    << " 到期日: " << dueDate.toString() << std::endl;

                User* user = findUser(info.getReaderUsername());
                if (user != nullptr) {
                    std::string bookTitle = findBookTitle(info.getBookISBN());
                    Notification dueSoonNoti(Notification::DUE_SOON,
                        info.getBookISBN(),
                        bookTitle,
                        dueDate);
                    user->addNotification(dueSoonNoti);

                    std::cout << "已向用户 " << info.getReaderUsername()
                        << " 发送到期提醒" << std::endl;
                }
            }
        }
    }

    // 生成罚款通知
    void generateFineNotifications() {
        Date currentDate = Date::currentdate();

        std::cout << "检查有罚款的图书..." << std::endl;

        for (auto& info : borrowInfos) {
            // 跳过已归还、预约记录
            if (info.isReturned() || info.is_Reservation()) {
                continue;
            }

            // 更新罚款金额
            info.updateFine(currentDate);
            double fineAmount = info.getFineAmount();

            if (fineAmount > 0) {
                std::cout << "发现有罚款图书: " << info.getBookISBN()
                    << " 用户: " << info.getReaderUsername()
                    << " 罚款: " << fineAmount << " 元" << std::endl;

                User* user = findUser(info.getReaderUsername());
                if (user != nullptr) {
                    std::string bookTitle = findBookTitle(info.getBookISBN());
                    Notification fineNoti(Notification::OVERDUE_FINE,
                        info.getBookISBN(),
                        bookTitle,
                        fineAmount);
                    user->addNotification(fineNoti);

                    std::cout << "已向用户 " << info.getReaderUsername()
                        << " 发送罚款通知" << std::endl;
                }
            }
        }
    }

    // 生成所有通知
    void generateAllNotifications() {
        std::cout << "=== 开始生成通知 ===" << std::endl;

        generateDueSoonNotifications();
        generateFineNotifications();

        // 清理过期通知
        for (auto user : users) {
            if (user != nullptr) {
                static_cast<User*>(user)->cleanupNotifications();
            }
        }

        std::cout << "=== 通知生成完成 ===" << std::endl;
    }

    // 获取系统中所有用户的未读通知总数（调试用）
    int getTotalUnreadNotifications() const {
        int total = 0;
        for (auto user : users) {
            if (user != nullptr) {
                total += static_cast<User*>(user)->getUnreadNotificationCount();
            }
        }
        return total;
    }
};
