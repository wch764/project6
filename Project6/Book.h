#pragma once
#include"Date.h"
#include"Serializable.h"
#include<string>
#include<algorithm>
#include<fstream>
#include<vector>
#include<iostream>
#include <sstream>
//ͼ����
using namespace std;
class Book : public Serializable {
private:
    string ISBN;
    string title;
    string author;
    vector<string> keywords;//�ؼ����б�����
    string introduction;
    int quantity;//����
    int borrowed;//������
    int m_reserved = 0; // ����������
    std::string reservedFor;  // Ϊ�ĸ��û�����
    Date reserveExpiry;      // ����������

public:

    void setTitle(const std::string& newTitle) { title = newTitle; }
    void setAuthor(const std::string& newAuthor) { author = newAuthor; }
    void setIntro(const std::string& newintro) { introduction = newintro; }
    void setquantity(const int& newquantity) { quantity = newquantity; }
    //void setborrowed(const int& newborrowed) { borrowed = newborrowed; }
    Book(const string& is = "", const string& t = "", const string& a = "", const string& k = "", const string& i = "", int q = 0);
    //���ݿո�ָ�keywords

    void splitkeywords(const std::string& keys) {
        keywords.clear();
        size_t start = 0, end = 0;
        do {
            end = keys.find(' ', start);
            // ����ǰ�ؼ���
            if (end != start) {  // �����token
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
        // ��keywords�ÿո����ӳ��ַ���
        for (size_t i = 0; i < keywords.size(); ++i) {
            if (i != 0) file << ' ';
            file << keywords[i];
        }
        file << '\n';
        file << introduction << '\n';

        file << quantity << '\n'
            << borrowed << '\n';
    }

    // ����ԭ�нӿڵ��޸�ʵ��
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
    bool borrowBook();//�������������Ƿ�ɹ�
    bool returnBook();//�������������Ƿ�ɹ�
    string getISBN() const;
    string getTitle() const;
    string getAuthor() const;
    int getQuantity() const;
    int getBorrowed() const;
    int getAvailable() const;
    int getReservedCount() const { return m_reserved; }
    // ԤԼ����
    bool reserve() {
        if (!isAvailable()) return false;
        m_reserved++;
        return true;
    }

    // ȡ��ԤԼ/���ԤԼ����
    void releaseReserve() {
        if (m_reserved > 0) m_reserved--;
    }
    string tostring()
    {
        std::ostringstream oss;


        oss << "ISBN: " << ISBN << "\n"
            << "����: " << title << "\n"
            << "����: " << author << "\n";


        oss << "�ؼ���: ";
        if (keywords.empty()) {
            oss << "(��)";
        }
        else {
            for (size_t i = 0; i < keywords.size(); ++i) {
                if (i != 0) oss << ", ";
                oss << keywords[i];
            }
        }
        oss << "\n";


        oss << "���: " << (introduction.empty() ? "(��)" : introduction) << "\n"
            << "����: " << quantity << "\n"
            << "�ɽ�: " << (quantity - borrowed) << "\n"
            << "���: " << borrowed;

        return oss.str();
    }
    void reserveFor(const std::string& username) {
        reservedFor = username;
        reserveExpiry = Date::currentdate();
        reserveExpiry.addDays(7);
    }
    // ����Ƿ��б���
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
    // ͨ���޸�ģ�庯��
    template<typename T, typename Setter>
    void modifyField(Setter setter, const T& value) {
        (this->*setter)(value); // ���ó�Ա����
    }
};