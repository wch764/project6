#pragma once
#include "Date.h"
#include "Serializable.h"
#include <string>
#include <algorithm>
#include <fstream>
#include <vector>
#include <iostream>
#include <sstream>
#include <numeric>
#include<iomanip>

using namespace std;

// 杜威十进制分类法十大基本大类
enum class BookCategory {
    COMPUTER_SCIENCE_INFORMATION = 0,    // 000-099 计算机科学、知识学、总论
    PHILOSOPHY_PSYCHOLOGY = 1,           // 100-199 哲学、心理学
    RELIGION = 2,                        // 200-299 宗教
    SOCIAL_SCIENCES = 3,                 // 300-399 社会科学
    LANGUAGE = 4,                        // 400-499 语言
    NATURAL_SCIENCES_MATHEMATICS = 5,    // 500-599 自然科学、数学
    TECHNOLOGY_APPLIED_SCIENCES = 6,     // 600-699 技术（应用科学）
    THE_ARTS_RECREATION = 7,            // 700-799 艺术、娱乐、体育
    LITERATURE_RHETORIC = 8,             // 800-899 文学
    HISTORY_GEOGRAPHY = 9                // 900-999 历史、传记、地理
};

// 评论结构体
struct Review {
    string readerUsername;
    int rating;           // 1-5星评分
    string comment;       // 评论内容
    Date reviewDate;      // 评论日期

    Review(const string& username = "", int r = 0, const string& c = "")
        : readerUsername(username), rating(r), comment(c), reviewDate(Date::currentdate()) {
    }
};

class Book : public Serializable {
private:
    string ISBN;
    string title;
    string author;
    vector<string> keywords;
    string introduction;
    int quantity;
    int borrowed;
    int totalBorrowCount;  
    int m_reserved = 0;
    string reservedFor;
    Date reserveExpiry;

    // 新增属性
    BookCategory category;              // 分类
              // 总借阅数
    vector<Review> reviews;            // 评论列表

public:
    Book(const string& is = "", const string& t = "", const string& a = "",
        const string& k = "", const string& i = "", int q = 0,int to=0,
        BookCategory cat = BookCategory::COMPUTER_SCIENCE_INFORMATION);

    // 原有的setter方法
    void setTitle(const string& newTitle) { title = newTitle; }
    void setAuthor(const string& newAuthor) { author = newAuthor; }
    void setIntro(const string& newintro) { introduction = newintro; }
    void setquantity(const int& newquantity) { quantity = newquantity; }

    // 新增的setter和getter方法
    void setCategory(BookCategory cat) { category = cat; }
    BookCategory getCategory() const { return category; }
    int getTotalBorrowCount() const { return totalBorrowCount; }
    void addTotalBorrowCount() {
        totalBorrowCount++;
        
    }

    // 评分相关方法
    void addReview(const string& username, int rating, const string& comment) {
        if (rating >= 1 && rating <= 5) {
            reviews.emplace_back(username, rating, comment);
        }
    }

    double getAverageRating() const {
        if (reviews.empty()) return 0.0;

        int totalRating = 0;
        for (const auto& review : reviews) {
            totalRating += review.rating;
        }
        return static_cast<double>(totalRating) / reviews.size();
    }

    int getReviewCount() const { return reviews.size(); }
    const vector<Review>& getReviews() const { return reviews; }

    // 获取分类名称
    string getCategoryName() const {
        switch (category) {
        case BookCategory::COMPUTER_SCIENCE_INFORMATION: return "计算机科学、知识学、总论";
        case BookCategory::PHILOSOPHY_PSYCHOLOGY: return "哲学、心理学";
        case BookCategory::RELIGION: return "宗教";
        case BookCategory::SOCIAL_SCIENCES: return "社会科学";
        case BookCategory::LANGUAGE: return "语言";
        case BookCategory::NATURAL_SCIENCES_MATHEMATICS: return "自然科学、数学";
        case BookCategory::TECHNOLOGY_APPLIED_SCIENCES: return "技术（应用科学）";
        case BookCategory::THE_ARTS_RECREATION: return "艺术、娱乐、体育";
        case BookCategory::LITERATURE_RHETORIC: return "文学";
        case BookCategory::HISTORY_GEOGRAPHY: return "历史、传记、地理";
        default: return "未知分类";
        }
    }

    // 分割关键词
    void splitkeywords(const string& keys) {
        keywords.clear();
        size_t start = 0, end = 0;
        do {
            end = keys.find(' ', start);
            if (end != start) {
                string kw = keys.substr(start, end - start);
                if (!kw.empty()) keywords.push_back(kw);
            }
            if (end == string::npos) break;
            start = end + 1;
        } while (true);
    }

