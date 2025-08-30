#include<iostream>
#include<string>
#include"Page.h"
#include"Action.h"
#include<vector>
#include"Reader.h"
#include"LoginPage.h"
#include"clearscreen.h"
using namespace std;
LoginPage::LoginPage(vector<Reader>& rdrs, vector<Admin>& Adm) : readers(rdrs), admins(Adm),currentUser(nullptr) {}

bool LoginPage::display() const {
        cout << "=== 图书管理系统登录 ===" << endl;
        cout << "1. 读者登录" << endl;
        cout << "2.管理员登录" << endl;
        cout << "3. 退出" << endl;
        return 0;
    }

void LoginPage::performAction()  {
        int choice;
        cout << "请选择操作: ";
        cin >> choice;
        cin.ignore();

        if (choice == 1) {
            
            string username, password;
            
            while (true)
            {
                cout << "用户名: ";
                getline(cin, username);
                cout << "密码: ";
                getline(cin, password);
                clearscreen();

                for (auto& reader : readers) {
                    if (reader.getUsername() == username && reader.getPassword() == password) {
                        currentUser = &reader;
                        
                        return;
                    }
                }
                cout << "用户名或密码错误!" << endl;
                cout << "1.重新尝试" << endl;
                cout << "2.退出，可至管理员处找回密码" << endl;
                cout << "请选择操作: ";
                cin >> choice;
                cin.ignore();
                if(choice==1){}
                if (choice == 2) { currentUser = nullptr; return; }
            }
        }
        if (choice == 2) {
            string username, password;
            
            while (true)
            {
                cout << "用户名: ";
                getline(cin, username);
                cout << "密码: ";
                getline(cin, password);
                clearscreen();

                for (auto& admin : admins) {
                    if (admin.getUsername() == username && admin.getPassword() == password) {
                        currentUser = &admin;

                        cout << "登录成功! 欢迎, " << admin.getName() << endl;
                       
                        return;
                    }
                }
                cout << "用户名或密码错误!" << endl;
                cout << "1.重新尝试" << endl;
                cout << "2.退出" << endl;
                cout << "请选择操作: ";
                cin >> choice;
                cin.ignore();
                if (choice == 1) {}
                if (choice == 2) { currentUser = nullptr; return; }
            }
           
        }
        if (choice==3)
        {
            currentUser = nullptr;
            return;
        }
    }
User* LoginPage::getCurrentUser() const { return currentUser; }
