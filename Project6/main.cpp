#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <ctime>
#include <iomanip>
#include <algorithm>
#include <limits>
#include <cstdio>
#include <filesystem> // C++17

#include "Serializable.h"
#include "Action.h"
#include "Page.h"
#include "Date.h"
#include "Book.h"
#include "Reader.h"
#include "BorrowInfo.h"
#include "LoginPage.h"
#include "MainMenuPage.h"
#include "Admin.h"
#include "ReserveManager.h"
#include "Notificationmanager.h"
#include "SystemLogger.h"
#include "RankingPage.h"
#include"failmanager.h"

using namespace std;



// 改进的数据加载函数，添加详细的错误处理和日志记录
template <typename T>
void loadDataFromFile(const string& filename, vector<T>& items) {
    auto logger = SystemLogger::getInstance();

    ifstream file(filename);
    if (!file.is_open()) {
        string error = "无法打开文件: " + filename;
        logger->logDataLoadError(filename, error);
        cout << error << " (如果是首次运行，这是正常的)" << endl;
        return;
    }

    logger->logInfo("DATA", "开始加载文件: " + filename);
    T item;
    int count = 0;
    int errorCount = 0;

    while (item.loadFromStream(file)) {
        // 验证加载的数据
        bool isValid = false;

        if constexpr (is_same_v<T, Reader> || is_same_v<T, Admin>) {
            isValid = !item.getUsername().empty();
        }
        else if constexpr (is_same_v<T, Book>) {
            isValid = !item.getISBN().empty();
        }
        else if constexpr (is_same_v<T, BorrowInfo>) {
            isValid = !item.getBookISBN().empty();
        }

        if (isValid) {
            items.push_back(item);
            count++;
        }
        else {
            errorCount++;
        }
    }

    file.close();

    if (errorCount > 0) {
        logger->logWarning("DATA", filename + " 中有 " + to_string(errorCount) + " 条无效记录被跳过");
    }

    logger->logDataLoad(filename, count);
    cout << "从 " << filename << " 成功加载了 " << count << " 条记录";
    if (errorCount > 0) {
        cout << "，跳过了 " << errorCount << " 条无效记录";
    }
    cout << endl;
}

// 改进的数据保存函数，添加备份功能
template <typename T>
void saveDataToFile(const string& filename, const vector<T>& items) {
    auto logger = SystemLogger::getInstance();

    // 创建备份（如果原文件存在）
    if (filesystem::exists(filename)) {
        string backupName = filename + ".backup";
        try {
            filesystem::copy_file(filename, backupName,
                filesystem::copy_options::overwrite_existing);
            logger->logInfo("DATA", "创建备份文件: " + backupName);
        }
        catch (const exception& e) {
            logger->logWarning("DATA", "备份文件创建失败: " + string(e.what()));
        }
    }

    ofstream file(filename, ios::trunc);
    if (!file.is_open()) {
        string error = "无法打开文件进行保存: " + filename;
        logger->logDataSaveError(filename, error);
        cout << error << endl;
        return;
    }

    int count = 0;
    int errorCount = 0;

    for (const auto& item : items) {
        try {
            item.saveToStream(file);
            count++;
        }
        catch (const exception& e) {
            errorCount++;
            logger->logError("DATA", "保存记录时出错: " + string(e.what()));
        }
    }

    file.close();

    if (errorCount > 0) {
        logger->logWarning("DATA", "保存 " + filename + " 时有 " + to_string(errorCount) + " 条记录失败");
    }

    logger->logDataSave(filename, count);
    cout << "成功保存了 " << count << " 条记录到 " << filename;
    if (errorCount > 0) {
        cout << "，有 " << errorCount << " 条记录保存失败";
    }
    cout << endl;
}

