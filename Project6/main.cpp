#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <ctime>
#include <iomanip>
#include <algorithm>
#include <limits>
#include <cstdio>
#include <filesystem> // C++17

#include "Serializable.h"
#include "Action.h"
#include "Page.h"
#include "Date.h"
#include "Book.h"
#include "Reader.h"
#include "BorrowInfo.h"
#include "LoginPage.h"
#include "MainMenuPage.h"
#include "Admin.h"
#include "ReserveManager.h"
#include "Notificationmanager.h"
#include "SystemLogger.h"
#include "RankingPage.h"
#include"failmanager.h"

using namespace std;



// �Ľ������ݼ��غ����������ϸ�Ĵ��������־��¼
template <typename T>
void loadDataFromFile(const string& filename, vector<T>& items) {
    auto logger = SystemLogger::getInstance();

    ifstream file(filename);
    if (!file.is_open()) {
        string error = "�޷����ļ�: " + filename;
        logger->logDataLoadError(filename, error);
        cout << error << " (������״����У�����������)" << endl;
        return;
    }

    logger->logInfo("DATA", "��ʼ�����ļ�: " + filename);
    T item;
    int count = 0;
    int errorCount = 0;

    while (item.loadFromStream(file)) {
        // ��֤���ص�����
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
        else {
            errorCount++;
        }
    }

    file.close();

    if (errorCount > 0) {
        logger->logWarning("DATA", filename + " ���� " + to_string(errorCount) + " ����Ч��¼������");
    }

    logger->logDataLoad(filename, count);
    cout << "�� " << filename << " �ɹ������� " << count << " ����¼";
    if (errorCount > 0) {
        cout << "�������� " << errorCount << " ����Ч��¼";
    }
    cout << endl;
}

// �Ľ������ݱ��溯������ӱ��ݹ���
template <typename T>
void saveDataToFile(const string& filename, const vector<T>& items) {
    auto logger = SystemLogger::getInstance();

    // �������ݣ����ԭ�ļ����ڣ�
    if (filesystem::exists(filename)) {
        string backupName = filename + ".backup";
        try {
            filesystem::copy_file(filename, backupName,
                filesystem::copy_options::overwrite_existing);
            logger->logInfo("DATA", "���������ļ�: " + backupName);
        }
        catch (const exception& e) {
            logger->logWarning("DATA", "�����ļ�����ʧ��: " + string(e.what()));
        }
    }

    ofstream file(filename, ios::trunc);
    if (!file.is_open()) {
        string error = "�޷����ļ����б���: " + filename;
        logger->logDataSaveError(filename, error);
        cout << error << endl;
        return;
    }

    int count = 0;
    int errorCount = 0;

    for (const auto& item : items) {
        try {
            item.saveToStream(file);
            count++;
        }
        catch (const exception& e) {
            errorCount++;
            logger->logError("DATA", "�����¼ʱ����: " + string(e.what()));
        }
    }

    file.close();

    if (errorCount > 0) {
        logger->logWarning("DATA", "���� " + filename + " ʱ�� " + to_string(errorCount) + " ����¼ʧ��");
    }

    logger->logDataSave(filename, count);
    cout << "�ɹ������� " << count << " ����¼�� " << filename;
    if (errorCount > 0) {
        cout << "���� " << errorCount << " ����¼����ʧ��";
    }
    cout << endl;
}

// �ؽ�ԤԼ���еĸĽ��汾
void rebuildReservationQueue(ReservationManager& manager, const vector<BorrowInfo>& borrowInfos) {
    auto logger = SystemLogger::getInstance();
    logger->logInfo("RESERVATION", "��ʼ�ؽ�ԤԼ����");

    int rebuildCount = 0;
    for (size_t i = 0; i < borrowInfos.size(); ++i) {
        const BorrowInfo& info = borrowInfos[i];
        if (info.is_Reservation() && !info.isReturned()) {
            if (manager.addReservation(i)) {
                rebuildCount++;
            }
        }
    }

    logger->logInfo("RESERVATION", "ԤԼ�����ؽ���ɣ��ؽ��� " + to_string(rebuildCount) + " ��ԤԼ");
    cout << "ԤԼ�����ؽ���ɣ��ؽ��� " << rebuildCount << " ��ԤԼ" << endl;
}

