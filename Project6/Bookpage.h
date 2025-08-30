#pragma once
#include<iostream>
#include<string>
#include"Page.h"
#include"Action.h"
#include<vector>
#include"Reader.h"
#include"LoginPage.h"
#include"clearscreen.h"
#include"Book.h"
#include"BorrowInfo.h"
#include <locale>
#include <codecvt>
#include <cctype>
#include <unordered_map>
#include <algorithm>
#include"failmanage.h"
#include"reservemanager.h"


// 图书页面类
class BookPage : public Page, public Action {
private:
    vector<Book>& books;
    vector<BorrowInfo>& borrowInfos;
    User* currentUser;
    ReservationManager& reservationManager;
    vector<Reader*>& users;

public:
    BookPage(vector<Book>& bs, vector<BorrowInfo>& bis, vector<Reader*>& us, User* user, ReservationManager& rm);
    bool display() const override final;
    void performAction() override final;
    void editbooks();
    void addbook();
    void deletebook();
    void modifybook();
    // 查找用户函数
    User* finduser(const string& username) {
        cout << "正在查找用户: " << username << endl;
        cout << "用户列表大小: " << users.size() << endl;

        for (size_t i = 0; i < users.size(); ++i) {
            if (users[i] != nullptr) {
                cout << "检查用户 " << i << ": " << users[i]->getUsername() << endl;

                // 使用字符串比较，不区分大小写
                string currentUsername = users[i]->getUsername();
                if (currentUsername == username) {
                    cout << "找到用户: " << currentUsername << endl;
                    return static_cast<User*>(users[i]);
                }
            }
            else {
                cout << "发现空指针用户在索引: " << i << endl;
            }
        }

        cout << "未找到用户: " << username << endl;
        cout << "系统中所有用户列表：" << endl;
        for (size_t i = 0; i < users.size(); ++i) {
            if (users[i] != nullptr) {
                cout << "  [" << i << "] " << users[i]->getUsername() << endl;
            }
        }

        return nullptr;
    }

    std::vector<std::string> tokenize(const std::string& str) const {
        std::vector<std::string> tokens;
        std::string currentToken;

        for (size_t i = 0; i < str.size(); ) {
            unsigned char c = static_cast<unsigned char>(str[i]);

            // 处理英文字母（ASCII 范围）
            if (isalpha(c) || c == '\'') {
                currentToken += tolower(c);
                i++;
            }
            else if (isdigit(c)) {
                if (!currentToken.empty()) {
                    tokens.push_back(currentToken);
                    currentToken.clear();
                }
                tokens.push_back(std::string(1, c)); // 当前数字作为独立token
                i++;
            }
            // 处理汉字（GBK/UTF-8 判断）
            else if (c >= 0x80) {
                if (!currentToken.empty()) {
                    tokens.push_back(currentToken);
                    currentToken.clear();
                }
                // UTF-8 汉字占 3 字节，GBK 占 2 字节
                size_t charLen = (c >= 0xE0) ? 3 : 2;  // 简单判断 UTF-8 汉字
                tokens.push_back(str.substr(i, charLen));
                i += charLen;
            }
            // 处理空格和标点符号
            else if (isspace(c) || ispunct(c)) {
                if (!currentToken.empty()) {
                    tokens.push_back(currentToken);
                    currentToken.clear();
                }
                i++;
            }
            else {
                i++;  // 其他情况（如控制字符）
            }
        }

        if (!currentToken.empty()) {
            tokens.push_back(currentToken);
        }

        return tokens;
    }

    // 计算公共字符数（英文单词 + 汉字单字）
    int countCommonTokens(const std::string& str1, const std::string& str2) const {
        auto tokens1 = tokenize(str1);
        auto tokens2 = tokenize(str2);

        std::unordered_map<std::string, int> freq1, freq2;

        // 统计 str1 的词频
        for (const auto& token : tokens1) {
            freq1[token]++;
        }

        // 统计 str2 的词频
        for (const auto& token : tokens2) {
            freq2[token]++;
        }

        // 计算公共字符数
        int commonCount = 0;
        for (const auto& entry : freq1) {
            const std::string& token = entry.first;
            int count = entry.second;
            if (freq2.count(token)) {

                commonCount += std::max(count, freq2[token]);//强调高频词
            }
        }

        return commonCount;
    }



