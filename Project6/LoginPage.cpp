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
        cout << "=== ͼ�����ϵͳ��¼ ===" << endl;
        cout << "1. ���ߵ�¼" << endl;
        cout << "2.����Ա��¼" << endl;
        cout << "3. �˳�" << endl;
        return 0;
    }

void LoginPage::performAction()  {
        int choice;
        cout << "��ѡ�����: ";
        cin >> choice;
        cin.ignore();

        if (choice == 1) {
            
            string username, password;
            
            while (true)
            {
                cout << "�û���: ";
                getline(cin, username);
                cout << "����: ";
                getline(cin, password);
                clearscreen();

                for (auto& reader : readers) {
                    if (reader.getUsername() == username && reader.getPassword() == password) {
                        currentUser = &reader;
                        
                        return;
                    }
                }
                cout << "�û������������!" << endl;
                cout << "1.���³���" << endl;
                cout << "2.�˳�����������Ա���һ�����" << endl;
                cout << "��ѡ�����: ";
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
                cout << "�û���: ";
                getline(cin, username);
                cout << "����: ";
                getline(cin, password);
                clearscreen();

                for (auto& admin : admins) {
                    if (admin.getUsername() == username && admin.getPassword() == password) {
                        currentUser = &admin;

                        cout << "��¼�ɹ�! ��ӭ, " << admin.getName() << endl;
                       
                        return;
                    }
                }
                cout << "�û������������!" << endl;
                cout << "1.���³���" << endl;
                cout << "2.�˳�" << endl;
                cout << "��ѡ�����: ";
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