// ��ʼ��ʾ�����ݵĸĽ��汾
void initializeSampleData(vector<Book>& books, vector<Reader>& readers, vector<Admin>& admins) {
    auto logger = SystemLogger::getInstance();

    if (books.empty()) {
        logger->logInfo("INIT", "��ʼ��ʾ��ͼ������");
        cout << "ͼ����ϢΪ�գ����ʾ��ͼ��..." << endl;

        // ��Ӳ�ͬ�����ʾ��ͼ��
        books.emplace_back("9787111636667", "C++ Primer", "Stanley Lippman",
            "��� C++ �����", "����C++�̳�", 5,
            BookCategory::COMPUTER_SCIENCE_INFORMATION);

        books.emplace_back("9787302517969", "�����������ϵͳ", "Randal Bryant",
            "����� ϵͳ", "�����ϵͳ����̲�", 3,
            BookCategory::COMPUTER_SCIENCE_INFORMATION);

        books.emplace_back("9787115419663", "�㷨����", "Thomas Cormen",
            "�㷨 ���ݽṹ", "�㷨��ƾ���̲�", 4,
            BookCategory::NATURAL_SCIENCES_MATHEMATICS);

        books.emplace_back("9787020002207", "��¥��", "��ѩ��",
            "�ŵ���ѧ ����", "�й��ŵ���ѧ�Ĵ�����֮һ", 10,
            BookCategory::LITERATURE_RHETORIC);

        books.emplace_back("9787100018616", "��ѧͨʷ", "������",
            "��ѧ ˼��ʷ", "�й���ѧͨʷ", 6,
            BookCategory::PHILOSOPHY_PSYCHOLOGY);

        saveDataToFile("books.dat", books);
        logger->logInfo("INIT", "ʾ��ͼ�����ݳ�ʼ�����");
    }

    if (admins.empty()) {
        logger->logInfo("INIT", "��ʼ��Ĭ�Ϲ���Ա�˺�");
        cout << "����Ա��ϢΪ�գ�����Ĭ�Ϲ���Ա�˺�..." << endl;
        admins.emplace_back("admin", "123456", "ϵͳ����Ա");
        saveDataToFile("admins.dat", admins);
        logger->logInfo("INIT", "Ĭ�Ϲ���Ա�˺Ŵ������");
    }
}

// ϵͳ�������
void performSystemHealthCheck(const vector<Reader>& readers, const vector<Book>& books,
    const vector<BorrowInfo>& borrowInfos) {
    auto logger = SystemLogger::getInstance();
    logger->logInfo("HEALTH", "��ʼϵͳ�������");

    int issues = 0;

    // �������һ����
    for (const auto& borrowInfo : borrowInfos) {
        if (!borrowInfo.is_Reservation() && !borrowInfo.isReturned()) {
            // �������Ƿ����
            auto readerIt = find_if(readers.begin(), readers.end(),
                [&](const Reader& r) { return r.getUsername() == borrowInfo.getReaderUsername(); });

            if (readerIt == readers.end()) {
                logger->logWarning("HEALTH", "���ֹ����Ľ��ļ�¼���û�������: " + borrowInfo.getReaderUsername());
                issues++;
            }

            // ���ͼ���Ƿ����
            auto bookIt = find_if(books.begin(), books.end(),
                [&](const Book& b) { return b.getISBN() == borrowInfo.getBookISBN(); });

            if (bookIt == books.end()) {
                logger->logWarning("HEALTH", "���ֹ����Ľ��ļ�¼��ͼ�鲻����: " + borrowInfo.getBookISBN());
                issues++;
            }
        }
    }

    if (issues == 0) {
        logger->logInfo("HEALTH", "ϵͳ���������ɣ�δ��������");
    }
    else {
        logger->logWarning("HEALTH", "ϵͳ���������ɣ����� " + to_string(issues) + " ������");
    }
}