    string removeSpaces(const string& str) const {
        string result;
        for (char c : str) {
            if (!isspace(static_cast<unsigned char>(c))) {
                result += c;
            }
        }
        return result;
    }
    bool match(const string& str1, const string& str2) const {
        // 移除空格后的字符串
        string s1 = removeSpaces(str1);
        string s2 = removeSpaces(str2);
        if (s1.size() != s2.size()) return false;

        return equal(s1.begin(), s1.end(), s2.begin(),
            [](char a, char b) {
                return tolower(static_cast<unsigned char>(a)) ==
                    tolower(static_cast<unsigned char>(b));
            });
    }
    template<typename FieldGetter>
    vector<Book> exactSearch(
        const string& query,
        FieldGetter fieldGetter//函数指针（指向book.getTitle()等）

    ) const {
        vector<Book> results;
        for (const Book& book : books) {
            string fieldValue = fieldGetter(book);
            if (match(query, fieldValue)) {
                results.push_back(book);
            }
        }


        // 按借阅量降序排序
        sort(results.begin(), results.end(),
            [](const Book& a, const Book& b) {
                return a.getBorrowed() > b.getBorrowed();
            });


        return results;
    }
    template<typename FieldGetter>
    vector<Book> fuzzySearch(
        const string& query,
        FieldGetter fieldGetter
    ) const {
        vector<pair<size_t, Book>> scoredBooks; // (LCS 分数, Book)

        for (const Book& book : books) {
            const string fieldValue = fieldGetter(book);
            size_t score = countCommonTokens(query, fieldValue); // 计算 LCS 分数
            if (score > 0) {
                scoredBooks.emplace_back(score, book);

            }
        }

        // 按分数降序排序
        sort(scoredBooks.begin(), scoredBooks.end(),
            [](const auto& a, const auto& b) {
                return a.first > b.first;
            });


        vector<Book> results;
        for (size_t i = 0; i < scoredBooks.size(); i++) {
            results.push_back(scoredBooks[i].second);

        }


        return results;
    }

    void searchbooks() const
    {
        cout << "请选择搜索方式：\n";
        cout << "1. 精确搜索（完全匹配）\n";
        cout << "2. 模糊搜索（相似度匹配）\n";
        cout << "请输入选项：";

        int searchType;


        if (validateInt(searchType))
        {
            if (searchType != 1 && searchType != 2) {
                cerr << "无效输入" << endl;
                return;
            }
            clearscreen();
            cout << "请选择搜索字段：\n";
            cout << "1. ISBN\n";
            cout << "2. 书名\n";
            cout << "3. 作者\n";
            cout << "4. 关键词\n";
            cout << "5. 简介\n";
            cout << "请输入选项：";
            int fieldChoice;


            if (validateInt(fieldChoice, "请输入选项："))
            {
                if (fieldChoice < 1 || fieldChoice > 5) {
                    cerr << "无效选项！" << endl;
                    return;
                }
                cout << "请输入搜索内容：";
                cin.ignore();
                string query;
                getline(cin, query);

                vector<Book> results;

                switch (fieldChoice) {
                case 1: // ISBN
                    if (searchType == 1) {
                        results = exactSearch(query, [](const Book& b) { return b.getISBN(); });
                    }
                    else {
                        results = fuzzySearch(query, [](const Book& b) { return b.getISBN(); });
                    }

                    break;
                case 2: // 书名
                    if (searchType == 1) {
                        results = exactSearch(query, [](const Book& b) { return b.getTitle(); });
                    }
                    else {
                        results = fuzzySearch(query, [](const Book& b) { return b.getTitle(); })
                            ;
                    }
                    break;
                case 3: // 作者
                    if (searchType == 1) {
                        results = exactSearch(query, [](const Book& b) { return b.getAuthor(); });
                    }
                    else {
                        results = fuzzySearch(query, [](const Book& b) { return b.getAuthor(); });
                    }
                    break;
                case 4: // 关键词
                    if (searchType == 1) {
                        results = exactSearch(query, [](const Book& b) {
                            string keywordsStr;
                            for (const auto& kw : b.getkeys()) {
                                keywordsStr += kw + " ";
                            }
                            return keywordsStr;
                            });
                    }
                    else {
                        results = fuzzySearch(query, [](const Book& b) {
                            string keywordsStr;
                            for (const auto& kw : b.getkeys()) {
                                keywordsStr += kw + " ";
                            }
                            return keywordsStr;
                            });
                    }
                    break;
                case 5: // 简介
                    if (searchType == 1) {
                        results = exactSearch(query, [](const Book& b) { return b.getintro(); });
                    }
                    else {
                        results = fuzzySearch(query, [](const Book& b) { return b.getintro(); });
                    }

                    break;
                default:
                    cerr << "无效选项！\n";
                    return;
                }

                // 显示搜索结果
                if (results.empty()) {
                    cout << "未找到匹配的书籍。\n";
                }
                else {
                    cout << "找到 " << results.size() << " 本书：\n";
                    for (const Book& book : results) {
                        cout << "ISBN: " << book.getISBN() << "\n";
                        cout << "书名: " << book.getTitle() << "\n";
                        cout << "作者: " << book.getAuthor() << "\n";
                        cout << "可借数量: " << book.getAvailable() << "\n";
                        cout << "----------------------------\n";
                    }
                }
            }

        }
    }

