#pragma once
#include"Serializable.h"
#include<iostream>
#include"Date.h"
#include<fstream>
#include<string>
#include"notification.h"
using namespace std;
// ������
class User : public Serializable, public Date {
private:
    string username;
    string password;
    string name;
    Date registrationDate;
    std::vector<Notification> notifications;


public:
    User(const string& uname = "", const string& pwd = "", const string& nm = "");
    virtual ~User()
    {
        std::cout << "����User: " << username << std::endl;
    }
    //��ȡ
    bool loadFromStream(istream& file) override;


    void saveToStream(ostream& file) const override;

    // ����ԭ�нӿڵ��޸�ʵ��
    void loadFromFile(const string& filename) override;


    void saveToFile(const string& filename) const override;
    void changePassword(const string& newPassword);
    string getUsername() const;
    string getPassword() const;
    string getName() const;
    virtual bool get_isadmin() = 0;
    Date getRegistrationDate() const;
  
    std::vector<Notification> getNotifications() {
        auto result = notifications;
        notifications.clear(); // ��ȡ�����
        return result;
    }

   
    // ���֪ͨ
    void addNotification(const Notification& noti) {
        // ����Ƿ��Ѵ�����ͬ��֪ͨ�������ظ���
        auto it = std::find_if(notifications.begin(), notifications.end(),
            [&noti](const Notification& existing) {
                return existing.getType() == noti.getType() &&
                    existing.getBookIsbn() == noti.getBookIsbn();
            });

        if (it != notifications.end()) {
            // ��������֪ͨ
            *it = noti;
        }
        else {
            // �����֪ͨ
            notifications.push_back(noti);
        }
    }

    // ��ȡ����֪ͨ������գ�������ʾ��
    std::vector<Notification> getAllNotifications() const {
        std::vector<Notification> result;
        for (const auto& noti : notifications) {
            if (noti.shouldDisplay()) {
                result.push_back(noti);
            }
        }
        return result;
    }

    // ��ȡδ��֪ͨ����
    int getUnreadNotificationCount() const {
        int count = 0;
        for (const auto& noti : notifications) {
            if (!noti.getIsRead() && noti.shouldDisplay()) {
                count++;
            }
        }
        return count;
    }

    // ��ǵ���֪ͨΪ�Ѷ�
    void markDueNotificationsAsRead() {
        for (auto& noti : notifications) {
            if (noti.getType() == Notification::DUE_SOON) {
                noti.markAsRead();
            }
        }
    }

    // ��ȡ�ɽ��ĵ�ԤԼ֪ͨ
    std::vector<Notification> getAvailableReservations() {
        std::vector<Notification> result;
        for (auto& noti : notifications) {
            if (noti.getType() == Notification::RESERVATION_AVAILABLE && !noti.getIsRead()) {
                result.push_back(noti);
                noti.markAsRead(); // ԤԼ֪ͨ��ȡ����Ϊ�Ѷ�
            }
        }
        return result;
    }

    // �������֪ͨ
    void cleanupNotifications() {
        auto currentDate = Date::currentdate();
        notifications.erase(
            std::remove_if(notifications.begin(), notifications.end(),
                [currentDate](const Notification& noti) {
                    // �Ƴ��Ѷ��ҳ���7���֪ͨ������֪ͨ���⣩
                    if (noti.getType() == Notification::OVERDUE_FINE) {
                        return false; // ����֪ͨ������
                    }
                    return noti.getIsRead() && (currentDate - noti.getCreationDate()) > 7;
                }),
            notifications.end());
    }

    // ����Ƿ����κ�֪ͨ
    bool hasNotifications() const {
        return getUnreadNotificationCount() > 0;
    }
};

