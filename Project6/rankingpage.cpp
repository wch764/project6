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
#include "bookdetailpage.h" 

RankingPage::RankingPage(std::vector<Reader>& rdrs, std::vector<Book>& bks, User* user, function<void(const string&)> cb)
    : readers(rdrs), books(bks), currentUser(user), borrowCallback(cb) {
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
void RankingPage::displayRankingBooksWithSelection(const vector<Book>& topBooks){
    while (true) {
        
       

        cout << left << setw(6) << "���"
            << setw(6) << "����"
            << setw(15) << "ISBN"
            << setw(30) << "����"
            << setw(18) << "����"
            << setw(8) << "������"
            << setw(10) << "����" << endl;
        cout << string(95, '-') << endl;

        // ��ʾ����ŵ����а�
        for (size_t i = 0; i < topBooks.size(); ++i) {
            const Book& book = topBooks[i];
            cout << left << setw(6) << ("[" + to_string(i + 1) + "]")
                << setw(6) << (i + 1)
                << setw(15) << book.getISBN()
                << setw(30) << (book.getTitle().length() > 29 ?
                    book.getTitle().substr(0, 26) + "..." : book.getTitle())
                << setw(18) << (book.getAuthor().length() > 17 ?
                    book.getAuthor().substr(0, 14) + "..." : book.getAuthor())
                << setw(8) << book.getTotalBorrowCount();

            // ��ʾ������Ϣ
            if (book.getReviewCount() > 0) {
                cout << setw(10) << (to_string(book.getAverageRating()).substr(0, 3) + "��");
            }
            else {
                cout << setw(10) << "��������";
            }
            cout << endl;
        }

        cout << "\n��ѡ��Ҫ�鿴���鼮��� (����0�˳�)��";

        int choice;
        if (validateInt(choice)) {
            if (choice == 0) {
                return; // �˳�
            }
            else if (choice >= 1 && choice <= static_cast<int>(topBooks.size())) {
               

                // ��������ʾ�鼮��ϸҳ�棬����ص�����
                BookDetailPage detailPage(topBooks[choice - 1], borrowCallback);
                detailPage.display();
            }
            else {
                cout << "��Ч��ţ����������롣\n";
                system("pause");
            }
        }
        else {
            cout << "������Ч�����������֡�\n";
            system("pause");
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
void RankingPage::showBookBorrowRanking() {
    clearscreen();
    std::cout << "=== ͼ����������а�ǰ100����===" << std::endl;

    // ����ͼ���б�������������
    std::vector<Book> sortedBooks = books;
    std::sort(sortedBooks.begin(), sortedBooks.end(),
        [](const Book& a, const Book& b) {
            return a.getTotalBorrowCount() > b.getTotalBorrowCount();
        });

    

    // ������������
    std::sort(sortedBooks.begin(), sortedBooks.end(),
        [](const Book& a, const Book& b) {
            return a.getTotalBorrowCount() > b.getTotalBorrowCount();
        });
    int displayCount = min(static_cast<size_t>(100), sortedBooks.size());
    vector<Book> topBooks(sortedBooks.begin(), sortedBooks.begin() + displayCount);

    displayRankingBooksWithSelection(topBooks);
}
// ͼ���������а�
void RankingPage::showBookRatingRanking()  {
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
                return a.getReviewCount() > b.getReviewCount();
            }
            return a.getAverageRating() > b.getAverageRating();
        });

    

    int rank = 1;
    int displayCount = std::min(static_cast<size_t>(30), ratedBooks.size());
    vector<Book> topBooks(ratedBooks.begin(), ratedBooks.begin() + displayCount);

    displayRankingBooksWithSelection(topBooks);

   
   
}

// ����������а�
void RankingPage::showCategoryBorrowRanking(){
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
void RankingPage::showCategoryRatingRanking() {
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
void RankingPage::showCategorySpecificBorrowRanking(BookCategory category)  {
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

    
    int rank = 1;
    int displayCount = std::min(static_cast<size_t>(30), categoryBooks.size());
    

    
    vector<Book> topBooks(categoryBooks.begin(), categoryBooks.begin() + displayCount);

    displayRankingBooksWithSelection(topBooks);

}

// ��ʾ�ض�������������а�
void RankingPage::showCategorySpecificRatingRanking(BookCategory category){
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


    int rank = 1;
    int displayCount = std::min(static_cast<size_t>(30), categoryBooks.size());

    vector<Book> topBooks(categoryBooks.begin(), categoryBooks.begin() + displayCount);

    displayRankingBooksWithSelection(topBooks);
}