// 重建预约队列的改进版本
void rebuildReservationQueue(ReservationManager& manager, const vector<BorrowInfo>& borrowInfos) {
    auto logger = SystemLogger::getInstance();
    logger->logInfo("RESERVATION", "开始重建预约队列");

    int rebuildCount = 0;
    for (size_t i = 0; i < borrowInfos.size(); ++i) {
        const BorrowInfo& info = borrowInfos[i];
        if (info.is_Reservation() && !info.isReturned()) {
            if (manager.addReservation(i)) {
                rebuildCount++;
            }
        }
    }

    logger->logInfo("RESERVATION", "预约队列重建完成，重建了 " + to_string(rebuildCount) + " 个预约");
    cout << "预约队列重建完成，重建了 " << rebuildCount << " 个预约" << endl;
}

// 初始化示例数据的改进版本
void initializeSampleData(vector<Book>& books, vector<Reader>& readers, vector<Admin>& admins) {
    auto logger = SystemLogger::getInstance();

    if (books.empty()) {
        logger->logInfo("INIT", "初始化示例图书数据");
        cout << "图书信息为空，添加示例图书..." << endl;

        // 添加不同分类的示例图书
        books.emplace_back("9787111636667", "C++ Primer", "Stanley Lippman",
            "编程 C++ 计算机", "经典C++教程", 5,
            BookCategory::COMPUTER_SCIENCE_INFORMATION);

        books.emplace_back("9787302517969", "深入理解计算机系统", "Randal Bryant",
            "计算机 系统", "计算机系统经典教材", 3,
            BookCategory::COMPUTER_SCIENCE_INFORMATION);

        books.emplace_back("9787115419663", "算法导论", "Thomas Cormen",
            "算法 数据结构", "算法设计经典教材", 4,
            BookCategory::NATURAL_SCIENCES_MATHEMATICS);

        books.emplace_back("9787020002207", "红楼梦", "曹雪芹",
            "古典文学 名著", "中国古典文学四大名著之一", 10,
            BookCategory::LITERATURE_RHETORIC);

        books.emplace_back("9787100018616", "哲学通史", "冯友兰",
            "哲学 思想史", "中国哲学通史", 6,
            BookCategory::PHILOSOPHY_PSYCHOLOGY);

        saveDataToFile("books.dat", books);
        logger->logInfo("INIT", "示例图书数据初始化完成");
    }

    if (admins.empty()) {
        logger->logInfo("INIT", "初始化默认管理员账号");
        cout << "管理员信息为空，创建默认管理员账号..." << endl;
        admins.emplace_back("admin", "123456", "系统管理员");
        saveDataToFile("admins.dat", admins);
        logger->logInfo("INIT", "默认管理员账号创建完成");
    }
}

// 系统健康检查
void performSystemHealthCheck(const vector<Reader>& readers, const vector<Book>& books,
    const vector<BorrowInfo>& borrowInfos) {
    auto logger = SystemLogger::getInstance();
    logger->logInfo("HEALTH", "开始系统健康检查");

    int issues = 0;

    // 检查数据一致性
    for (const auto& borrowInfo : borrowInfos) {
        if (!borrowInfo.is_Reservation() && !borrowInfo.isReturned()) {
            // 检查读者是否存在
            auto readerIt = find_if(readers.begin(), readers.end(),
                [&](const Reader& r) { return r.getUsername() == borrowInfo.getReaderUsername(); });

            if (readerIt == readers.end()) {
                logger->logWarning("HEALTH", "发现孤立的借阅记录，用户不存在: " + borrowInfo.getReaderUsername());
                issues++;
            }

            // 检查图书是否存在
            auto bookIt = find_if(books.begin(), books.end(),
                [&](const Book& b) { return b.getISBN() == borrowInfo.getBookISBN(); });

            if (bookIt == books.end()) {
                logger->logWarning("HEALTH", "发现孤立的借阅记录，图书不存在: " + borrowInfo.getBookISBN());
                issues++;
            }
        }
    }

    if (issues == 0) {
        logger->logInfo("HEALTH", "系统健康检查完成，未发现问题");
    }
    else {
        logger->logWarning("HEALTH", "系统健康检查完成，发现 " + to_string(issues) + " 个问题");
    }
}

