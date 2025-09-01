#pragma once
#include<iostream>
#include<string>
#include"Page.h"
#include"Action.h"
#include<vector>
#include"Reader.h"
#include"Book.h"
#include"BorrowInfo.h"
#include"User.h"
#include"reservemanager.h"
#include"Notificationmanager.h"
#include"Bookpage.h"
#include"ReaderInfoPage.h"
#include"BorrowStatisticsPage.h"
#include"RankingPage.h"
using namespace std;
// ������ǿ�����˵�ҳ��
class MainMenuPage : public Page, public Action {
private:
    vector<Book>& books;
    vector<BorrowInfo>& borrowInfos;
    User* currentUser;
    vector<Reader>& readers;
    ReservationManager& reservationManager;
    NotificationManager& notificationManager;
    SystemLogger* logger;

public:
    MainMenuPage(vector<Book>& bs, vector<BorrowInfo>& bis, User* user,
        vector<Reader>& rdrs, ReservationManager& rm, NotificationManager& nm)
        : books(bs), borrowInfos(bis), currentUser(user), readers(rdrs),
        reservationManager(rm), notificationManager(nm) {
        logger = SystemLogger::getInstance();
    }

    bool display() const override {
        clearscreen();
        cout << "��ӭ, " << currentUser->getName() << endl;
        cout << "=== ���˵� ===" << endl;
        cout << "1. ͼ�����" << endl;
        cout << "2. ���߹���" << endl;
        cout << "3. ����ͳ��" << endl;
        cout << "4. ���а�" << endl;
        cout << "5. �޸�����" << endl;

        int unreadCount = currentUser->getUnreadNotificationCount();
        if (unreadCount > 0) {
            cout << "6. ��Ϣ���� (" << unreadCount << " ��δ����Ϣ)" << endl;
        }
        else {
            cout << "6. ��Ϣ����" << endl;
        }
        cout << "7. ϵͳ��Ϣ" << endl;
        cout << "8. �˳���¼" << endl;
        return false;
    }

    void performAction() override {
        while (true) {
            notificationManager.generateAllNotifications();
            display();

            int choice;
            cout << "��ѡ�����: ";
            
            if (validateInt(choice)) {
                switch (choice) {
                case 1: {
                   
                    // ͼ�������
                    logger->logInfo("MENU", currentUser->getUsername() + " ����ͼ�����");
                    std::vector<Reader*> readerPointers;
                    convertToPointerVector(readers, readerPointers);
                    BookPage bookPage{ books, borrowInfos, readerPointers, currentUser, reservationManager };
                    bookPage.performAction();
                    break;
                }
                case 2: {
                    // ���߹����� 
                    logger->logInfo("MENU", currentUser->getUsername() + " ������߹���");
                    ReaderInfoPage readerInfoPage{ readers, currentUser };
                    readerInfoPage.performAction();
                    break;
                }
                case 3: {
                    // ����ͳ�ƹ���
                    logger->logInfo("MENU", currentUser->getUsername() + " �鿴����ͳ��");
                    BorrowStatisticsPage borrowStatsPage{ borrowInfos, books, currentUser };
                    borrowStatsPage.performAction();
                    break;
                }
                case 4: {
                    // ���а���
                    logger->logInfo("MENU", currentUser->getUsername() + " �鿴���а�");
                    vector<Reader*> readerPtrs;
                    for (auto& reader : readers) {
                        readerPtrs.push_back(&reader);
                    }
                    RankingPage rankingPage(readers, books, currentUser);
                    rankingPage.performAction();
                    break;
                }
                case 5: {
                    // �޸�����
                    string newPassword;
                    cout << "����������: ";
                    cin.ignore();
                    getline(cin, newPassword);
                    currentUser->changePassword(newPassword);
                    logger->logInfo("AUTH", currentUser->getUsername() + " �޸�����");
                    cout << "�����޸ĳɹ�!" << endl;
                    break;
                }
                case 6: {
                    // ��Ϣ����
                    logger->logInfo("MENU", currentUser->getUsername() + " �鿴��Ϣ����");
                    showNotifications();
                    break;
                }
                case 7: {
                    // ϵͳ��Ϣ
                    showSystemInfo();
                    break;
                }
                case 8:
                    logger->logUserLogout(currentUser->getUsername());
                    cout << "�˳���¼..." << endl;
                    currentUser = nullptr;
                    return;
                default:
                    cout << "��Чѡ��!" << endl;
                    logger->logInputError("MENU", "1-8", to_string(choice));
                }
            }
        }
    }

private:
    void showNotifications() {
        clearscreen();
        cout << "=== ��Ϣ���� ===" << endl;

        auto allNotifications = currentUser->getAllNotifications();

        if (allNotifications.empty()) {
            cout << "û����֪ͨ\n";
            return;
        }

        // �����ͷ�����ʾ֪ͨ
        vector<Notification> reservationNotifications;
        vector<Notification> dueSoonNotifications;
        vector<Notification> fineNotifications;

        for (const auto& noti : allNotifications) {
            switch (noti.getType()) {
            case Notification::RESERVATION_AVAILABLE:
            case Notification::RESERVATION_EXPIRED:
                reservationNotifications.push_back(noti);
                break;
            case Notification::DUE_SOON:
                dueSoonNotifications.push_back(noti);
                break;
            case Notification::OVERDUE_FINE:
                fineNotifications.push_back(noti);
                break;
            }
        }

        // ��ʾ����֪ͨ
        if (!reservationNotifications.empty()) {
            cout << "\n��ԤԼ֪ͨ��" << endl;
            for (const auto& noti : reservationNotifications) {
                cout << "�� " << noti.getMessage();
                if (!noti.getIsRead()) cout << " [��]";
                cout << endl;
            }
        }

        if (!dueSoonNotifications.empty()) {
            cout << "\n���������ѡ�" << endl;
            for (const auto& noti : dueSoonNotifications) {
                cout << "�� " << noti.getMessage();
                if (!noti.getIsRead()) cout << " [��]";
                cout << endl;
            }
            currentUser->markDueNotificationsAsRead();
        }

        if (!fineNotifications.empty()) {
            cout << "\n������֪ͨ��" << endl;
            double totalFine = 0.0;
            for (const auto& noti : fineNotifications) {
                cout << "�� " << noti.getMessage() << " [δ����]" << endl;
                totalFine += noti.getFineAmount();
            }
            cout << "����ǰ�ܼ�δ������: " << totalFine << " Ԫ" << endl;
        }

        cout << "\n���س�������...";
        cin.ignore();
        cin.get();
    }

