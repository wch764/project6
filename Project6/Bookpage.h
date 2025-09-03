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
#include"failmanager.h"
#include"reservemanager.h"
#include"bookdetailpage.h"

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
    void addBookReview(Book& book);
    void showCategoryBooks(BookCategory category) ;
    void showBooksByCategory();
    void searchBooksWithCategory() ;
    template<typename FieldGetter>
    vector<Book> exactSearchWithCategory(
        const string& query,
        FieldGetter fieldGetter,
        bool filterByCategory,
        BookCategory category
    ) const;
    template<typename FieldGetter>
    vector<Book> fuzzySearchWithCategory(
        const string& query,
        FieldGetter fieldGetter,
        bool filterByCategory,
        BookCategory category
    ) const;
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

    void searchbooks()
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

                displaySearchResults(results);
                }
            }

        }
    
    void displaySearchResults(const vector<Book>& results)  {
        if (results.empty()) {
            cout << "未找到匹配的书籍。\n";
            system("pause");
            return;
        }

        while (true) {
            clearscreen();
            cout << "找到 " << results.size() << " 本书：\n\n";

            // 显示带序号的搜索结果
            for (size_t i = 0; i < results.size(); ++i) {
                cout << "[" << (i + 1) << "] ";
                cout << "ISBN: " << results[i].getISBN() << "\n";
                cout << "    书名: " << results[i].getTitle() << "\n";
                cout << "    作者: " << results[i].getAuthor() << "\n";
                cout << "    可借数量: " << results[i].getAvailable() << "\n";
                cout << "    ----------------------------\n";
            }

            cout << "\n请选择要查看的书籍序号 (输入0退出)：";

            int choice;
            if (validateInt(choice)) {
                if (choice == 0) {
                    return; // 退出搜索结果页面
                }
                else if (choice >= 1 && choice <= static_cast<int>(results.size())) {
                    // 创建并显示书籍详细页面
                    auto borrowCallback = [this](const string& isbn) {
                        this->borrowBook(isbn);
                        };
                    BookDetailPage detailPage(results[choice - 1], borrowCallback);
                    detailPage.display();
                    // 查看详细信息后返回到搜索结果页面
                }
                else {
                    cout << "无效序号！请重新输入。\n";
                    system("pause");
                }
            }
            else {
                cout << "输入无效！请输入数字。\n";
                system("pause");
            }
        }
    }
    void showAllBooks() ;
    void borrowBook(const string isbn);
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

    void displayCategoryBooksWithSelection(const vector<Book>& bookList, const string& title);

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
    
       
};