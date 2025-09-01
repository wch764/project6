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
    std::cout << "=== 排行榜 ===" << std::endl;
    std::cout << "1. 读者借阅量排行榜" << std::endl;
    std::cout << "2. 图书借阅量排行榜" << std::endl;
    std::cout << "3. 图书评分排行榜" << std::endl;
    std::cout << "4. 分类借阅排行榜" << std::endl;
    std::cout << "5. 分类评分排行榜" << std::endl;
    std::cout << "6. 返回主菜单" << std::endl;
    return false;
}

// 等待用户按回车继续
void waitForEnter(const std::string& message = "\n按回车键继续...") {
    std::cout << message;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // 清空输入缓冲区
    std::cin.get();
}

void RankingPage::performAction() {
    while (true) {
        display();

        int choice;
        if (validateInt(choice, "请选择排行榜类型: ")) {
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
                break; // 这个函数内部会处理等待
            case 5:
                showCategoryRatingRanking();
                break; // 这个函数内部会处理等待
            case 6:
                return;
            default:
                std::cout << "无效选择!" << std::endl;
                waitForEnter();
                break;
            }
        }
        else {
            std::cout << "输入无效，请重新输入!" << std::endl;
            waitForEnter();
        }
    }
}

void RankingPage::showReaderBorrowRanking() const {
    clearscreen();
    std::cout << "=== 读者借阅量排行榜（前100名）===" << std::endl;

    // 复制读者列表并按借阅量排序
    std::vector<Reader> sortedReaders = readers;
    std::sort(sortedReaders.begin(), sortedReaders.end(),
        [](const Reader& a, const Reader& b) {
            return a.getBorrowedCount() > b.getBorrowedCount();
        });

    std::cout << std::left << std::setw(6) << "排名"
        << std::setw(15) << "用户名"
        << std::setw(12) << "姓名"
        << std::setw(15) << "院系"
        << std::setw(10) << "类型"
        << std::setw(8) << "借阅量" << std::endl;
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

        // 如果借阅量为0，可以选择在这里停止显示或继续显示
        // 这里选择继续显示前100名，即使借阅量为0
    }

    if (!hasData) {
        std::cout << "暂无读者数据。" << std::endl;
    }
}

// 图书借阅量排行榜
void RankingPage::showBookBorrowRanking() const {
    clearscreen();
    std::cout << "=== 图书借阅量排行榜（前100名）===" << std::endl;

    // 复制图书列表并按借阅量排序
    std::vector<Book> sortedBooks = books;
    std::sort(sortedBooks.begin(), sortedBooks.end(),
        [](const Book& a, const Book& b) {
            return a.getTotalBorrowCount() > b.getTotalBorrowCount();
        });

    std::cout << std::left << std::setw(6) << "排名"
        << std::setw(15) << "ISBN"
        << std::setw(25) << "书名"
        << std::setw(15) << "作者"
        << std::setw(20) << "分类"
        << std::setw(8) << "借阅量" << std::endl;
    std::cout << std::string(95, '-') << std::endl;

    int rank = 1;
    int displayCount = std::min(static_cast<size_t>(100), sortedBooks.size());
    bool hasData = false;

    // 修复：移除 getTotalBorrowCount() > 0 的限制，显示所有图书
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
        std::cout << "暂无图书数据。" << std::endl;
    }
}

