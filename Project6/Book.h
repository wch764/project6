#pragma once
#include"Date.h"
#include"Serializable.h"
#include<string>
#include<algorithm>
#include<fstream>
#include<vector>
#include<iostream>
#include <sstream>
//图书类
using namespace std;
class Book : public Serializable {
private:
    string ISBN;
    string title;
    string author;
    vector<string> keywords;//关键词列表向量
    string introduction;
    int quantity;//存量
    int borrowed;//出借量
    int m_reserved = 0; // 被保留数量
    std::string reservedFor;  // 为哪个用户保留
    Date reserveExpiry;      // 保留到期日

public:

    void setTitle(const std::string& newTitle) { title = newTitle; }
    void setAuthor(const std::string& newAuthor) { author = newAuthor; }
    void setIntro(const std::string& newintro) { introduction = newintro; }
    void setquantity(const int& newquantity) { quantity = newquantity; }
    //void setborrowed(const int& newborrowed) { borrowed = newborrowed; }
    Book(const string& is = "", const string& t = "", const string& a = "", const string& k = "", const string& i = "", int q = 0);
    //根据空格分割keywords

    void splitkeywords(const std::string& keys) {
        keywords.clear();
        size_t start = 0, end = 0;
        do {
            end = keys.find(' ', start);
            // 处理当前关键词
            if (end != start) {  // 避免空token
                string kw = keys.substr(start, end - start);
                if (!kw.empty()) keywords.push_back(kw);
            }
            if (end == string::npos) break;
            start = end + 1;
        } while (true);
    }
    
    bool loadFromStream(std::istream& file) override {
        try {
            if (!getline(file, ISBN) || ISBN.empty()) return false;
            if (!getline(file, title)) return false;
            if (!getline(file, author)) return false;
            string keywordstr;
            if (!getline(file, keywordstr)) return false;
            splitkeywords(keywordstr);
            if (!getline(file, introduction)) return false;
            std::string temp;
            if (!getline(file, temp)) return false;
            quantity = std::stoi(temp);

            if (!getline(file, temp)) return false;
            borrowed = std::stoi(temp);

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
        // 将keywords用空格连接成字符串
        for (size_t i = 0; i < keywords.size(); ++i) {
            if (i != 0) file << ' ';
            file << keywords[i];
        }
        file << '\n';
        file << introduction << '\n';

        file << quantity << '\n'
            << borrowed << '\n';
    }

    // 保持原有接口但修改实现
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
    bool borrowBook();//出借操作并检查是否成功
    bool returnBook();//还书操作并检查是否成功
    string getISBN() const;
    string getTitle() const;
    string getAuthor() const;
    int getQuantity() const;
    int getBorrowed() const;
    int getAvailable() const;
    int getReservedCount() const { return m_reserved; }
    // 预约操作
    bool reserve() {
        if (!isAvailable()) return false;
        m_reserved++;
        return true;
    }

    // 取消预约/完成预约借阅
    void releaseReserve() {
        if (m_reserved > 0) m_reserved--;
    }
    string tostring()
    {
        std::ostringstream oss;


        oss << "ISBN: " << ISBN << "\n"
            << "书名: " << title << "\n"
            << "作者: " << author << "\n";


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
            << "可借: " << (quantity - borrowed) << "\n"
            << "借出: " << borrowed;

        return oss.str();
    }
    void reserveFor(const std::string& username) {
        reservedFor = username;
        reserveExpiry = Date::currentdate();
        reserveExpiry.addDays(7);
    }
    // 检查是否有保留
    bool isReserved() const {
        return !reservedFor.empty();
    }
    void releaseReservation() {
        reservedFor.clear();
    }

    bool isReservedFor(const std::string& username) const {
        return !reservedFor.empty() && reservedFor == username;
    }

    bool isReservationExpired() const {
        return Date::currentdate() > reserveExpiry;
    }
    string getintro() const { return introduction; }
    vector<std::string> getkeys() const { return keywords; }
    // 通用修改模板函数
    template<typename T, typename Setter>
    void modifyField(Setter setter, const T& value) {
        (this->*setter)(value); // 调用成员函数
    }
};