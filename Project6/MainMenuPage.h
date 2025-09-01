#pragma once
#include<iostream>
#include<string>
#include"Page.h"
#include"Action.h"
#include<vector>
#include"Reader.h"
#include"Book.h"
#include"BorrowInfo.h"
#include"User.h"
#include"reservemanager.h"
#include"Notificationmanager.h"
#include"Bookpage.h"
#include"ReaderInfoPage.h"
#include"BorrowStatisticsPage.h"
#include"RankingPage.h"
using namespace std;
// 创建增强的主菜单页面
class MainMenuPage : public Page, public Action {
private:
    vector<Book>& books;
    vector<BorrowInfo>& borrowInfos;
    User* currentUser;
    vector<Reader>& readers;
    ReservationManager& reservationManager;
    NotificationManager& notificationManager;
    SystemLogger* logger;

public:
    MainMenuPage(vector<Book>& bs, vector<BorrowInfo>& bis, User* user,
        vector<Reader>& rdrs, ReservationManager& rm, NotificationManager& nm)
        : books(bs), borrowInfos(bis), currentUser(user), readers(rdrs),
        reservationManager(rm), notificationManager(nm) {
        logger = SystemLogger::getInstance();
    }

    bool display() const override {
        clearscreen();
        cout << "欢迎, " << currentUser->getName() << endl;
        cout << "=== 主菜单 ===" << endl;
        cout << "1. 图书管理" << endl;
        cout << "2. 读者管理" << endl;
        cout << "3. 借阅统计" << endl;
        cout << "4. 排行榜" << endl;
        cout << "5. 修改密码" << endl;

        int unreadCount = currentUser->getUnreadNotificationCount();
        if (unreadCount > 0) {
            cout << "6. 消息中心 (" << unreadCount << " 条未读消息)" << endl;
        }
        else {
            cout << "6. 消息中心" << endl;
        }
        cout << "7. 系统信息" << endl;
        cout << "8. 退出登录" << endl;
        return false;
    }

    void performAction() override {
        while (true) {
            notificationManager.generateAllNotifications();
            display();

            int choice;
            cout << "请选择操作: ";
            
            if (validateInt(choice)) {
                switch (choice) {
                case 1: {
                   
                    // 图书管理功能
                    logger->logInfo("MENU", currentUser->getUsername() + " 进入图书管理");
                    std::vector<Reader*> readerPointers;
                    convertToPointerVector(readers, readerPointers);
                    BookPage bookPage{ books, borrowInfos, readerPointers, currentUser, reservationManager };
                    bookPage.performAction();
                    break;
                }
                case 2: {
                    // 读者管理功能 
                    logger->logInfo("MENU", currentUser->getUsername() + " 进入读者管理");
                    ReaderInfoPage readerInfoPage{ readers, currentUser };
                    readerInfoPage.performAction();
                    break;
                }
                case 3: {
                    // 借阅统计功能
                    logger->logInfo("MENU", currentUser->getUsername() + " 查看借阅统计");
                    BorrowStatisticsPage borrowStatsPage{ borrowInfos, books, currentUser };
                    borrowStatsPage.performAction();
                    break;
                }
                case 4: {
                    // 排行榜功能
                    logger->logInfo("MENU", currentUser->getUsername() + " 查看排行榜");
                    vector<Reader*> readerPtrs;
                    for (auto& reader : readers) {
                        readerPtrs.push_back(&reader);
                    }
                    RankingPage rankingPage(readers, books, currentUser);
                    rankingPage.performAction();
                    break;
                }
                case 5: {
                    // 修改密码
                    string newPassword;
                    cout << "输入新密码: ";
                    cin.ignore();
                    getline(cin, newPassword);
                    currentUser->changePassword(newPassword);
                    logger->logInfo("AUTH", currentUser->getUsername() + " 修改密码");
                    cout << "密码修改成功!" << endl;
                    break;
                }
                case 6: {
                    // 消息中心
                    logger->logInfo("MENU", currentUser->getUsername() + " 查看消息中心");
                    showNotifications();
                    break;
                }
                case 7: {
                    // 系统信息
                    showSystemInfo();
                    break;
                }
                case 8:
                    logger->logUserLogout(currentUser->getUsername());
                    cout << "退出登录..." << endl;
                    currentUser = nullptr;
                    return;
                default:
                    cout << "无效选择!" << endl;
                    logger->logInputError("MENU", "1-8", to_string(choice));
                }
            }
        }
    }

private:
    void showNotifications() {
        clearscreen();
        cout << "=== 消息中心 ===" << endl;

        auto allNotifications = currentUser->getAllNotifications();

        if (allNotifications.empty()) {
            cout << "没有新通知\n";
            return;
        }

        // 按类型分组显示通知
        vector<Notification> reservationNotifications;
        vector<Notification> dueSoonNotifications;
        vector<Notification> fineNotifications;

        for (const auto& noti : allNotifications) {
            switch (noti.getType()) {
            case Notification::RESERVATION_AVAILABLE:
            case Notification::RESERVATION_EXPIRED:
                reservationNotifications.push_back(noti);
                break;
            case Notification::DUE_SOON:
                dueSoonNotifications.push_back(noti);
                break;
            case Notification::OVERDUE_FINE:
                fineNotifications.push_back(noti);
                break;
            }
        }

        // 显示各类通知
        if (!reservationNotifications.empty()) {
            cout << "\n【预约通知】" << endl;
            for (const auto& noti : reservationNotifications) {
                cout << "◆ " << noti.getMessage();
                if (!noti.getIsRead()) cout << " [新]";
                cout << endl;
            }
        }

        if (!dueSoonNotifications.empty()) {
            cout << "\n【到期提醒】" << endl;
            for (const auto& noti : dueSoonNotifications) {
                cout << "◆ " << noti.getMessage();
                if (!noti.getIsRead()) cout << " [新]";
                cout << endl;
            }
            currentUser->markDueNotificationsAsRead();
        }

        if (!fineNotifications.empty()) {
            cout << "\n【罚款通知】" << endl;
            double totalFine = 0.0;
            for (const auto& noti : fineNotifications) {
                cout << "◆ " << noti.getMessage() << " [未处理]" << endl;
                totalFine += noti.getFineAmount();
            }
            cout << "您当前总计未交罚款: " << totalFine << " 元" << endl;
        }

        cout << "\n按回车键继续...";
        cin.ignore();
        cin.get();
    }