    void showAllBooks() const;
    void borrowBook();
    void renewBook();
    void returnBook();
    // 辅助函数：通过ISBN查找书名
    string findBookTitle(const string& isbn) const {
        for (const auto& book : books) {
            if (book.getISBN() == isbn) {
                return book.getTitle();
            }
        }
        return "未知图书";
    }
    // 修复预约创建流程，确保正确添加到ReservationManager
    void reserveBook(const string isbn) {
       

        // 查找图书
        auto bookIt = find_if(books.begin(), books.end(),
            [&isbn](const Book& b) { return b.getISBN() == isbn; });

        if (bookIt == books.end()) {
            cout << "图书不存在！" << endl;
            return;
        }

        Book& book = *bookIt;
        cout << "找到图书: " << book.getTitle() << endl;

        // 检查是否可预约
        if (book.isAvailable()) {
            cout << "该图书当前可借，无需预约。请直接借阅。" << endl;
            return;
        }

        // 检查用户是否已经预约过此书
        for (const auto& info : borrowInfos) {
            if (info.getBookISBN() == isbn &&
                info.getReaderUsername() == currentUser->getUsername() &&
                info.is_Reservation()) {
                cout << "您已经预约过此书！" << endl;
                return;
            }
        }

        cout << "开始创建预约记录..." << endl;

        // 获取当前队列大小
        int queueSize = reservationManager.getQueueSize(isbn);
        cout << "当前预约队列大小: " << queueSize << endl;

        // 创建预约记录
        BorrowInfo newReservation(currentUser->getUsername(), isbn);
        Date expectedDate = calculateAvailableDate(isbn);
        newReservation.setAsReservation(queueSize + 1, expectedDate);

        cout << "创建预约记录完成:" << endl;
        cout << "  - 预约序号: " << newReservation.getReservationSequence() << endl;
        cout << "  - 是否预约记录: " << (newReservation.is_Reservation() ? "是" : "否") << endl;

        // 添加到borrowInfos并记录索引
        size_t newIndex = borrowInfos.size();
        borrowInfos.push_back(newReservation);
        cout << "已添加到borrowInfos，索引: " << newIndex << endl;

        // 使用索引添加到预约管理器
        if (reservationManager.addReservation(newIndex)) {
            cout << "预约成功！您的排队序号: "
                << newReservation.getReservationSequence() << endl;
            cout << "预计可借日期: "
                << newReservation.getExpectedAvailableDate().toString() << endl;
        }
        else {
            cout << "预约失败，从borrowInfos中移除记录" << endl;
            borrowInfos.pop_back();
        }
    }

   

    Date calculateAvailableDate(const string& isbn) {
        // 实现查找最早可借日期的逻辑
        // 示例：找到该书最早到期的借阅记录
        Date earliest(2100, 1, 1);
        for (const auto& info : borrowInfos) {
            if (info.getBookISBN() == isbn && !info.isReturned()) {
                if (info.getDueDate() < earliest) {
                    earliest = info.getDueDate();
                }
            }
        }
        earliest.addDays(1); // 到期日次日可借
        return earliest;
    
    }
    void showNotifications() {
        cout << "=== 消息中心 ===" << endl;

        // 获取所有通知
        auto allNotifications = currentUser->getAllNotifications();

        if (allNotifications.empty()) {
            cout << "没有新通知\n";
            return;
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
       
};