#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <ctime>
#include <iomanip>
#include <algorithm>
#include <limits>
#include"Serializable.h"
#include"Action.h"
#include"Page.h"
#include"Date.h"
#include"Book.h"
#include"Reader.h"
#include"BorrowInfo.h"
#include"LoginPage.h"
#include"MainMenuPage.h"
#include"Admin.h"
#include"reservemanager.h"
#include"notificationcenter.h" 
using namespace std;

// �Ľ������ݼ��غ�������ӵ�����Ϣ
template <typename T>
void loadDataFromFile(const string& filename, vector<T>& items) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "�޷����ļ�: " << filename << endl;
        return;
    }

    cout << "��ʼ�����ļ�: " << filename << endl;
    T item;
    int count = 0;

    while (item.loadFromStream(file)) {
        // ��ͬ���͵Ŀ�״̬�жϷ���
        bool isValid = false;

        if constexpr (is_same_v<T, Reader> || is_same_v<T, Admin>) {
            isValid = !item.getUsername().empty();
        }
        else if constexpr (is_same_v<T, Book>) {
            isValid = !item.getISBN().empty();
        }
        else if constexpr (is_same_v<T, BorrowInfo>) {
            isValid = !item.getBookISBN().empty();
        }

        if (isValid) {
            items.push_back(item);
            count++;
        }
    }

    cout << "�� " << filename << " ������ " << count << " ����¼" << endl;
    file.close();
}

// �Ľ������ݱ��溯��
template <typename T>
void saveDataToFile(const string& filename, const vector<T>& items) {
    cout << "���� " << items.size() << " ����¼�� " << filename << endl;

    ofstream file(filename, ios::trunc); // ʹ��truncģʽ����ļ�
    if (!file.is_open()) {
        cout << "�޷����ļ����б���: " << filename << endl;
        return;
    }

    int count = 0;
    for (const auto& item : items) {
        item.saveToStream(file);
        count++;
    }

    file.close();
    cout << "�ɹ������� " << count << " ����¼�� " << filename << endl;
}
void rebuildReservationQueue(ReservationManager& manager, const vector<BorrowInfo>& borrowInfos) {
    cout << "=== �ؽ�ԤԼ���� ===" << endl;

    for (size_t i = 0; i < borrowInfos.size(); ++i) {
        const BorrowInfo& info = borrowInfos[i];
        if (info.is_Reservation() && !info.isReturned()) {
            cout << "�ؽ�ԤԼ: �û�=" << info.getReaderUsername()
                << ", ISBN=" << info.getBookISBN()
                << ", ���=" << info.getReservationSequence() << endl;

            manager.addReservation(i);
        }
    }

    cout << "ԤԼ�����ؽ����" << endl;
}
int main() {
    vector<Reader> readers;
    vector<Admin> admins;
    vector<Book> books;
    vector<BorrowInfo> borrowInfos;

    cout << "=== ��ʼ�������� ===" << endl;

    // ��������
    loadDataFromFile("readers.dat", readers);
    loadDataFromFile("books.dat", books);
    loadDataFromFile("borrowinfo.dat", borrowInfos);
    loadDataFromFile("admins.dat", admins);

    cout << "������� - ����: " << readers.size()
        << ", ����Ա: " << admins.size()
        << ", ͼ��: " << books.size()
        << ", ���ļ�¼: " << borrowInfos.size() << endl;

    // ����ָ����������֪ͨ����
    std::vector<Reader*> readerPointers;
    for (auto& reader : readers) {
        readerPointers.push_back(&reader);
    }

    // ����ȫ��ԤԼ������
    ReservationManager *globalReservationManager = new ReservationManager(borrowInfos);

    // ����֪ͨ������
    NotificationManager notificationManager(borrowInfos, books, readerPointers);

    // �ؽ�ԤԼ����
    rebuildReservationQueue(*globalReservationManager, borrowInfos);

    // ����֪ͨ��ϵͳ����ʱ�������֪ͨ��
    cout << "��鲢����֪ͨ..." << endl;
    notificationManager.generateAllNotifications();

    cout << "ϵͳ���ܹ��� " << notificationManager.getTotalUnreadNotifications()
        << " ��δ��֪ͨ" << endl;

    // �������ļ�
    if (readers.empty()) {
        cout << "������ϢΪ�գ�����ע���û���" << endl;
        delete globalReservationManager;
        return 0;
    }

    // ���ͼ��Ϊ�գ����һЩʾ��ͼ��
    if (books.empty()) {
        cout << "ͼ����ϢΪ�գ����ʾ��ͼ��..." << endl;
        books.emplace_back("9787111636667", "C++ Primer", "Stanley Lippman", "��� C++ �����", "����C++�̳�", 5);
        books.emplace_back("9787302517969", "�����������ϵͳ", "Randal Bryant", "����� ϵͳ", "�����ϵͳ����̲�", 3);
        saveDataToFile("books.dat", books);
    }

    // ��ӡ��ǰ�û��б����ڵ���
    cout << "\n��ǰϵͳ�е��û���" << endl;
    for (size_t i = 0; i < readers.size(); ++i) {
        cout << i << ": " << readers[i].getUsername() << " - " << readers[i].getName();
        int unreadCount = readers[i].getUnreadNotificationCount();
        if (unreadCount > 0) {
            cout << " (" << unreadCount << " ��δ��֪ͨ)";
        }
        cout << endl;
    }

    // ������¼ҳ��
    LoginPage loginPage(readers, admins);

    while (true) {
        // ��ÿ����ʾ��¼����ǰ����֪ͨ
        cout << "\nˢ��֪ͨ..." << endl;
        notificationManager.generateAllNotifications();

        loginPage.display();
        loginPage.performAction();
        User* currentUser = loginPage.getCurrentUser();

        if (!currentUser) {
            break;  // �û�ѡ���˳�
        }

        cout << "�û���¼�ɹ�: " << currentUser->getUsername();
        int unreadCount = currentUser->getUnreadNotificationCount();
        if (unreadCount > 0) {
            cout << " (�� " << unreadCount << " ��δ��֪ͨ)";
        }
        cout << endl;

        // �������˵�ҳ�棬����ȫ��ԤԼ��������֪ͨ������
        MainMenuPage mainMenu(books, borrowInfos, currentUser, readers,
            *globalReservationManager, notificationManager);
        mainMenu.performAction();
    }

    cout << "\n=== ��ʼ�������� ===" << endl;
    cout << "����ǰ����ͳ�� - ����: " << readers.size()
        << ", ͼ��: " << books.size()
        << ", ���ļ�¼: " << borrowInfos.size() << endl;

    // ��������
    saveDataToFile("readers.dat", readers);
    saveDataToFile("books.dat", books);
    saveDataToFile("borrowinfo.dat", borrowInfos);
    saveDataToFile("admins.dat", admins);

    // ����ȫ����Դ
    delete globalReservationManager;
    globalReservationManager = nullptr;

    cout << "ϵͳ���˳��������ѱ��档" << endl;
    return 0;
}