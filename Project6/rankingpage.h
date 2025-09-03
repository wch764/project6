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
#include "failmanager.h"
#include<functional>

class RankingPage : public Page, public Action {
private:
    std::vector<Reader>& readers;
    std::vector<Book>& books;
    User* currentUser;
    function<void(const string&)> borrowCallback;

public:
    RankingPage(std::vector<Reader>& rdrs, std::vector<Book>& bks, User* user, function<void(const string&)> cb);
       

    bool display() const override final;

    void performAction() override final;

private:
    void displayRankingBooksWithSelection(const vector<Book>& topBooks);
    // 读者借阅量排行榜
    void showReaderBorrowRanking() const;
    // 图书借阅量排行榜
    void showBookBorrowRanking() ;
    

    // 图书评分排行榜
    void showBookRatingRanking();
    // 分类借阅排行榜
    void showCategoryBorrowRanking();
    // 分类评分排行榜
    void showCategoryRatingRanking();

    // 显示特定分类的借阅排行榜
    void showCategorySpecificBorrowRanking(BookCategory category);
	void showCategorySpecificRatingRanking(BookCategory category) ;
};