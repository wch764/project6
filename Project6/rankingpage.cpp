#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <limits>
#include "Page.h"
#include "Action.h"
#include "Reader.h"
#include "Book.h"
#include "User.h"
#include "clearscreen.h"
#include "failmanager.h"
#include "rankingpage.h"

RankingPage::RankingPage(std::vector<Reader>& rdrs, std::vector<Book>& bks, User* user)
    : readers(rdrs), books(bks), currentUser(user) {
}

bool RankingPage::display() const {
    clearscreen();
    std::cout << "=== ���а� ===" << std::endl;
    std::cout << "1. ���߽��������а�" << std::endl;
    std::cout << "2. ͼ����������а�" << std::endl;
    std::cout << "3. ͼ���������а�" << std::endl;
    std::cout << "4. ����������а�" << std::endl;
    std::cout << "5. �����������а�" << std::endl;
    std::cout << "6. �������˵�" << std::endl;
    return false;
}

// �ȴ��û����س�����
void waitForEnter(const std::string& message = "\n���س�������...") {
    std::cout << message;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // ������뻺����
    std::cin.get();
}

void RankingPage::performAction() {
    while (true) {
        display();

        int choice;
        if (validateInt(choice, "��ѡ�����а�����: ")) {
            switch (choice) {
            case 1:
                showReaderBorrowRanking();
                waitForEnter();
                break;
            case 2:
                showBookBorrowRanking();
                waitForEnter();
                break;
            case 3:
                showBookRatingRanking();
                waitForEnter();
                break;
            case 4:
                showCategoryBorrowRanking();
                break; // ��������ڲ��ᴦ��ȴ�
            case 5:
                showCategoryRatingRanking();
                break; // ��������ڲ��ᴦ��ȴ�
            case 6:
                return;
            default:
                std::cout << "��Чѡ��!" << std::endl;
                waitForEnter();
                break;
            }
        }
        else {
            std::cout << "������Ч������������!" << std::endl;
            waitForEnter();
        }
    }
}

void RankingPage::showReaderBorrowRanking() const {
    clearscreen();
    std::cout << "=== ���߽��������а�ǰ100����===" << std::endl;

    // ���ƶ����б�������������
    std::vector<Reader> sortedReaders = readers;
    std::sort(sortedReaders.begin(), sortedReaders.end(),
        [](const Reader& a, const Reader& b) {
            return a.getBorrowedCount() > b.getBorrowedCount();
        });

    std::cout << std::left << std::setw(6) << "����"
        << std::setw(15) << "�û���"
        << std::setw(12) << "����"
        << std::setw(15) << "Ժϵ"
        << std::setw(10) << "����"
        << std::setw(8) << "������" << std::endl;
    std::cout << std::string(70, '-') << std::endl;

    int rank = 1;
    int displayCount = std::min(static_cast<size_t>(100), sortedReaders.size());
    bool hasData = false;

    
    for (int i = 0; i < displayCount; i++) {
        const Reader& reader = sortedReaders[i];
        std::cout << std::left << std::setw(6) << rank++
            << std::setw(15) << reader.getUsername()
            << std::setw(12) << reader.getName()
            << std::setw(15) << reader.getDepartmentName()
            << std::setw(10) << reader.getStudentTypeName()
            << std::setw(8) << reader.getBorrowedCount() << std::endl;
        hasData = true;

        // ���������Ϊ0������ѡ��������ֹͣ��ʾ�������ʾ
        // ����ѡ�������ʾǰ100������ʹ������Ϊ0
    }

    if (!hasData) {
        std::cout << "���޶������ݡ�" << std::endl;
    }
}

// ͼ����������а�
void RankingPage::showBookBorrowRanking() const {
    clearscreen();
    std::cout << "=== ͼ����������а�ǰ100����===" << std::endl;

    // ����ͼ���б�������������
    std::vector<Book> sortedBooks = books;
    std::sort(sortedBooks.begin(), sortedBooks.end(),
        [](const Book& a, const Book& b) {
            return a.getTotalBorrowCount() > b.getTotalBorrowCount();
        });

    std::cout << std::left << std::setw(6) << "����"
        << std::setw(15) << "ISBN"
        << std::setw(25) << "����"
        << std::setw(15) << "����"
        << std::setw(20) << "����"
        << std::setw(8) << "������" << std::endl;
    std::cout << std::string(95, '-') << std::endl;

    int rank = 1;
    int displayCount = std::min(static_cast<size_t>(100), sortedBooks.size());
    bool hasData = false;

    // �޸����Ƴ� getTotalBorrowCount() > 0 �����ƣ���ʾ����ͼ��
    for (int i = 0; i < displayCount; i++) {
        const Book& book = sortedBooks[i];
        std::cout << std::left << std::setw(6) << rank++
            << std::setw(15) << book.getISBN()
            << std::setw(25) << (book.getTitle().length() > 24 ?
                book.getTitle().substr(0, 21) + "..." : book.getTitle())
            << std::setw(15) << (book.getAuthor().length() > 14 ?
                book.getAuthor().substr(0, 11) + "..." : book.getAuthor())
            << std::setw(20) << book.getCategoryName().substr(0, 19)
            << std::setw(8) << book.getTotalBorrowCount() << std::endl;
        hasData = true;
    }

    if (!hasData) {
        std::cout << "����ͼ�����ݡ�" << std::endl;
    }
}

