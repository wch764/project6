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
       

        int unreadCount = currentUser->getUnreadNotificationCount();
        if (unreadCount > 0) {
            cout << "5. ��Ϣ���� (" << unreadCount << " ��δ����Ϣ)" << endl;
        }
        else {
            cout << "5. ��Ϣ����" << endl;
        }
        cout << "6. ϵͳ��Ϣ" << endl;
        cout << "7. �˳���¼" << endl;
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
                    /*vector<Reader*> readerPtrs;
                    for (auto& reader : readers) {
                        readerPtrs.push_back(&reader);
                    }*/
                    std::vector<Reader*> readerPointers;
                    convertToPointerVector(readers, readerPointers);
                    BookPage bookPage{ books, borrowInfos, readerPointers, currentUser, reservationManager };
                    RankingPage rankingPage(readers, books, currentUser, [&bookPage](const string& isbn) {
                        bookPage.borrowBook(isbn);  
                        });
                    rankingPage.performAction();
                    break;
                }
                
                case 5: {
                    // ��Ϣ����
                    logger->logInfo("MENU", currentUser->getUsername() + " �鿴��Ϣ����");
                    showNotifications();
                    break;
                }
                case 6: {
                    // ϵͳ��Ϣ
                    showSystemInfo();
                    break;
                }
                case 7:
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
        cout << "=== ��Ϣ���� ===" << endl;

        // ��ȡ����֪ͨ
        auto allNotifications = currentUser->getAllNotifications();

        if (allNotifications.empty()) {
            cout << "û����֪ͨ\n";
			system("pause");
        }

        // �����ͷ���֪ͨ
        vector<Notification> reservationNotifications;
        vector<Notification> dueSoonNotifications;
        vector<Notification> fineNotifications;
        vector<Notification> otherNotifications;

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
            default:
                otherNotifications.push_back(noti);
                break;
            }
        }

        // ��ʾԤԼ֪ͨ
        if (!reservationNotifications.empty()) {
            cout << "\n��ԤԼ֪ͨ��" << endl;
            vector<string> availableBooks;

            for (const auto& noti : reservationNotifications) {
                cout << "�� " << noti.getMessage();
                if (!noti.getIsRead()) cout << " [��]";
                cout << endl;

                if (noti.getType() == Notification::RESERVATION_AVAILABLE && !noti.getIsRead()) {
                    availableBooks.push_back(noti.getBookIsbn());
                }

            }

            // ����ԤԼȷ��
            if (!availableBooks.empty()) {
                cout << "\n�Ƿ�Ҫ������ЩԤԼͼ�飿(y/n): ";
                char choice;
                cin >> choice;

                if (choice == 'y' || choice == 'Y') {
                    cout << "��ʼ����ԤԼ����..." << endl;
                    for (const auto& isbn : availableBooks) {
                        bool result = confirmborrow(isbn, currentUser->getUsername());
                        if (result) {
                            cout << "ISBN " << isbn << " ����ȷ�ϳɹ�" << endl;
                        }
                        else {
                            cout << "ISBN " << isbn << " ����ȷ��ʧ��" << endl;
                        }
                    }

                    // ����Ѵ����ԤԼ֪ͨ
                    currentUser->getAvailableReservations();
                }
            }
        }

        // ��ʾ��������
        if (!dueSoonNotifications.empty()) {
            cout << "\n���������ѡ�" << endl;
            for (const auto& noti : dueSoonNotifications) {
                cout << "�� " << noti.getMessage();
                if (!noti.getIsRead()) cout << " [��]";
                cout << endl;
            }

            // ��ǵ���֪ͨΪ�Ѷ����û�����֪ͨҳ���
            currentUser->markDueNotificationsAsRead();
            cout << "���������ѱ��Ϊ�Ѷ�" << endl;
        }

        // ��ʾ����֪ͨ����Զ��ʾΪδ����
        if (!fineNotifications.empty()) {
            cout << "\n������֪ͨ��" << endl;
            double totalFine = 0.0;

            for (const auto& noti : fineNotifications) {
                cout << "�� " << noti.getMessage() << " [δ����]" << endl;
                totalFine += noti.getFineAmount();
            }

            cout << "����ǰ�ܼ�δ������: " << totalFine << " Ԫ" << endl;
            cout << "�뾡����ɷ���������Ӱ�����Ȩ�ޣ�" << endl;
        }

        // ��ʾ����֪ͨ
        if (!otherNotifications.empty()) {
            cout << "\n������֪ͨ��" << endl;
            for (const auto& noti : otherNotifications) {
                cout << "�� " << noti.getMessage();
                if (!noti.getIsRead()) cout << " [��]";
                cout << endl;
            }
        }

        cout << "\n֪ͨ�鿴��ϡ�" << endl;
        system("pause");
    }
    bool confirmborrow(const string& isbn, const string& username) {
        // ���������Ϊ��һ�У�ȷ������������
        cout << "*** CONFIRMBORROW ���������� *** ISBN=" << isbn << " �û�=" << username << endl;

        cout << "=== ȷ�Ͻ������̿�ʼ ===" << endl;
        cout << "ISBN: " << isbn << ", �û�: " << username << endl;

        // 1. ����ͼ��
        auto bookIt = find_if(books.begin(), books.end(),
            [&isbn](const Book& b) { return b.getISBN() == isbn; });

        if (bookIt == books.end()) {
            cout << "����ͼ�鲻����" << endl;
            return false;
        }

        Book& book = *bookIt;
        cout << "�ҵ�ͼ��: " << book.getTitle() << ", �ɽ�����: " << book.getAvailable() << endl;

        // 2. ���ͼ���Ƿ�ɽ�
        if (!book.isAvailable()) {
            cout << "���鵱ǰ���ɽ裬��������: " << book.getAvailable() << endl;
            return false;
        }

        // 3. �����û���ԤԼ��¼
        cout << "��ʼ����ԤԼ��¼..." << endl;
        auto reservationIt = find_if(borrowInfos.begin(), borrowInfos.end(),
            [&](BorrowInfo& bi) {
                cout << "����¼: �û�=" << bi.getReaderUsername()
                    << " ISBN=" << bi.getBookISBN()
                    << " ԤԼ=" << (bi.is_Reservation() ? "��" : "��")
                    << " �ѻ�=" << (bi.isReturned() ? "��" : "��") << endl;

                return bi.getBookISBN() == isbn &&
                    bi.getReaderUsername() == username &&
                    bi.is_Reservation() &&
                    !bi.isReturned();
            });

        if (reservationIt == borrowInfos.end()) {
            cout << "����δ�ҵ����û�����ЧԤԼ��¼" << endl;
            cout << "borrowInfos����: " << borrowInfos.size() << endl;
            return false;
        }

        cout << "*** �ҵ�ԤԼ��¼����ʼת��... ***" << endl;

        // 4. ת��ԤԼ��¼Ϊ���ļ�¼
        BorrowInfo& reservation = *reservationIt;

        // �����µĽ������ں�Ӧ������
        Date currentDate = Date::currentdate();
        reservation.setBorrowDate(currentDate);
        reservation.setDueDate(BorrowInfo::calculateDueDate(currentDate));

        // ת��Ϊ�������ļ�¼
        reservation.converttoborrow();
        currentUser->addBorrowedCount();
        bookIt->addTotalBorrowCount();

        cout << "ԤԼ��¼��ת��Ϊ���ļ�¼" << endl;
        cout << "  - ��������: " << reservation.getBorrowDate().toString() << endl;
        cout << "  - Ӧ������: " << reservation.getDueDate().toString() << endl;

        // 5. ����ͼ��״̬
        if (!book.borrowBook()) {
            cout << "����ͼ�����״̬����ʧ��" << endl;
            return false;
        }

        // 6. ��ԤԼ�������Ƴ�
        if (reservationManager.fulfillReservation(isbn)) {
            cout << "�Ѵ�ԤԼ�������Ƴ�" << endl;
        }
        else {
            cout << "���棺��ԤԼ�����Ƴ�ʧ��" << endl;
        }

        cout << "*** CONFIRMBORROW �ɹ���� *** Ӧ�����ڣ�" << reservation.getDueDate().toString() << endl;
        return true;
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

