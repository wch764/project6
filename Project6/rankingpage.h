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
    // ���߽��������а�
    void showReaderBorrowRanking() const;
    // ͼ����������а�
    void showBookBorrowRanking() ;
    

    // ͼ���������а�
    void showBookRatingRanking();
    // ����������а�
    void showCategoryBorrowRanking();
    // �����������а�
    void showCategoryRatingRanking();

    // ��ʾ�ض�����Ľ������а�
    void showCategorySpecificBorrowRanking(BookCategory category);
	void showCategorySpecificRatingRanking(BookCategory category) ;
};