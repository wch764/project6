#include<string>
#include"Page.h"
#include"Action.h"
#include<vector>
#include"Reader.h"
#include"LoginPage.h"
#include"clearscreen.h"
#include"Book.h"
#include"BorrowInfo.h"
#include"BorrowStatisticsPage.h"
#include"Bookpage.h"
#include"ReaderInfoPage.h"
using namespace std;
ReaderInfoPage::ReaderInfoPage(vector<Reader>& rdrs, User* user)
        : readers(rdrs), currentUser(user) {
    }

bool ReaderInfoPage::display() const {
    bool i = currentUser->get_isadmin();
    cout << "=== ������Ϣ ===" << endl;
    cout << "1. �鿴������Ϣ" << endl;
    cout << "2. �鿴���ж���" << endl;
    cout << "3. �������˵�" << endl;
    return i;
}

void ReaderInfoPage::performAction()  {
        while (true) {
            clearscreen();
            display();
            int choice;
            cout << "��ѡ�����: ";
            cin >> choice;
            cin.ignore();

            switch (choice) {
            case 1:
                showPersonalInfo();
                cout << "���س���������һҳ" << endl;
                if(cin.get() == '\n')
                break;
            case 2:
                showAllReaders();
                cout << "���س���������һҳ" << endl;
                if (cin.get() == '\n')
                break;
            case 3:
                return;
            default:
                cout << "��Чѡ��!" << endl;
                cout << "���س���������һҳ" << endl;
                if (cin.get() == '\n')
                    break;
            }
        }
    }

void  ReaderInfoPage::showPersonalInfo() const {
        clearscreen();
        cout << "=== ������Ϣ ===" << endl;
        cout << "�û���: " << currentUser->getUsername() << endl;
        cout << "����: " << currentUser->getName() << endl;
        cout << "ע������: ";
        currentUser->getRegistrationDate().displayDate();
        cout << endl;
    }

void  ReaderInfoPage::showAllReaders() const {
        clearscreen();
        cout << "=== ���ж��� ===" << endl;
        for (const auto& reader : readers) {
            cout << "�û���: " << reader.getUsername()
                << ", ���룺" << reader.getPassword()
                << ", ����: " << reader.getName()
                << ", ע������: ";
            reader.getRegistrationDate().displayDate();
            cout << endl;
        }
    }
