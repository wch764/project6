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
void RankingPage::displayRankingBooksWithSelection(const vector<Book>& topBooks){
    while (true) {
        
       

        cout << left << setw(6) << "序号"
            << setw(6) << "排名"
            << setw(15) << "ISBN"
            << setw(30) << "书名"
            << setw(18) << "作者"
            << setw(8) << "借阅量"
            << setw(10) << "评分" << endl;
        cout << string(95, '-') << endl;

        // 显示带序号的排行榜
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

            // 显示评分信息
            if (book.getReviewCount() > 0) {
                cout << setw(10) << (to_string(book.getAverageRating()).substr(0, 3) + "星");
            }
            else {
                cout << setw(10) << "暂无评分";
            }
            cout << endl;
        }

        cout << "\n请选择要查看的书籍序号 (输入0退出)：";

        int choice;
        if (validateInt(choice)) {
            if (choice == 0) {
                return; // 退出
            }
            else if (choice >= 1 && choice <= static_cast<int>(topBooks.size())) {
               

                // 创建并显示书籍详细页面，传入回调函数
                BookDetailPage detailPage(topBooks[choice - 1], borrowCallback);
                detailPage.display();
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
void RankingPage::showBookBorrowRanking() {
    clearscreen();
    std::cout << "=== 图书借阅量排行榜（前100名）===" << std::endl;

    // 复制图书列表并按借阅量排序
    std::vector<Book> sortedBooks = books;
    std::sort(sortedBooks.begin(), sortedBooks.end(),
        [](const Book& a, const Book& b) {
            return a.getTotalBorrowCount() > b.getTotalBorrowCount();
        });

    

    // 按借阅量排序
    std::sort(sortedBooks.begin(), sortedBooks.end(),
        [](const Book& a, const Book& b) {
            return a.getTotalBorrowCount() > b.getTotalBorrowCount();
        });
    int displayCount = min(static_cast<size_t>(100), sortedBooks.size());
    vector<Book> topBooks(sortedBooks.begin(), sortedBooks.begin() + displayCount);

    displayRankingBooksWithSelection(topBooks);
}
// 图书评分排行榜
void RankingPage::showBookRatingRanking()  {
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
                return a.getReviewCount() > b.getReviewCount();
            }
            return a.getAverageRating() > b.getAverageRating();
        });

    

    int rank = 1;
    int displayCount = std::min(static_cast<size_t>(30), ratedBooks.size());
    vector<Book> topBooks(ratedBooks.begin(), ratedBooks.begin() + displayCount);

    displayRankingBooksWithSelection(topBooks);

   
   
}

// 分类借阅排行榜
void RankingPage::showCategoryBorrowRanking(){
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
void RankingPage::showCategoryRatingRanking() {
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
void RankingPage::showCategorySpecificBorrowRanking(BookCategory category)  {
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

    
    int rank = 1;
    int displayCount = std::min(static_cast<size_t>(30), categoryBooks.size());
    

    
    vector<Book> topBooks(categoryBooks.begin(), categoryBooks.begin() + displayCount);

    displayRankingBooksWithSelection(topBooks);

}

// 显示特定分类的评分排行榜
void RankingPage::showCategorySpecificRatingRanking(BookCategory category){
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


    int rank = 1;
    int displayCount = std::min(static_cast<size_t>(30), categoryBooks.size());

    vector<Book> topBooks(categoryBooks.begin(), categoryBooks.begin() + displayCount);

    displayRankingBooksWithSelection(topBooks);
}