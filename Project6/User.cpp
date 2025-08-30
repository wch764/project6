#include<iostream>
#include"Serializable.h"
#include"Date.h"
#include"User.h"
#include<fstream>
#include<string>
using namespace std;
User::User(const string& uname, const string& pwd , const string& nm )
        : username(uname), password(pwd), name(nm){
        registrationDate.currentDate();
        std::cout << "����User: " << uname << std::endl;
    }
bool User::loadFromStream(istream& file)  {
    try {
        if (!getline(file, username)) return false;
        if (!getline(file, password)) return false;
        if (!getline(file, name)) return false;

        string dateStr;
        if (!getline(file, dateStr)) return false;
        // ����ע������
        size_t pos1 = dateStr.find('/');
        size_t pos2 = dateStr.find('/', pos1 + 1);
        if (pos1 != string::npos && pos2 != string::npos) {
            registrationDate = Date(stoi(dateStr.substr(0, pos1)),
                stoi(dateStr.substr(pos1 + 1, pos2 - pos1 - 1)),
                stoi(dateStr.substr(pos2 + 1)));
        }

        // ����֪ͨ��Ϣ
        string notificationCountStr;
        if (getline(file, notificationCountStr) && !notificationCountStr.empty()) {
            int notificationCount = stoi(notificationCountStr);
            notifications.clear();

            for (int i = 0; i < notificationCount; ++i) {
                string typeStr, isbn, title, expiryStr;
                if (getline(file, typeStr) && getline(file, isbn) &&
                    getline(file, title) && getline(file, expiryStr)) {

                    Notification::Type type = static_cast<Notification::Type>(stoi(typeStr));

                    // ������������
                    size_t pos1 = expiryStr.find('/');
                    size_t pos2 = expiryStr.find('/', pos1 + 1);
                    Date expiryDate(stoi(expiryStr.substr(0, pos1)),
                        stoi(expiryStr.substr(pos1 + 1, pos2 - pos1 - 1)),
                        stoi(expiryStr.substr(pos2 + 1)));

                    notifications.emplace_back(type, isbn, title, expiryDate);
                }
            }
        }

        return true;
    }
    catch (...) {
        return false;
    }
}

void User::saveToStream(ostream& file) const {
    file << username << "\n" << password << "\n" << name << "\n";
    file << registrationDate.toString() << "\n";
    file << notifications.size() << '\n';
    for (const auto& noti : notifications) {
        file << static_cast<int>(noti.getType()) << '\n'
            << noti.getBookIsbn() << '\n'
            << noti.getBookTitle() << '\n'  // ��Ҫ��Notification�������getBookTitle()����
            << noti.getExpiryDate().toString() << '\n';
    }
}
void User::loadFromFile(const string& filename){
    ifstream file(filename);
    loadFromStream(file);
}
void User::saveToFile(const string& filename) const
{
    ofstream file(filename);
    if (!file.is_open()) {
        cerr << "�޷����ļ�: " << filename << endl;
        return;
    }
    saveToStream(file);  // �������е� saveToStream ����
    file.close();
}
void User::changePassword(const string& newPassword) {
        password = newPassword;
    }

string User::getUsername() const { return username; }
string User::getPassword() const { return password; }
string User::getName() const { return name; }
Date User::getRegistrationDate() const { return registrationDate; }