// ͼ���������а�
void RankingPage::showBookRatingRanking() const {
    clearscreen();
    std::cout << "=== ͼ���������а�ǰ100����===" << std::endl;

    // ֻ��ʾ�����ֵ�ͼ��
    std::vector<Book> ratedBooks;
    for (const auto& book : books) {
        if (book.getReviewCount() > 0) {
            ratedBooks.push_back(book);
        }
    }

    if (ratedBooks.empty()) {
        std::cout << "����ͼ���������ݡ�" << std::endl;
        return;
    }

    // ��ƽ����������
    std::sort(ratedBooks.begin(), ratedBooks.end(),
        [](const Book& a, const Book& b) {
            if (a.getAverageRating() == b.getAverageRating()) {
                return a.getReviewCount() > b.getReviewCount(); // ������ͬʱ������������
            }
            return a.getAverageRating() > b.getAverageRating();
        });

    std::cout << std::left << std::setw(6) << "����"
        << std::setw(15) << "ISBN"
        << std::setw(25) << "����"
        << std::setw(15) << "����"
        << std::setw(20) << "����"
        << std::setw(8) << "����"
        << std::setw(8) << "������" << std::endl;
    std::cout << std::string(100, '-') << std::endl;

    int rank = 1;
    int displayCount = std::min(static_cast<size_t>(100), ratedBooks.size());

    for (int i = 0; i < displayCount; i++) {
        const Book& book = ratedBooks[i];
        std::cout << std::left << std::setw(6) << rank++
            << std::setw(15) << book.getISBN()
            << std::setw(25) << (book.getTitle().length() > 24 ?
                book.getTitle().substr(0, 21) + "..." : book.getTitle())
            << std::setw(15) << (book.getAuthor().length() > 14 ?
                book.getAuthor().substr(0, 11) + "..." : book.getAuthor())
            << std::setw(20) << book.getCategoryName().substr(0, 19)
            << std::setw(8) << std::fixed << std::setprecision(1) << book.getAverageRating()
            << std::setw(8) << book.getReviewCount() << std::endl;
    }
}

// ����������а�
void RankingPage::showCategoryBorrowRanking() const {
    while (true) {
        clearscreen();
        std::cout << "=== ����������а� ===" << std::endl;
        std::cout << "��ѡ��Ҫ�鿴�ķ���:" << std::endl;
        std::cout << "0. �������ѧ��֪ʶѧ������" << std::endl;
        std::cout << "1. ��ѧ������ѧ" << std::endl;
        std::cout << "2. �ڽ�" << std::endl;
        std::cout << "3. ����ѧ" << std::endl;
        std::cout << "4. ����" << std::endl;
        std::cout << "5. ��Ȼ��ѧ����ѧ" << std::endl;
        std::cout << "6. ������Ӧ�ÿ�ѧ��" << std::endl;
        std::cout << "7. ���������֡�����" << std::endl;
        std::cout << "8. ��ѧ" << std::endl;
        std::cout << "9. ��ʷ�����ǡ�����" << std::endl;
        std::cout << "10. ����" << std::endl;

        int categoryChoice;
        if (validateInt(categoryChoice, "��ѡ����� (0-10): ")) {
            if (categoryChoice == 10) {
                return; // �����ϼ��˵�
            }

            if (categoryChoice >= 0 && categoryChoice <= 9) {
                BookCategory category = static_cast<BookCategory>(categoryChoice);
                showCategorySpecificBorrowRanking(category);
                waitForEnter();
            }
            else {
                std::cout << "��Чѡ��!" << std::endl;
                waitForEnter();
            }
        }
        else {
            std::cout << "������Ч������������!" << std::endl;
            waitForEnter();
        }
    }
}