// 图书评分排行榜
void RankingPage::showBookRatingRanking() const {
    clearscreen();
    std::cout << "=== 图书评分排行榜（前100名）===" << std::endl;

    // 只显示有评分的图书
    std::vector<Book> ratedBooks;
    for (const auto& book : books) {
        if (book.getReviewCount() > 0) {
            ratedBooks.push_back(book);
        }
    }

    if (ratedBooks.empty()) {
        std::cout << "暂无图书评分数据。" << std::endl;
        return;
    }

    // 按平均评分排序
    std::sort(ratedBooks.begin(), ratedBooks.end(),
        [](const Book& a, const Book& b) {
            if (a.getAverageRating() == b.getAverageRating()) {
                return a.getReviewCount() > b.getReviewCount(); // 评分相同时按评论数排序
            }
            return a.getAverageRating() > b.getAverageRating();
        });

    std::cout << std::left << std::setw(6) << "排名"
        << std::setw(15) << "ISBN"
        << std::setw(25) << "书名"
        << std::setw(15) << "作者"
        << std::setw(20) << "分类"
        << std::setw(8) << "评分"
        << std::setw(8) << "评论数" << std::endl;
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

// 分类借阅排行榜
void RankingPage::showCategoryBorrowRanking() const {
    while (true) {
        clearscreen();
        std::cout << "=== 分类借阅排行榜 ===" << std::endl;
        std::cout << "请选择要查看的分类:" << std::endl;
        std::cout << "0. 计算机科学、知识学、总论" << std::endl;
        std::cout << "1. 哲学、心理学" << std::endl;
        std::cout << "2. 宗教" << std::endl;
        std::cout << "3. 社会科学" << std::endl;
        std::cout << "4. 语言" << std::endl;
        std::cout << "5. 自然科学、数学" << std::endl;
        std::cout << "6. 技术（应用科学）" << std::endl;
        std::cout << "7. 艺术、娱乐、体育" << std::endl;
        std::cout << "8. 文学" << std::endl;
        std::cout << "9. 历史、传记、地理" << std::endl;
        std::cout << "10. 返回" << std::endl;

        int categoryChoice;
        if (validateInt(categoryChoice, "请选择分类 (0-10): ")) {
            if (categoryChoice == 10) {
                return; // 返回上级菜单
            }

            if (categoryChoice >= 0 && categoryChoice <= 9) {
                BookCategory category = static_cast<BookCategory>(categoryChoice);
                showCategorySpecificBorrowRanking(category);
                waitForEnter();
            }
            else {
                std::cout << "无效选择!" << std::endl;
                waitForEnter();
            }
        }
        else {
            std::cout << "输入无效，请重新输入!" << std::endl;
            waitForEnter();
        }
    }
}

// 分类评分排行榜
void RankingPage::showCategoryRatingRanking() const {
    while (true) {
        clearscreen();
        std::cout << "=== 分类评分排行榜 ===" << std::endl;
        std::cout << "请选择要查看的分类:" << std::endl;
        std::cout << "0. 计算机科学、知识学、总论" << std::endl;
        std::cout << "1. 哲学、心理学" << std::endl;
        std::cout << "2. 宗教" << std::endl;
        std::cout << "3. 社会科学" << std::endl;
        std::cout << "4. 语言" << std::endl;
        std::cout << "5. 自然科学、数学" << std::endl;
        std::cout << "6. 技术（应用科学）" << std::endl;
        std::cout << "7. 艺术、娱乐、体育" << std::endl;
        std::cout << "8. 文学" << std::endl;
        std::cout << "9. 历史、传记、地理" << std::endl;
        std::cout << "10. 返回" << std::endl;

        int categoryChoice;
        if (validateInt(categoryChoice, "请选择分类 (0-10): ")) {
            if (categoryChoice == 10) {
                return; // 返回上级菜单
            }

            if (categoryChoice >= 0 && categoryChoice <= 9) {
                BookCategory category = static_cast<BookCategory>(categoryChoice);
                showCategorySpecificRatingRanking(category);
                waitForEnter();
            }
            else {
                std::cout << "无效选择!" << std::endl;
                waitForEnter();
            }
        }
        else {
            std::cout << "输入无效，请重新输入!" << std::endl;
            waitForEnter();
        }
    }
}

// 显示特定分类的借阅排行榜
void RankingPage::showCategorySpecificBorrowRanking(BookCategory category) const {
    clearscreen();

    // 筛选指定分类的图书
    std::vector<Book> categoryBooks;
    for (const auto& book : books) {
        if (book.getCategory() == category) {
            categoryBooks.push_back(book);
        }
    }

    if (categoryBooks.empty()) {
        std::cout << "该分类下暂无图书。" << std::endl;
        return;
    }

    // 按借阅量排序
    std::sort(categoryBooks.begin(), categoryBooks.end(),
        [](const Book& a, const Book& b) {
            return a.getTotalBorrowCount() > b.getTotalBorrowCount();
        });

    std::cout << "=== " << categoryBooks[0].getCategoryName() << " - 借阅排行榜（前30名）===" << std::endl;

    std::cout << std::left << std::setw(6) << "排名"
        << std::setw(15) << "ISBN"
        << std::setw(30) << "书名"
        << std::setw(18) << "作者"
        << std::setw(8) << "借阅量" << std::endl;
    std::cout << std::string(80, '-') << std::endl;

    int rank = 1;
    int displayCount = std::min(static_cast<size_t>(30), categoryBooks.size());
    bool hasData = false;

    // 修复：移除 getTotalBorrowCount() > 0 的限制
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
        std::cout << "该分类下暂无图书数据。" << std::endl;
    }
}

// 显示特定分类的评分排行榜
void RankingPage::showCategorySpecificRatingRanking(BookCategory category) const {
    clearscreen();

    // 筛选指定分类且有评分的图书
    std::vector<Book> categoryBooks;
    for (const auto& book : books) {
        if (book.getCategory() == category && book.getReviewCount() > 0) {
            categoryBooks.push_back(book);
        }
    }

    if (categoryBooks.empty()) {
        std::cout << "该分类下暂无已评分图书。" << std::endl;
        return;
    }

    // 按平均评分排序
    std::sort(categoryBooks.begin(), categoryBooks.end(),
        [](const Book& a, const Book& b) {
            if (a.getAverageRating() == b.getAverageRating()) {
                return a.getReviewCount() > b.getReviewCount();
            }
            return a.getAverageRating() > b.getAverageRating();
        });

    std::cout << "=== " << categoryBooks[0].getCategoryName() << " - 评分排行榜（前30名）===" << std::endl;

    std::cout << std::left << std::setw(6) << "排名"
        << std::setw(15) << "ISBN"
        << std::setw(30) << "书名"
        << std::setw(15) << "作者"
        << std::setw(8) << "评分"
        << std::setw(8) << "评论数" << std::endl;
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