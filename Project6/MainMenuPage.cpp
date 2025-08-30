// MainMenuPage.cpp - 对应的实现
#include"MainMenuPage.h"
#include"failmanage.h"
#include"Bookpage.h"
#include"BorrowStatisticsPage.h"
#include"ReaderInfoPage.h"
#include<iostream>
using namespace std;
MainMenuPage::MainMenuPage(vector<Book>& bs, vector<BorrowInfo>& bis, User* user,
    vector<Reader>& rdrs, ReservationManager& rm, NotificationManager& nm)
    : books(bs), borrowInfos(bis), currentUser(user), readers(rdrs),
    reservationManager(rm), notificationManager(nm) {
    std::cout << "MainMenuPage构造完成，读者数量: " << readers.size()
        << "，使用全局预约管理器和通知管理器" << endl;
}

bool MainMenuPage::display() const {
    std::cout << "欢迎, " << currentUser->getName() << endl;
    std::cout << "=== 主菜单 ===" << endl;
    std::cout << "1. 图书管理" << endl;
    std::cout << "2. 读者信息" << endl;
    std::cout << "3. 借阅统计" << endl;
    std::cout << "4. 修改密码" << endl;

    // 显示未读通知数量
    int unreadCount = currentUser->getUnreadNotificationCount();
    if (unreadCount > 0) {
        cout << "5. 消息中心 (" << unreadCount << " 条未读消息)\n";
    }
    else {
        cout << "5. 消息中心\n";
    }
    cout << "6. 退出登录\n";
    return 0;
}

void MainMenuPage::performAction() {
    while (true) {
        // 在每次显示菜单前刷新通知
        notificationManager.generateAllNotifications();

        display();
        int choice;
        cout << "请选择操作: ";
        if (validateInt(choice)) {
            switch (choice) {
            case 1: {
                cout << "进入图书管理页面，使用全局预约管理器..." << endl;
                cout << "当前读者数量: " << readers.size() << endl;

                std::vector<Reader*> readerPointers;
                convertToPointerVector(readers, readerPointers);

                cout << "创建BookPage，指针数量: " << readerPointers.size() << endl;
                // 传递全局预约管理器引用
                BookPage bookPage{ books, borrowInfos, readerPointers, currentUser, reservationManager };
                bookPage.performAction();

                cout << "退出图书管理页面，当前读者数量: " << readers.size() << endl;
                break;
            }
            case 2: {
                ReaderInfoPage readerInfoPage{ readers, currentUser };
                readerInfoPage.performAction();
                break;
            }
            case 3: {
                BorrowStatisticsPage borrowStatsPage{ borrowInfos, books, currentUser };
                borrowStatsPage.performAction();
                break;
            }
            case 4: {
                string newPassword;
                cout << "输入新密码: ";
                cin.ignore();
                getline(cin, newPassword);
                clearscreen();
                currentUser->changePassword(newPassword);
                cout << "密码修改成功!" << endl;
                break;
            }
            case 5: {
                cout << "进入消息中心..." << endl;

                // 使用增强的通知显示方法
                std::vector<Reader*> readerPointers;
                convertToPointerVector(readers, readerPointers);

                BookPage bookPage{ books, borrowInfos, readerPointers, currentUser, reservationManager };
                bookPage.showNotifications();
                break;
            }
            case 6:
                clearscreen();
                cout << "退出登录..." << endl;
                currentUser = nullptr;
                return;
            default:
                cout << "无效选择!" << endl;
            }
        }
    }
}