    void showSystemInfo() {
        clearscreen();
        cout << "=== 系统信息 ===" << endl;
        cout << "系统版本: 图书管理系统 v2.0" << endl;
        cout << "当前用户: " << currentUser->getName()
            << " (" << currentUser->getUsername() << ")" << endl;
        cout << "用户类型: " << (currentUser->get_isadmin() ? "管理员" : "普通用户") << endl;

        // 显示系统统计
        cout << "\n=== 系统统计 ===" << endl;
        cout << "总用户数: " << readers.size() << endl;
        cout << "总图书数: " << books.size() << endl;
        cout << "总借阅记录: " << borrowInfos.size() << endl;

        // 统计当前借阅
        int currentBorrows = 0;
        int reservations = 0;
        for (const auto& info : borrowInfos) {
            if (!info.isReturned()) {
                if (info.is_Reservation()) {
                    reservations++;
                }
                else {
                    currentBorrows++;
                }
            }
        }
        cout << "当前在借: " << currentBorrows << endl;
        cout << "当前预约: " << reservations << endl;

        // 显示通知统计
        auto stats = notificationManager.getNotificationStats();
        cout << "\n=== 通知统计 ===" << endl;
        cout << "系统总通知数: " << stats.totalNotifications << endl;
        cout << "未读通知数: " << stats.unreadCount << endl;
        cout << "到期提醒: " << stats.dueSoonCount << endl;
        cout << "罚款通知: " << stats.fineCount << endl;
        cout << "预约通知: " << stats.reservationCount << endl;

        cout << "\n日志文件: " << logger->getLogFileName() << endl;
        cout << "当前时间: " << Date::currentdate().toString() << endl;

        cout << "\n按回车键继续...";
        cin.ignore();
        cin.get();
    }
    static void convertToPointerVector(vector<Reader>& readers, vector<Reader*>& pointers) {
        pointers.clear();
        pointers.reserve(readers.size());
        cout << "转换指针向量，源数据大小: " << readers.size() << endl;
        for (size_t i = 0; i < readers.size(); ++i) {
            pointers.push_back(&readers[i]);
            cout << "添加指针 " << i << ": " << readers[i].getUsername() << endl;
        }
        cout << "指针向量转换完成，大小: " << pointers.size() << endl;
    }
};

