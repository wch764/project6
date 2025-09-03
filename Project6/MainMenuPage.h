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
       

        int unreadCount = currentUser->getUnreadNotificationCount();
        if (unreadCount > 0) {
            cout << "5. 消息中心 (" << unreadCount << " 条未读消息)" << endl;
        }
        else {
            cout << "5. 消息中心" << endl;
        }
        cout << "6. 系统信息" << endl;
        cout << "7. 退出登录" << endl;
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
                    /*vector<Reader*> readerPtrs;
                    for (auto& reader : readers) {
                        readerPtrs.push_back(&reader);
                    }*/
                    std::vector<Reader*> readerPointers;
                    convertToPointerVector(readers, readerPointers);
                    BookPage bookPage{ books, borrowInfos, readerPointers, currentUser, reservationManager };
                    RankingPage rankingPage(readers, books, currentUser, [&bookPage](const string& isbn) {
                        bookPage.borrowBook(isbn);  
                        });
                    rankingPage.performAction();
                    break;
                }
                
                case 5: {
                    // 消息中心
                    logger->logInfo("MENU", currentUser->getUsername() + " 查看消息中心");
                    showNotifications();
                    break;
                }
                case 6: {
                    // 系统信息
                    showSystemInfo();
                    break;
                }
                case 7:
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
        cout << "=== 消息中心 ===" << endl;

        // 获取所有通知
        auto allNotifications = currentUser->getAllNotifications();

        if (allNotifications.empty()) {
            cout << "没有新通知\n";
			system("pause");
        }

        // 按类型分组通知
        vector<Notification> reservationNotifications;
        vector<Notification> dueSoonNotifications;
        vector<Notification> fineNotifications;
        vector<Notification> otherNotifications;

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
            default:
                otherNotifications.push_back(noti);
                break;
            }
        }

        // 显示预约通知
        if (!reservationNotifications.empty()) {
            cout << "\n【预约通知】" << endl;
            vector<string> availableBooks;

            for (const auto& noti : reservationNotifications) {
                cout << "◆ " << noti.getMessage();
                if (!noti.getIsRead()) cout << " [新]";
                cout << endl;

                if (noti.getType() == Notification::RESERVATION_AVAILABLE && !noti.getIsRead()) {
                    availableBooks.push_back(noti.getBookIsbn());
                }

            }

            // 处理预约确认
            if (!availableBooks.empty()) {
                cout << "\n是否要借阅这些预约图书？(y/n): ";
                char choice;
                cin >> choice;

                if (choice == 'y' || choice == 'Y') {
                    cout << "开始处理预约借阅..." << endl;
                    for (const auto& isbn : availableBooks) {
                        bool result = confirmborrow(isbn, currentUser->getUsername());
                        if (result) {
                            cout << "ISBN " << isbn << " 借阅确认成功" << endl;
                        }
                        else {
                            cout << "ISBN " << isbn << " 借阅确认失败" << endl;
                        }
                    }

                    // 清除已处理的预约通知
                    currentUser->getAvailableReservations();
                }
            }
        }

        // 显示到期提醒
        if (!dueSoonNotifications.empty()) {
            cout << "\n【到期提醒】" << endl;
            for (const auto& noti : dueSoonNotifications) {
                cout << "◆ " << noti.getMessage();
                if (!noti.getIsRead()) cout << " [新]";
                cout << endl;
            }

            // 标记到期通知为已读（用户进入通知页面后）
            currentUser->markDueNotificationsAsRead();
            cout << "到期提醒已标记为已读" << endl;
        }

        // 显示罚款通知（永远显示为未读）
        if (!fineNotifications.empty()) {
            cout << "\n【罚款通知】" << endl;
            double totalFine = 0.0;

            for (const auto& noti : fineNotifications) {
                cout << "◆ " << noti.getMessage() << " [未处理]" << endl;
                totalFine += noti.getFineAmount();
            }

            cout << "您当前总计未交罚款: " << totalFine << " 元" << endl;
            cout << "请尽快缴纳罚款，否则可能影响借阅权限！" << endl;
        }

        // 显示其他通知
        if (!otherNotifications.empty()) {
            cout << "\n【其他通知】" << endl;
            for (const auto& noti : otherNotifications) {
                cout << "◆ " << noti.getMessage();
                if (!noti.getIsRead()) cout << " [新]";
                cout << endl;
            }
        }

        cout << "\n通知查看完毕。" << endl;
        system("pause");
    }
    bool confirmborrow(const string& isbn, const string& username) {
        // 添加这行作为第一行，确保函数被调用
        cout << "*** CONFIRMBORROW 函数被调用 *** ISBN=" << isbn << " 用户=" << username << endl;

        cout << "=== 确认借阅流程开始 ===" << endl;
        cout << "ISBN: " << isbn << ", 用户: " << username << endl;

        // 1. 查找图书
        auto bookIt = find_if(books.begin(), books.end(),
            [&isbn](const Book& b) { return b.getISBN() == isbn; });

        if (bookIt == books.end()) {
            cout << "错误：图书不存在" << endl;
            return false;
        }

        Book& book = *bookIt;
        cout << "找到图书: " << book.getTitle() << ", 可借数量: " << book.getAvailable() << endl;

        // 2. 检查图书是否可借
        if (!book.isAvailable()) {
            cout << "该书当前不可借，可用数量: " << book.getAvailable() << endl;
            return false;
        }

        // 3. 查找用户的预约记录
        cout << "开始查找预约记录..." << endl;
        auto reservationIt = find_if(borrowInfos.begin(), borrowInfos.end(),
            [&](BorrowInfo& bi) {
                cout << "检查记录: 用户=" << bi.getReaderUsername()
                    << " ISBN=" << bi.getBookISBN()
                    << " 预约=" << (bi.is_Reservation() ? "是" : "否")
                    << " 已还=" << (bi.isReturned() ? "是" : "否") << endl;

                return bi.getBookISBN() == isbn &&
                    bi.getReaderUsername() == username &&
                    bi.is_Reservation() &&
                    !bi.isReturned();
            });

        if (reservationIt == borrowInfos.end()) {
            cout << "错误：未找到该用户的有效预约记录" << endl;
            cout << "borrowInfos总数: " << borrowInfos.size() << endl;
            return false;
        }

        cout << "*** 找到预约记录，开始转换... ***" << endl;

        // 4. 转换预约记录为借阅记录
        BorrowInfo& reservation = *reservationIt;

        // 设置新的借阅日期和应还日期
        Date currentDate = Date::currentdate();
        reservation.setBorrowDate(currentDate);
        reservation.setDueDate(BorrowInfo::calculateDueDate(currentDate));

        // 转换为正常借阅记录
        reservation.converttoborrow();
        currentUser->addBorrowedCount();
        bookIt->addTotalBorrowCount();

        cout << "预约记录已转换为借阅记录" << endl;
        cout << "  - 借阅日期: " << reservation.getBorrowDate().toString() << endl;
        cout << "  - 应还日期: " << reservation.getDueDate().toString() << endl;

        // 5. 更新图书状态
        if (!book.borrowBook()) {
            cout << "错误：图书借阅状态更新失败" << endl;
            return false;
        }

        // 6. 从预约队列中移除
        if (reservationManager.fulfillReservation(isbn)) {
            cout << "已从预约队列中移除" << endl;
        }
        else {
            cout << "警告：从预约队列移除失败" << endl;
        }

        cout << "*** CONFIRMBORROW 成功完成 *** 应还日期：" << reservation.getDueDate().toString() << endl;
        return true;
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