int main() {
    // 初始化系统日志
    auto logger = SystemLogger::getInstance();
    logger->logSystemStart();

    try {
        vector<Reader> readers;
        vector<Admin> admins;
        vector<Book> books;
        vector<BorrowInfo> borrowInfos;

        cout << "=== 图书管理系统 v2.0 启动 ===" << endl;
        logger->logInfo("SYSTEM", "开始加载数据");

        // 加载数据
        loadDataFromFile("readers.dat", readers);
        loadDataFromFile("books.dat", books);
        loadDataFromFile("borrowinfo.dat", borrowInfos);
        loadDataFromFile("admins.dat", admins);

        cout << "数据加载完成 - 读者: " << readers.size()
            << ", 管理员: " << admins.size()
            << ", 图书: " << books.size()
            << ", 借阅记录: " << borrowInfos.size() << endl;

        // 初始化示例数据
        initializeSampleData(books, readers, admins);

        // 系统健康检查
        performSystemHealthCheck(readers, books, borrowInfos);

        // 创建指针向量用于通知管理
        vector<Reader*> readerPointers;
        for (auto& reader : readers) {
            readerPointers.push_back(&reader);
        }

        // 创建全局组件
        ReservationManager* globalReservationManager = new ReservationManager(borrowInfos);
        NotificationManager* notificationManager =
            new NotificationManager(borrowInfos, books, readerPointers);

        // 重建预约队列
        rebuildReservationQueue(*globalReservationManager, borrowInfos);

        // 生成通知
        logger->logInfo("NOTIFICATION", "生成系统通知");
        notificationManager->generateAllNotifications();

        auto stats = notificationManager->getNotificationStats();
        cout << "通知系统初始化完成，当前有 " << stats.unreadCount << " 条未读通知" << endl;

        // 检查读者文件
        if (readers.empty()) {
            cout << "读者信息为空，请先注册用户！" << endl;
            logger->logWarning("SYSTEM", "系统中无读者数据");
            delete globalReservationManager;
            delete notificationManager;
            return 0;
        }

        // 创建登录页面
        LoginPage loginPage(readers, admins);

        // 主程序循环
        while (true) {
            try {
                // 在每次显示登录界面前更新通知
                notificationManager->generateAllNotifications();

                clearscreen();
                cout << "=== 图书管理系统 v2.0 ===" << endl;
                loginPage.display();
                loginPage.performAction();
                User* currentUser = loginPage.getCurrentUser();

                if (!currentUser) {
                    break;  // 用户选择退出
                }

                logger->logUserLogin(currentUser->getUsername(), currentUser->get_isadmin());

                cout << "用户登录成功: " << currentUser->getUsername();
                int unreadCount = currentUser->getUnreadNotificationCount();
                if (unreadCount > 0) {
                    cout << " (有 " << unreadCount << " 条未读通知)";
                }
                cout << endl;
           



                // 创建增强的主菜单
                MainMenuPage MainMenu(books, borrowInfos, currentUser,
                    readers, *globalReservationManager,
                    *notificationManager);
                MainMenu.performAction();

            }
            catch (const exception& e) {
                logger->logSystemError("MAIN", "主程序循环异常: " + string(e.what()));
                cout << "系统发生错误: " << e.what() << endl;
                cout << "按回车键继续..." << endl;
                cin.ignore();
                cin.get();
            }
        }

        cout << "\n=== 开始保存数据 ===" << endl;
        logger->logInfo("SYSTEM", "开始保存系统数据");

        cout << "保存前数据统计 - 读者: " << readers.size()
            << ", 图书: " << books.size()
            << ", 借阅记录: " << borrowInfos.size() << endl;

        // 保存数据
        saveDataToFile("readers.dat", readers);
        saveDataToFile("books.dat", books);
        saveDataToFile("borrowinfo.dat", borrowInfos);
        saveDataToFile("admins.dat", admins);

        logger->logInfo("SYSTEM", "所有数据保存完成");

        // 清理资源
        delete globalReservationManager;
        delete notificationManager;

        logger->logSystemShutdown();
        cout << "系统已安全退出，数据已保存。" << endl;

    }
    catch (const exception& e) {
        logger->logCritical("SYSTEM", "系统严重错误: " + string(e.what()));
        cout << "系统发生严重错误: " << e.what() << endl;
        cout << "错误已记录到日志文件中。" << endl;
        return 1;
    }

    return 0;
}