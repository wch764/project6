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

    // ����ISBN��������
    std::string findBookTitle(const std::string& isbn) const {
        auto it = std::find_if(books.begin(), books.end(),
            [&isbn](const Book& b) { return b.getISBN() == isbn; });

        if (it != books.end()) {
            return it->getTitle();
        }
        return "δ֪ͼ��";
    }

    // �����û�
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

    // ���ɼ�������֪ͨ
    void generateDueSoonNotifications() {
        Date currentDate = Date::currentdate();
        Date threeDaysLater = currentDate;
        threeDaysLater.addDays(3);

        std::cout << "��鼴�����ڵ�ͼ��..." << std::endl;

        for (const auto& info : borrowInfos) {
            // �����ѹ黹��ԤԼ��¼
            if (info.isReturned() || info.is_Reservation()) {
                continue;
            }

            Date dueDate = info.getDueDate();

            // ����Ƿ���3���ڵ���
            if (dueDate > currentDate-1 && dueDate < threeDaysLater) {
                std::cout << "���ּ�������ͼ��: " << info.getBookISBN()
                    << " �û�: " << info.getReaderUsername()
                    << " ������: " << dueDate.toString() << std::endl;

                User* user = findUser(info.getReaderUsername());
                if (user != nullptr) {
                    std::string bookTitle = findBookTitle(info.getBookISBN());
                    Notification dueSoonNoti(Notification::DUE_SOON,
                        info.getBookISBN(),
                        bookTitle,
                        dueDate);
                    user->addNotification(dueSoonNoti);

                    std::cout << "�����û� " << info.getReaderUsername()
                        << " ���͵�������" << std::endl;
                }
            }
        }
    }

    // ���ɷ���֪ͨ
    void generateFineNotifications() {
        Date currentDate = Date::currentdate();

        std::cout << "����з����ͼ��..." << std::endl;

        for (auto& info : borrowInfos) {
            // �����ѹ黹��ԤԼ��¼
            if (info.isReturned() || info.is_Reservation()) {
                continue;
            }

            // ���·�����
            info.updateFine(currentDate);
            double fineAmount = info.getFineAmount();

            if (fineAmount > 0) {
                std::cout << "�����з���ͼ��: " << info.getBookISBN()
                    << " �û�: " << info.getReaderUsername()
                    << " ����: " << fineAmount << " Ԫ" << std::endl;

                User* user = findUser(info.getReaderUsername());
                if (user != nullptr) {
                    std::string bookTitle = findBookTitle(info.getBookISBN());
                    Notification fineNoti(Notification::OVERDUE_FINE,
                        info.getBookISBN(),
                        bookTitle,
                        fineAmount);
                    user->addNotification(fineNoti);

                    std::cout << "�����û� " << info.getReaderUsername()
                        << " ���ͷ���֪ͨ" << std::endl;
                }
            }
        }
    }

    // ��������֪ͨ
    void generateAllNotifications() {
        std::cout << "=== ��ʼ����֪ͨ ===" << std::endl;

        generateDueSoonNotifications();
        generateFineNotifications();

        // �������֪ͨ
        for (auto user : users) {
            if (user != nullptr) {
                static_cast<User*>(user)->cleanupNotifications();
            }
        }

        std::cout << "=== ֪ͨ������� ===" << std::endl;
    }

    // ��ȡϵͳ�������û���δ��֪ͨ�����������ã�
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