    void showSystemInfo() {
        clearscreen();
        cout << "=== ϵͳ��Ϣ ===" << endl;
        cout << "ϵͳ�汾: ͼ�����ϵͳ v2.0" << endl;
        cout << "��ǰ�û�: " << currentUser->getName()
            << " (" << currentUser->getUsername() << ")" << endl;
        cout << "�û�����: " << (currentUser->get_isadmin() ? "����Ա" : "��ͨ�û�") << endl;

        // ��ʾϵͳͳ��
        cout << "\n=== ϵͳͳ�� ===" << endl;
        cout << "���û���: " << readers.size() << endl;
        cout << "��ͼ����: " << books.size() << endl;
        cout << "�ܽ��ļ�¼: " << borrowInfos.size() << endl;

        // ͳ�Ƶ�ǰ����
        int currentBorrows = 0;
        int reservations = 0;
        for (const auto& info : borrowInfos) {
            if (!info.isReturned()) {
                if (info.is_Reservation()) {
                    reservations++;
                }
                else {
                    currentBorrows++;
                }
            }
        }
        cout << "��ǰ�ڽ�: " << currentBorrows << endl;
        cout << "��ǰԤԼ: " << reservations << endl;

        // ��ʾ֪ͨͳ��
        auto stats = notificationManager.getNotificationStats();
        cout << "\n=== ֪ͨͳ�� ===" << endl;
        cout << "ϵͳ��֪ͨ��: " << stats.totalNotifications << endl;
        cout << "δ��֪ͨ��: " << stats.unreadCount << endl;
        cout << "��������: " << stats.dueSoonCount << endl;
        cout << "����֪ͨ: " << stats.fineCount << endl;
        cout << "ԤԼ֪ͨ: " << stats.reservationCount << endl;

        cout << "\n��־�ļ�: " << logger->getLogFileName() << endl;
        cout << "��ǰʱ��: " << Date::currentdate().toString() << endl;

        cout << "\n���س�������...";
        cin.ignore();
        cin.get();
    }
    static void convertToPointerVector(vector<Reader>& readers, vector<Reader*>& pointers) {
        pointers.clear();
        pointers.reserve(readers.size());
        cout << "ת��ָ��������Դ���ݴ�С: " << readers.size() << endl;
        for (size_t i = 0; i < readers.size(); ++i) {
            pointers.push_back(&readers[i]);
            cout << "���ָ�� " << i << ": " << readers[i].getUsername() << endl;
        }
        cout << "ָ������ת����ɣ���С: " << pointers.size() << endl;
    }
};