// �����������а�
void RankingPage::showCategoryRatingRanking() const {
    while (true) {
        clearscreen();
        std::cout << "=== �����������а� ===" << std::endl;
        std::cout << "��ѡ��Ҫ�鿴�ķ���:" << std::endl;
        std::cout << "0. �������ѧ��֪ʶѧ������" << std::endl;
        std::cout << "1. ��ѧ������ѧ" << std::endl;
        std::cout << "2. �ڽ�" << std::endl;
        std::cout << "3. ����ѧ" << std::endl;
        std::cout << "4. ����" << std::endl;
        std::cout << "5. ��Ȼ��ѧ����ѧ" << std::endl;
        std::cout << "6. ������Ӧ�ÿ�ѧ��" << std::endl;
        std::cout << "7. ���������֡�����" << std::endl;
        std::cout << "8. ��ѧ" << std::endl;
        std::cout << "9. ��ʷ�����ǡ�����" << std::endl;
        std::cout << "10. ����" << std::endl;

        int categoryChoice;
        if (validateInt(categoryChoice, "��ѡ����� (0-10): ")) {
            if (categoryChoice == 10) {
                return; // �����ϼ��˵�
            }

            if (categoryChoice >= 0 && categoryChoice <= 9) {
                BookCategory category = static_cast<BookCategory>(categoryChoice);
                showCategorySpecificRatingRanking(category);
                waitForEnter();
            }
            else {
                std::cout << "��Чѡ��!" << std::endl;
                waitForEnter();
            }
        }
        else {
            std::cout << "������Ч������������!" << std::endl;
            waitForEnter();
        }
    }
}

// ��ʾ�ض�����Ľ������а�
void RankingPage::showCategorySpecificBorrowRanking(BookCategory category) const {
    clearscreen();

    // ɸѡָ�������ͼ��
    std::vector<Book> categoryBooks;
    for (const auto& book : books) {
        if (book.getCategory() == category) {
            categoryBooks.push_back(book);
        }
    }

    if (categoryBooks.empty()) {
        std::cout << "�÷���������ͼ�顣" << std::endl;
        return;
    }

    // ������������
    std::sort(categoryBooks.begin(), categoryBooks.end(),
        [](const Book& a, const Book& b) {
            return a.getTotalBorrowCount() > b.getTotalBorrowCount();
        });

    std::cout << "=== " << categoryBooks[0].getCategoryName() << " - �������а�ǰ30����===" << std::endl;

    std::cout << std::left << std::setw(6) << "����"
        << std::setw(15) << "ISBN"
        << std::setw(30) << "����"
        << std::setw(18) << "����"
        << std::setw(8) << "������" << std::endl;
    std::cout << std::string(80, '-') << std::endl;

    int rank = 1;
    int displayCount = std::min(static_cast<size_t>(30), categoryBooks.size());
    bool hasData = false;

    // �޸����Ƴ� getTotalBorrowCount() > 0 ������
    for (int i = 0; i < displayCount; i++) {
        const Book& book = categoryBooks[i];
        std::cout << std::left << std::setw(6) << rank++
            << std::setw(15) << book.getISBN()
            << std::setw(30) << (book.getTitle().length() > 29 ?
                book.getTitle().substr(0, 26) + "..." : book.getTitle())
            << std::setw(18) << (book.getAuthor().length() > 17 ?
                book.getAuthor().substr(0, 14) + "..." : book.getAuthor())
            << std::setw(8) << book.getTotalBorrowCount() << std::endl;
        hasData = true;
    }

    if (!hasData) {
        std::cout << "�÷���������ͼ�����ݡ�" << std::endl;
    }
}

// ��ʾ�ض�������������а�
void RankingPage::showCategorySpecificRatingRanking(BookCategory category) const {
    clearscreen();

    // ɸѡָ�������������ֵ�ͼ��
    std::vector<Book> categoryBooks;
    for (const auto& book : books) {
        if (book.getCategory() == category && book.getReviewCount() > 0) {
            categoryBooks.push_back(book);
        }
    }

    if (categoryBooks.empty()) {
        std::cout << "�÷���������������ͼ�顣" << std::endl;
        return;
    }

    // ��ƽ����������
    std::sort(categoryBooks.begin(), categoryBooks.end(),
        [](const Book& a, const Book& b) {
            if (a.getAverageRating() == b.getAverageRating()) {
                return a.getReviewCount() > b.getReviewCount();
            }
            return a.getAverageRating() > b.getAverageRating();
        });

    std::cout << "=== " << categoryBooks[0].getCategoryName() << " - �������а�ǰ30����===" << std::endl;

    std::cout << std::left << std::setw(6) << "����"
        << std::setw(15) << "ISBN"
        << std::setw(30) << "����"
        << std::setw(15) << "����"
        << std::setw(8) << "����"
        << std::setw(8) << "������" << std::endl;
    std::cout << std::string(85, '-') << std::endl;

    int rank = 1;
    int displayCount = std::min(static_cast<size_t>(30), categoryBooks.size());

    for (int i = 0; i < displayCount; i++) {
        const Book& book = categoryBooks[i];
        std::cout << std::left << std::setw(6) << rank++
            << std::setw(15) << book.getISBN()
            << std::setw(30) << (book.getTitle().length() > 29 ?
                book.getTitle().substr(0, 26) + "..." : book.getTitle())
            << std::setw(15) << (book.getAuthor().length() > 14 ?
                book.getAuthor().substr(0, 11) + "..." : book.getAuthor())
            << std::setw(8) << std::fixed << std::setprecision(1) << book.getAverageRating()
            << std::setw(8) << book.getReviewCount() << std::endl;
    }
}