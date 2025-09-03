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

// ����ʮ���Ʒ��෨ʮ���������
enum class BookCategory {
    COMPUTER_SCIENCE_INFORMATION = 0,    // 000-099 �������ѧ��֪ʶѧ������
    PHILOSOPHY_PSYCHOLOGY = 1,           // 100-199 ��ѧ������ѧ
    RELIGION = 2,                        // 200-299 �ڽ�
    SOCIAL_SCIENCES = 3,                 // 300-399 ����ѧ
    LANGUAGE = 4,                        // 400-499 ����
    NATURAL_SCIENCES_MATHEMATICS = 5,    // 500-599 ��Ȼ��ѧ����ѧ
    TECHNOLOGY_APPLIED_SCIENCES = 6,     // 600-699 ������Ӧ�ÿ�ѧ��
    THE_ARTS_RECREATION = 7,            // 700-799 ���������֡�����
    LITERATURE_RHETORIC = 8,             // 800-899 ��ѧ
    HISTORY_GEOGRAPHY = 9                // 900-999 ��ʷ�����ǡ�����
};

// ���۽ṹ��
struct Review {
    string readerUsername;
    int rating;           // 1-5������
    string comment;       // ��������
    Date reviewDate;      // ��������

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

    // ��������
    BookCategory category;              // ����
              // �ܽ�����
    vector<Review> reviews;            // �����б�

public:
    Book(const string& is = "", const string& t = "", const string& a = "",
        const string& k = "", const string& i = "", int q = 0,int to=0,
        BookCategory cat = BookCategory::COMPUTER_SCIENCE_INFORMATION);

    // ԭ�е�setter����
    void setTitle(const string& newTitle) { title = newTitle; }
    void setAuthor(const string& newAuthor) { author = newAuthor; }
    void setIntro(const string& newintro) { introduction = newintro; }
    void setquantity(const int& newquantity) { quantity = newquantity; }

    // ������setter��getter����
    void setCategory(BookCategory cat) { category = cat; }
    BookCategory getCategory() const { return category; }
    int getTotalBorrowCount() const { return totalBorrowCount; }
    void addTotalBorrowCount() {
        totalBorrowCount++;
        
    }

    // ������ط���
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

    // ��ȡ��������
    string getCategoryName() const {
        switch (category) {
        case BookCategory::COMPUTER_SCIENCE_INFORMATION: return "�������ѧ��֪ʶѧ������";
        case BookCategory::PHILOSOPHY_PSYCHOLOGY: return "��ѧ������ѧ";
        case BookCategory::RELIGION: return "�ڽ�";
        case BookCategory::SOCIAL_SCIENCES: return "����ѧ";
        case BookCategory::LANGUAGE: return "����";
        case BookCategory::NATURAL_SCIENCES_MATHEMATICS: return "��Ȼ��ѧ����ѧ";
        case BookCategory::TECHNOLOGY_APPLIED_SCIENCES: return "������Ӧ�ÿ�ѧ��";
        case BookCategory::THE_ARTS_RECREATION: return "���������֡�����";
        case BookCategory::LITERATURE_RHETORIC: return "��ѧ";
        case BookCategory::HISTORY_GEOGRAPHY: return "��ʷ�����ǡ�����";
        default: return "δ֪����";
        }
    }

    // �ָ�ؼ���
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

    // ��д���л�����
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

            // ��ȡ����
            if (!getline(file, temp)) return false;
            category = static_cast<BookCategory>(stoi(temp));

            // ��ȡ�ܽ�����
            if (!getline(file, temp)) return false;
            totalBorrowCount = stoi(temp);

            // ��ȡ��������
            if (!getline(file, temp)) return false;
            int reviewCount = stoi(temp);

            // ��ȡ����
            reviews.clear();
            for (int i = 0; i < reviewCount; i++) {
                string username, ratingStr, comment, dateStr;
                if (!getline(file, username)) break;
                if (!getline(file, ratingStr)) break;
                if (!getline(file, comment)) break;
                if (!getline(file, dateStr)) break;

                Review review(username, stoi(ratingStr), comment);

                // ��������
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

        // ����ؼ���
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

        // ��������
        for (const auto& review : reviews) {
            file << review.readerUsername << '\n'
                << review.rating << '\n'
                << review.comment << '\n'
                << review.reviewDate.toString() << '\n';
        }
    }

    // ����ԭ�нӿ�
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

    // ԭ�е�getter����
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

    // ԤԼ��ط��������ֲ��䣩
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

    // ��ǿ��toString����
    string tostring() const {
        ostringstream oss;

        oss << "ISBN: " << ISBN << "\n"
            << "����: " << title << "\n"
            << "����: " << author << "\n"
            << "����: " << getCategoryName() << "\n";

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
            << "�ɽ�: " << getAvailable() << "\n"
            << "���: " << borrowed << "\n"
            << "�ܽ��Ĵ���: " << totalBorrowCount << "\n";

        if (reviews.empty()) {
            oss << "����: ��������\n"
                << "������: 0";
        }
        else {
            oss << "ƽ������: " << fixed << setprecision(1) << getAverageRating() << " ��\n"
                << "������: " << reviews.size();
        }

        return oss.str();
    }

    // ��ʾ��������
    string getReviewsDetail() const {
        if (reviews.empty()) {
            return "��������";
        }

        ostringstream oss;
        oss << "=== �������� ===\n";
        oss << "ƽ������: " << fixed << setprecision(1) << getAverageRating() << " �� (" << reviews.size() << " ������)\n\n";

        for (const auto& review : reviews) {
            oss << "�û�: " << review.readerUsername << "\n"
                << "����: " << review.rating << " ��\n"
                << "����: " << review.comment << "\n"
                << "����: " << review.reviewDate.toString() << "\n"
                << "-------------------\n";
        }

        return oss.str();
    }

    // ͨ���޸�ģ�庯��
    template<typename T, typename Setter>
    void modifyField(Setter setter, const T& value) {
        (this->*setter)(value);
    }
};