    // 重写序列化方法
    bool loadFromStream(istream& file) override {
        try {
            if (!getline(file, ISBN) || ISBN.empty()) return false;
            if (!getline(file, title)) return false;
            if (!getline(file, author)) return false;

            string keywordstr;
            if (!getline(file, keywordstr)) return false;
            splitkeywords(keywordstr);

            if (!getline(file, introduction)) return false;

            string temp;
            if (!getline(file, temp)) return false;
            quantity = stoi(temp);

            if (!getline(file, temp)) return false;
            borrowed = stoi(temp);

            // 读取分类
            if (!getline(file, temp)) return false;
            category = static_cast<BookCategory>(stoi(temp));

            // 读取总借阅数
            if (!getline(file, temp)) return false;
            totalBorrowCount = stoi(temp);

            // 读取评论数量
            if (!getline(file, temp)) return false;
            int reviewCount = stoi(temp);

            // 读取评论
            reviews.clear();
            for (int i = 0; i < reviewCount; i++) {
                string username, ratingStr, comment, dateStr;
                if (!getline(file, username)) break;
                if (!getline(file, ratingStr)) break;
                if (!getline(file, comment)) break;
                if (!getline(file, dateStr)) break;

                Review review(username, stoi(ratingStr), comment);

                // 解析日期
                size_t pos1 = dateStr.find('/');
                size_t pos2 = dateStr.find('/', pos1 + 1);
                if (pos1 != string::npos && pos2 != string::npos) {
                    review.reviewDate = Date(stoi(dateStr.substr(0, pos1)),
                        stoi(dateStr.substr(pos1 + 1, pos2 - pos1 - 1)),
                        stoi(dateStr.substr(pos2 + 1)));
                }

                reviews.push_back(review);
            }

            return true;
        }
        catch (...) {
            return false;
        }
    }

    void saveToStream(ostream& file) const override {
        file << ISBN << '\n'
            << title << '\n'
            << author << '\n';

        // 保存关键词
        for (size_t i = 0; i < keywords.size(); ++i) {
            if (i != 0) file << ' ';
            file << keywords[i];
        }
        file << '\n';

        file << introduction << '\n'
            << quantity << '\n'
            << borrowed << '\n'
            << static_cast<int>(category) << '\n'
            << totalBorrowCount << '\n'
            << reviews.size() << '\n';

        // 保存评论
        for (const auto& review : reviews) {
            file << review.readerUsername << '\n'
                << review.rating << '\n'
                << review.comment << '\n'
                << review.reviewDate.toString() << '\n';
        }
    }

    // 保持原有接口
    void loadFromFile(const string& filename) override {
        ifstream file(filename);
        loadFromStream(file);
    }

    void saveToFile(const string& filename) const override {
        ofstream file(filename, ios::app);
        saveToStream(file);
    }

    bool isAvailable() const {
        return (quantity - borrowed - m_reserved) > 0;
    }

    bool borrowBook() {
        if (isAvailable()) {
            borrowed++;
            addTotalBorrowCount();
            return true;
        }
        return false;
    }

    bool returnBook() {
        if (borrowed > 0) {
            borrowed--;
            return true;
        }
        return false;
    }

    // 原有的getter方法
    string getISBN() const { return ISBN; }
    string getTitle() const { return title; }
    string getAuthor() const { return author; }
    int getQuantity() const { return quantity; }
    int getBorrowed() const { return borrowed; }
    int getAvailable() const { return quantity - borrowed - m_reserved; }
    int getReservedCount() const { return m_reserved; }
    int addTotalBorrowedCount(){ totalBorrowCount++; }
    string getintro() const { return introduction; }
    vector<string> getkeys() const { return keywords; }

    // 预约相关方法（保持不变）
    bool reserve() {
        if (!isAvailable()) return false;
        m_reserved++;
        return true;
    }

    void releaseReserve() {
        if (m_reserved > 0) m_reserved--;
    }

    void reserveFor(const string& username) {
        reservedFor = username;
        reserveExpiry = Date::currentdate();
        reserveExpiry.addDays(7);
    }

    bool isReserved() const {
        return !reservedFor.empty();
    }

    void releaseReservation() {
        reservedFor.clear();
    }

    bool isReservedFor(const string& username) const {
        return !reservedFor.empty() && reservedFor == username;
    }

    bool isReservationExpired() const {
        return Date::currentdate() > reserveExpiry;
    }

    // 增强的toString方法
    string tostring() const {
        ostringstream oss;

        oss << "ISBN: " << ISBN << "\n"
            << "书名: " << title << "\n"
            << "作者: " << author << "\n"
            << "分类: " << getCategoryName() << "\n";

        oss << "关键词: ";
        if (keywords.empty()) {
            oss << "(无)";
        }
        else {
            for (size_t i = 0; i < keywords.size(); ++i) {
                if (i != 0) oss << ", ";
                oss << keywords[i];
            }
        }
        oss << "\n";

        oss << "简介: " << (introduction.empty() ? "(无)" : introduction) << "\n"
            << "总数: " << quantity << "\n"
            << "可借: " << getAvailable() << "\n"
            << "借出: " << borrowed << "\n"
            << "总借阅次数: " << totalBorrowCount << "\n";

        if (reviews.empty()) {
            oss << "评分: 暂无评分\n"
                << "评论数: 0";
        }
        else {
            oss << "平均评分: " << fixed << setprecision(1) << getAverageRating() << " 星\n"
                << "评论数: " << reviews.size();
        }

        return oss.str();
    }

    // 显示评论详情
    string getReviewsDetail() const {
        if (reviews.empty()) {
            return "暂无评论";
        }

        ostringstream oss;
        oss << "=== 评论详情 ===\n";
        oss << "平均评分: " << fixed << setprecision(1) << getAverageRating() << " 星 (" << reviews.size() << " 条评论)\n\n";

        for (const auto& review : reviews) {
            oss << "用户: " << review.readerUsername << "\n"
                << "评分: " << review.rating << " 星\n"
                << "评论: " << review.comment << "\n"
                << "日期: " << review.reviewDate.toString() << "\n"
                << "-------------------\n";
        }

        return oss.str();
    }

    // 通用修改模板函数
    template<typename T, typename Setter>
    void modifyField(Setter setter, const T& value) {
        (this->*setter)(value);
    }
};