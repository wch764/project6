#pragma once
#include "User.h"
#include "BorrowInfo.h"
#include "Book.h"
#include "reader.h"
#include <vector>
#include <algorithm>
#include "systemlogger.h"

class NotificationManager {
private:
    std::vector<BorrowInfo>& borrowInfos;
    std::vector<Book>& books;
    std::vector<Reader*>& users;
    SystemLogger* logger;

    std::string findBookTitle(const std::string & isbn) const {
                auto it = std::find_if(books.begin(), books.end(),
                    [&isbn](const Book& b) { return b.getISBN() == isbn; });

                if (it != books.end()) {
                    return it->getTitle();
                }
                return "未知图书";
            }

            // 查找用户
    User* findUser(const std::string & username) const {
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
    NotificationManager(std::vector<BorrowInfo>&borrowInfos,
        std::vector<Book>&books,
        std::vector<Reader*>&users)
        : borrowInfos(borrowInfos), books(books), users(users) {
        logger = SystemLogger::getInstance();
    }

    // 生成即将到期通知
    void generateDueSoonNotifications() {
        Date currentDate = Date::currentdate();
        Date threeDaysLater = currentDate;
        threeDaysLater.addDays(3);

        logger->logInfo("NOTIFICATION", "检查即将到期的图书");

        int notificationCount = 0;
        for (const auto& info : borrowInfos) {
            // 跳过已归还、预约记录
            if (info.isReturned() || info.is_Reservation()) {
                continue;
            }

            Date dueDate = info.getDueDate();

            // 检查是否在3天内到期
            if ((dueDate > currentDate || dueDate == currentDate) && dueDate < threeDaysLater) {
                User* user = findUser(info.getReaderUsername());
                if (user != nullptr) {
                    std::string bookTitle = findBookTitle(info.getBookISBN());
                    Notification dueSoonNoti(Notification::DUE_SOON,
                        info.getBookISBN(),
                        bookTitle,
                        dueDate);
                    user->addNotification(dueSoonNoti);
                    notificationCount++;
                }
            }
        }

        logger->logInfo("NOTIFICATION", "生成了 " + std::to_string(notificationCount) + " 条到期提醒通知");
    }

    // 生成罚款通知，支持已缴纳罚款的处理
    void generateFineNotifications() {
        Date currentDate = Date::currentdate();
        logger->logInfo("NOTIFICATION", "检查有罚款的图书");

        int notificationCount = 0;
        for (auto& info : borrowInfos) {
            // 跳过已归还、预约记录
            if (info.isReturned() || info.is_Reservation()) {
                continue;
            }

            // 更新罚款金额
            info.updateFine(currentDate);
            double fineAmount = info.getFineAmount();

            if (fineAmount > 0) {
                User* user = findUser(info.getReaderUsername());
                if (user != nullptr) {
                    // 检查用户是否已经缴纳了罚款
                    Reader* reader = static_cast<Reader*>(user);
                    if (reader->getFineAmount() == 0) {
                        // 用户已缴纳所有罚款，清除罚款通知
                        clearFineNotifications(*user);
                        continue;
                    }

                    std::string bookTitle = findBookTitle(info.getBookISBN());
                    Notification fineNoti(Notification::OVERDUE_FINE,
                        info.getBookISBN(),
                        bookTitle,
                        fineAmount);
                    user->addNotification(fineNoti);
                    notificationCount++;
                }
            }
        }

        logger->logInfo("NOTIFICATION", "生成了 " + std::to_string(notificationCount) + " 条罚款通知");
    }

    // 清除用户的罚款通知
    void clearFineNotifications(User & user) {
        auto notifications = user.getAllNotifications();
        bool hasCleared = false;

        for (auto& noti : notifications) {
            if (noti.getType() == Notification::OVERDUE_FINE) {
                noti.markAsRead();
                hasCleared = true;
            }
        }

        if (hasCleared) {
            logger->logInfo("NOTIFICATION", "清除用户 " + user.getUsername() + " 的罚款通知");
        }
    }

    // 处理罚款缴纳后的通知清理
    void handleFinePayment(const std::string & username, double paidAmount) {
        User* user = findUser(username);
        if (user != nullptr) {
            clearFineNotifications(*user);
            logger->logInfo("FINE", "用户 " + username + " 缴纳罚款 " + std::to_string(paidAmount) + " 元，相关通知已清理");
        }
    }

    // 增强的通知清理功能
    void cleanupNotifications() {
        Date currentDate = Date::currentdate();
        int totalCleaned = 0;

        for (auto user : users) {
            if (user != nullptr) {
                User* userPtr = static_cast<User*>(user);

                // 获取清理前的通知数量
                int beforeCount = userPtr->getAllNotifications().size();

                // 执行清理
                userPtr->cleanupNotifications();

                // 计算清理的数量
                int afterCount = userPtr->getAllNotifications().size();
                int cleanedCount = beforeCount - afterCount;
                totalCleaned += cleanedCount;
            }
        }

        if (totalCleaned > 0) {
            logger->logInfo("NOTIFICATION", "自动清理了 " + std::to_string(totalCleaned) + " 条过期通知");
        }
    }

    // 生成所有通知的增强版本
    void generateAllNotifications() {
        logger->logInfo("NOTIFICATION", "开始生成系统通知");

        generateDueSoonNotifications();
        generateFineNotifications();

        // 清理过期通知
        cleanupNotifications();

        logger->logInfo("NOTIFICATION", "通知生成完成，系统中共有 " +
            std::to_string(getTotalUnreadNotifications()) + " 条未读通知");
    }

    // 获取系统中所有用户的未读通知总数
    int getTotalUnreadNotifications() const {
        int total = 0;
        for (auto user : users) {
            if (user != nullptr) {
                total += static_cast<User*>(user)->getUnreadNotificationCount();
            }
        }
        return total;
    }

    // 手动标记特定用户的特定类型通知为已读
    void markNotificationsAsRead(const std::string & username, Notification::Type type) {
        User* user = findUser(username);
        if (user != nullptr) {
            if (type == Notification::DUE_SOON) {
                user->markDueNotificationsAsRead();
            }
            else if (type == Notification::OVERDUE_FINE) {
                clearFineNotifications(*user);
            }
            logger->logInfo("NOTIFICATION", "手动标记用户 " + username + " 的通知为已读");
        }
    }

    // 检查并处理预约过期
    void handleExpiredReservations() {
        Date currentDate = Date::currentdate();
        int expiredCount = 0;

        for (auto& info : borrowInfos) {
            if (info.is_Reservation() && !info.isReturned()) {
                if (info.getExpectedAvailableDate() < currentDate) {
                    User* user = findUser(info.getReaderUsername());
                    if (user != nullptr) {
                        std::string bookTitle = findBookTitle(info.getBookISBN());
                        Notification expiredNoti(Notification::RESERVATION_EXPIRED,
                            info.getBookISBN(),
                            bookTitle,
                            currentDate);
                        user->addNotification(expiredNoti);
                        expiredCount++;
                    }
                }
            }
        }

        if (expiredCount > 0) {
            logger->logInfo("NOTIFICATION", "处理了 " + std::to_string(expiredCount) + " 个过期预约通知");
        }
    }

    // 发送预约可用通知
    void sendReservationAvailableNotification(const std::string & username, const std::string & isbn) {
        User* user = findUser(username);
        if (user != nullptr) {
            std::string bookTitle = findBookTitle(isbn);
            Date expiryDate = Date::currentdate();
            expiryDate.addDays(7); // 7天内确认借阅

            Notification noti(Notification::RESERVATION_AVAILABLE, isbn, bookTitle, expiryDate);
            user->addNotification(noti);

            logger->logNotificationSent(username, "预约可用");
        }
    }

    // 获取通知统计信息
    struct NotificationStats {
        int totalNotifications;
        int dueSoonCount;
        int fineCount;
        int reservationCount;
        int unreadCount;
    };

    NotificationStats getNotificationStats() const {
        NotificationStats stats = { 0, 0, 0, 0, 0 };

        for (auto user : users) {
            if (user != nullptr) {
                User* userPtr = static_cast<User*>(user);
                auto notifications = userPtr->getAllNotifications();

                stats.totalNotifications += notifications.size();
                stats.unreadCount += userPtr->getUnreadNotificationCount();

                for (const auto& noti : notifications) {
                    switch (noti.getType()) {
                    case Notification::DUE_SOON:
                        stats.dueSoonCount++;
                        break;
                    case Notification::OVERDUE_FINE:
                        stats.fineCount++;
                        break;
                    case Notification::RESERVATION_AVAILABLE:
                    case Notification::RESERVATION_EXPIRED:
                        stats.reservationCount++;
                        break;
                    }
                }
            }
        }

        return stats;
    }

    // 打印通知统计信息（调试用）
    void printNotificationStats() const {
        auto stats = getNotificationStats();
        std::cout << "=== 通知系统统计 ===" << std::endl;
        std::cout << "总通知数: " << stats.totalNotifications << std::endl;
        std::cout << "未读通知数: " << stats.unreadCount << std::endl;
        std::cout << "到期提醒: " << stats.dueSoonCount << std::endl;
        std::cout << "罚款通知: " << stats.fineCount << std::endl;
        std::cout << "预约通知: " << stats.reservationCount << std::endl;
    }
};