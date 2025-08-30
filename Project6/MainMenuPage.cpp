// MainMenuPage.cpp - ��Ӧ��ʵ��
#include"MainMenuPage.h"
#include"failmanage.h"
#include"Bookpage.h"
#include"BorrowStatisticsPage.h"
#include"ReaderInfoPage.h"
#include<iostream>
using namespace std;
MainMenuPage::MainMenuPage(vector<Book>& bs, vector<BorrowInfo>& bis, User* user,
    vector<Reader>& rdrs, ReservationManager& rm, NotificationManager& nm)
    : books(bs), borrowInfos(bis), currentUser(user), readers(rdrs),
    reservationManager(rm), notificationManager(nm) {
    std::cout << "MainMenuPage������ɣ���������: " << readers.size()
        << "��ʹ��ȫ��ԤԼ��������֪ͨ������" << endl;
}

bool MainMenuPage::display() const {
    std::cout << "��ӭ, " << currentUser->getName() << endl;
    std::cout << "=== ���˵� ===" << endl;
    std::cout << "1. ͼ�����" << endl;
    std::cout << "2. ������Ϣ" << endl;
    std::cout << "3. ����ͳ��" << endl;
    std::cout << "4. �޸�����" << endl;

    // ��ʾδ��֪ͨ����
    int unreadCount = currentUser->getUnreadNotificationCount();
    if (unreadCount > 0) {
        cout << "5. ��Ϣ���� (" << unreadCount << " ��δ����Ϣ)\n";
    }
    else {
        cout << "5. ��Ϣ����\n";
    }
    cout << "6. �˳���¼\n";
    return 0;
}

void MainMenuPage::performAction() {
    while (true) {
        // ��ÿ����ʾ�˵�ǰˢ��֪ͨ
        notificationManager.generateAllNotifications();

        display();
        int choice;
        cout << "��ѡ�����: ";
        if (validateInt(choice)) {
            switch (choice) {
            case 1: {
                cout << "����ͼ�����ҳ�棬ʹ��ȫ��ԤԼ������..." << endl;
                cout << "��ǰ��������: " << readers.size() << endl;

                std::vector<Reader*> readerPointers;
                convertToPointerVector(readers, readerPointers);

                cout << "����BookPage��ָ������: " << readerPointers.size() << endl;
                // ����ȫ��ԤԼ����������
                BookPage bookPage{ books, borrowInfos, readerPointers, currentUser, reservationManager };
                bookPage.performAction();

                cout << "�˳�ͼ�����ҳ�棬��ǰ��������: " << readers.size() << endl;
                break;
            }
            case 2: {
                ReaderInfoPage readerInfoPage{ readers, currentUser };
                readerInfoPage.performAction();
                break;
            }
            case 3: {
                BorrowStatisticsPage borrowStatsPage{ borrowInfos, books, currentUser };
                borrowStatsPage.performAction();
                break;
            }
            case 4: {
                string newPassword;
                cout << "����������: ";
                cin.ignore();
                getline(cin, newPassword);
                clearscreen();
                currentUser->changePassword(newPassword);
                cout << "�����޸ĳɹ�!" << endl;
                break;
            }
            case 5: {
                cout << "������Ϣ����..." << endl;

                // ʹ����ǿ��֪ͨ��ʾ����
                std::vector<Reader*> readerPointers;
                convertToPointerVector(readers, readerPointers);

                BookPage bookPage{ books, borrowInfos, readerPointers, currentUser, reservationManager };
                bookPage.showNotifications();
                break;
            }
            case 6:
                clearscreen();
                cout << "�˳���¼..." << endl;
                currentUser = nullptr;
                return;
            default:
                cout << "��Чѡ��!" << endl;
            }
        }
    }
}