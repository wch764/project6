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
                return "δ֪ͼ��";
            }

            // �����û�
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

    // ���ɼ�������֪ͨ
    void generateDueSoonNotifications() {
        Date currentDate = Date::currentdate();
        Date threeDaysLater = currentDate;
        threeDaysLater.addDays(3);

        logger->logInfo("NOTIFICATION", "��鼴�����ڵ�ͼ��");

        int notificationCount = 0;
        for (const auto& info : borrowInfos) {
            // �����ѹ黹��ԤԼ��¼
            if (info.isReturned() || info.is_Reservation()) {
                continue;
            }

            Date dueDate = info.getDueDate();

            // ����Ƿ���3���ڵ���
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

        logger->logInfo("NOTIFICATION", "������ " + std::to_string(notificationCount) + " ����������֪ͨ");
    }

    // ���ɷ���֪ͨ��֧���ѽ��ɷ���Ĵ���
    void generateFineNotifications() {
        Date currentDate = Date::currentdate();
        logger->logInfo("NOTIFICATION", "����з����ͼ��");

        int notificationCount = 0;
        for (auto& info : borrowInfos) {
            // �����ѹ黹��ԤԼ��¼
            if (info.isReturned() || info.is_Reservation()) {
                continue;
            }

            // ���·�����
            info.updateFine(currentDate);
            double fineAmount = info.getFineAmount();

            if (fineAmount > 0) {
                User* user = findUser(info.getReaderUsername());
                if (user != nullptr) {
                    // ����û��Ƿ��Ѿ������˷���
                    Reader* reader = static_cast<Reader*>(user);
                    if (reader->getFineAmount() == 0) {
                        // �û��ѽ������з���������֪ͨ
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

        logger->logInfo("NOTIFICATION", "������ " + std::to_string(notificationCount) + " ������֪ͨ");
    }

    // ����û��ķ���֪ͨ
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
            logger->logInfo("NOTIFICATION", "����û� " + user.getUsername() + " �ķ���֪ͨ");
        }
    }

    // ��������ɺ��֪ͨ����
    void handleFinePayment(const std::string & username, double paidAmount) {
        User* user = findUser(username);
        if (user != nullptr) {
            clearFineNotifications(*user);
            logger->logInfo("FINE", "�û� " + username + " ���ɷ��� " + std::to_string(paidAmount) + " Ԫ�����֪ͨ������");
        }
    }

    // ��ǿ��֪ͨ������
    void cleanupNotifications() {
        Date currentDate = Date::currentdate();
        int totalCleaned = 0;

        for (auto user : users) {
            if (user != nullptr) {
                User* userPtr = static_cast<User*>(user);

                // ��ȡ����ǰ��֪ͨ����
                int beforeCount = userPtr->getAllNotifications().size();

                // ִ������
                userPtr->cleanupNotifications();

                // �������������
                int afterCount = userPtr->getAllNotifications().size();
                int cleanedCount = beforeCount - afterCount;
                totalCleaned += cleanedCount;
            }
        }

        if (totalCleaned > 0) {
            logger->logInfo("NOTIFICATION", "�Զ������� " + std::to_string(totalCleaned) + " ������֪ͨ");
        }
    }

    // ��������֪ͨ����ǿ�汾
    void generateAllNotifications() {
        logger->logInfo("NOTIFICATION", "��ʼ����ϵͳ֪ͨ");

        generateDueSoonNotifications();
        generateFineNotifications();

        // �������֪ͨ
        cleanupNotifications();

        logger->logInfo("NOTIFICATION", "֪ͨ������ɣ�ϵͳ�й��� " +
            std::to_string(getTotalUnreadNotifications()) + " ��δ��֪ͨ");
    }

    // ��ȡϵͳ�������û���δ��֪ͨ����
    int getTotalUnreadNotifications() const {
        int total = 0;
        for (auto user : users) {
            if (user != nullptr) {
                total += static_cast<User*>(user)->getUnreadNotificationCount();
            }
        }
        return total;
    }

    // �ֶ�����ض��û����ض�����֪ͨΪ�Ѷ�
    void markNotificationsAsRead(const std::string & username, Notification::Type type) {
        User* user = findUser(username);
        if (user != nullptr) {
            if (type == Notification::DUE_SOON) {
                user->markDueNotificationsAsRead();
            }
            else if (type == Notification::OVERDUE_FINE) {
                clearFineNotifications(*user);
            }
            logger->logInfo("NOTIFICATION", "�ֶ�����û� " + username + " ��֪ͨΪ�Ѷ�");
        }
    }

    // ��鲢����ԤԼ����
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
            logger->logInfo("NOTIFICATION", "������ " + std::to_string(expiredCount) + " ������ԤԼ֪ͨ");
        }
    }

    // ����ԤԼ����֪ͨ
    void sendReservationAvailableNotification(const std::string & username, const std::string & isbn) {
        User* user = findUser(username);
        if (user != nullptr) {
            std::string bookTitle = findBookTitle(isbn);
            Date expiryDate = Date::currentdate();
            expiryDate.addDays(7); // 7����ȷ�Ͻ���

            Notification noti(Notification::RESERVATION_AVAILABLE, isbn, bookTitle, expiryDate);
            user->addNotification(noti);

            logger->logNotificationSent(username, "ԤԼ����");
        }
    }

    // ��ȡ֪ͨͳ����Ϣ
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

    // ��ӡ֪ͨͳ����Ϣ�������ã�
    void printNotificationStats() const {
        auto stats = getNotificationStats();
        std::cout << "=== ֪ͨϵͳͳ�� ===" << std::endl;
        std::cout << "��֪ͨ��: " << stats.totalNotifications << std::endl;
        std::cout << "δ��֪ͨ��: " << stats.unreadCount << std::endl;
        std::cout << "��������: " << stats.dueSoonCount << std::endl;
        std::cout << "����֪ͨ: " << stats.fineCount << std::endl;
        std::cout << "ԤԼ֪ͨ: " << stats.reservationCount << std::endl;
    }
};