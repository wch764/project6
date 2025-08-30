#pragma once
#include"Serializable.h"
#include<iostream>
#include"Date.h"
#include<fstream>
#include<string>
#include"notification.h"
using namespace std;
// 读者类
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
        std::cout << "销毁User: " << username << std::endl;
    }
    //读取
    bool loadFromStream(istream& file) override;


    void saveToStream(ostream& file) const override;

    // 保持原有接口但修改实现
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
        notifications.clear(); // 读取后清空
        return result;
    }

   
    // 添加通知
    void addNotification(const Notification& noti) {
        // 检查是否已存在相同的通知（避免重复）
        auto it = std::find_if(notifications.begin(), notifications.end(),
            [&noti](const Notification& existing) {
                return existing.getType() == noti.getType() &&
                    existing.getBookIsbn() == noti.getBookIsbn();
            });

        if (it != notifications.end()) {
            // 更新现有通知
            *it = noti;
        }
        else {
            // 添加新通知
            notifications.push_back(noti);
        }
    }

    // 获取所有通知（不清空，用于显示）
    std::vector<Notification> getAllNotifications() const {
        std::vector<Notification> result;
        for (const auto& noti : notifications) {
            if (noti.shouldDisplay()) {
                result.push_back(noti);
            }
        }
        return result;
    }

    // 获取未读通知数量
    int getUnreadNotificationCount() const {
        int count = 0;
        for (const auto& noti : notifications) {
            if (!noti.getIsRead() && noti.shouldDisplay()) {
                count++;
            }
        }
        return count;
    }

    // 标记到期通知为已读
    void markDueNotificationsAsRead() {
        for (auto& noti : notifications) {
            if (noti.getType() == Notification::DUE_SOON) {
                noti.markAsRead();
            }
        }
    }

    // 获取可借阅的预约通知
    std::vector<Notification> getAvailableReservations() {
        std::vector<Notification> result;
        for (auto& noti : notifications) {
            if (noti.getType() == Notification::RESERVATION_AVAILABLE && !noti.getIsRead()) {
                result.push_back(noti);
                noti.markAsRead(); // 预约通知读取后标记为已读
            }
        }
        return result;
    }

    // 清理过期通知
    void cleanupNotifications() {
        auto currentDate = Date::currentdate();
        notifications.erase(
            std::remove_if(notifications.begin(), notifications.end(),
                [currentDate](const Notification& noti) {
                    // 移除已读且超过7天的通知（罚款通知除外）
                    if (noti.getType() == Notification::OVERDUE_FINE) {
                        return false; // 罚款通知不清理
                    }
                    return noti.getIsRead() && (currentDate - noti.getCreationDate()) > 7;
                }),
            notifications.end());
    }

    // 检查是否有任何通知
    bool hasNotifications() const {
        return getUnreadNotificationCount() > 0;
    }
};

