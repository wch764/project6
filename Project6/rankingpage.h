#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <iomanip>
#include "Page.h"
#include "Action.h"
#include "Reader.h"
#include "Book.h"
#include "User.h"
#include "clearscreen.h"


class RankingPage : public Page, public Action {
private:
    std::vector<Reader>& readers;
    std::vector<Book>& books;
    User* currentUser;

public:
    RankingPage(std::vector<Reader>& rdrs, std::vector<Book>& bks, User* user);
       

    bool display() const override final;

    void performAction() override final;

private:
    // 读者借阅量排行榜
    void showReaderBorrowRanking() const;
    // 图书借阅量排行榜
    void showBookBorrowRanking() const;
    

    // 图书评分排行榜
    void showBookRatingRanking() const;
    // 分类借阅排行榜
    void showCategoryBorrowRanking() const;
    // 分类评分排行榜
    void showCategoryRatingRanking() const;

    // 显示特定分类的借阅排行榜
    void showCategorySpecificBorrowRanking(BookCategory category) const;
	void showCategorySpecificRatingRanking(BookCategory category) const;
};