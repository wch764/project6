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
    cout << "=== 读者信息 ===" << endl;
    cout << "1. 查看个人信息" << endl;
    cout << "2. 查看所有读者" << endl;
    cout << "3. 返回主菜单" << endl;
    return i;
}

void ReaderInfoPage::performAction()  {
        while (true) {
            clearscreen();
            display();
            int choice;
            cout << "请选择操作: ";
            cin >> choice;
            cin.ignore();

            switch (choice) {
            case 1:
                showPersonalInfo();
                cout << "按回车键返回上一页" << endl;
                if(cin.get() == '\n')
                break;
            case 2:
                showAllReaders();
                cout << "按回车键返回上一页" << endl;
                if (cin.get() == '\n')
                break;
            case 3:
                return;
            default:
                cout << "无效选择!" << endl;
                cout << "按回车键返回上一页" << endl;
                if (cin.get() == '\n')
                    break;
            }
        }
    }

void  ReaderInfoPage::showPersonalInfo() const {
        clearscreen();
        cout << "=== 个人信息 ===" << endl;
        cout << "用户名: " << currentUser->getUsername() << endl;
        cout << "姓名: " << currentUser->getName() << endl;
        cout << "注册日期: ";
        currentUser->getRegistrationDate().displayDate();
        cout << endl;
    }

void  ReaderInfoPage::showAllReaders() const {
        clearscreen();
        cout << "=== 所有读者 ===" << endl;
        for (const auto& reader : readers) {
            cout << "用户名: " << reader.getUsername()
                << ", 密码：" << reader.getPassword()
                << ", 姓名: " << reader.getName()
                << ", 注册日期: ";
            reader.getRegistrationDate().displayDate();
            cout << endl;
        }
    }