int main() {
    // ��ʼ��ϵͳ��־
    auto logger = SystemLogger::getInstance();
    logger->logSystemStart();

    try {
        vector<Reader> readers;
        vector<Admin> admins;
        vector<Book> books;
        vector<BorrowInfo> borrowInfos;

        cout << "=== ͼ�����ϵͳ v2.0 ���� ===" << endl;
        logger->logInfo("SYSTEM", "��ʼ��������");

        // ��������
        loadDataFromFile("readers.dat", readers);
        loadDataFromFile("books.dat", books);
        loadDataFromFile("borrowinfo.dat", borrowInfos);
        loadDataFromFile("admins.dat", admins);

        cout << "���ݼ������ - ����: " << readers.size()
            << ", ����Ա: " << admins.size()
            << ", ͼ��: " << books.size()
            << ", ���ļ�¼: " << borrowInfos.size() << endl;

        // ��ʼ��ʾ������
        initializeSampleData(books, readers, admins);

        // ϵͳ�������
        performSystemHealthCheck(readers, books, borrowInfos);

        // ����ָ����������֪ͨ����
        vector<Reader*> readerPointers;
        for (auto& reader : readers) {
            readerPointers.push_back(&reader);
        }

        // ����ȫ�����
        ReservationManager* globalReservationManager = new ReservationManager(borrowInfos);
        NotificationManager* notificationManager =
            new NotificationManager(borrowInfos, books, readerPointers);

        // �ؽ�ԤԼ����
        rebuildReservationQueue(*globalReservationManager, borrowInfos);

        // ����֪ͨ
        logger->logInfo("NOTIFICATION", "����ϵͳ֪ͨ");
        notificationManager->generateAllNotifications();

        auto stats = notificationManager->getNotificationStats();
        cout << "֪ͨϵͳ��ʼ����ɣ���ǰ�� " << stats.unreadCount << " ��δ��֪ͨ" << endl;

        // �������ļ�
        if (readers.empty()) {
            cout << "������ϢΪ�գ�����ע���û���" << endl;
            logger->logWarning("SYSTEM", "ϵͳ���޶�������");
            delete globalReservationManager;
            delete notificationManager;
            return 0;
        }

        // ������¼ҳ��
        LoginPage loginPage(readers, admins);

        // ������ѭ��
        while (true) {
            try {
                // ��ÿ����ʾ��¼����ǰ����֪ͨ
                notificationManager->generateAllNotifications();

                clearscreen();
                cout << "=== ͼ�����ϵͳ v2.0 ===" << endl;
                loginPage.display();
                loginPage.performAction();
                User* currentUser = loginPage.getCurrentUser();

                if (!currentUser) {
                    break;  // �û�ѡ���˳�
                }

                logger->logUserLogin(currentUser->getUsername(), currentUser->get_isadmin());

                cout << "�û���¼�ɹ�: " << currentUser->getUsername();
                int unreadCount = currentUser->getUnreadNotificationCount();
                if (unreadCount > 0) {
                    cout << " (�� " << unreadCount << " ��δ��֪ͨ)";
                }
                cout << endl;
           



                // ������ǿ�����˵�
                MainMenuPage MainMenu(books, borrowInfos, currentUser,
                    readers, *globalReservationManager,
                    *notificationManager);
                MainMenu.performAction();

            }
            catch (const exception& e) {
                logger->logSystemError("MAIN", "������ѭ���쳣: " + string(e.what()));
                cout << "ϵͳ��������: " << e.what() << endl;
                cout << "���س�������..." << endl;
                cin.ignore();
                cin.get();
            }
        }

        cout << "\n=== ��ʼ�������� ===" << endl;
        logger->logInfo("SYSTEM", "��ʼ����ϵͳ����");

        cout << "����ǰ����ͳ�� - ����: " << readers.size()
            << ", ͼ��: " << books.size()
            << ", ���ļ�¼: " << borrowInfos.size() << endl;

        // ��������
        saveDataToFile("readers.dat", readers);
        saveDataToFile("books.dat", books);
        saveDataToFile("borrowinfo.dat", borrowInfos);
        saveDataToFile("admins.dat", admins);

        logger->logInfo("SYSTEM", "�������ݱ������");

        // ������Դ
        delete globalReservationManager;
        delete notificationManager;

        logger->logSystemShutdown();
        cout << "ϵͳ�Ѱ�ȫ�˳��������ѱ��档" << endl;

    }
    catch (const exception& e) {
        logger->logCritical("SYSTEM", "ϵͳ���ش���: " + string(e.what()));
        cout << "ϵͳ�������ش���: " << e.what() << endl;
        cout << "�����Ѽ�¼����־�ļ��С�" << endl;
        return 1;
    }

    return 0;
}