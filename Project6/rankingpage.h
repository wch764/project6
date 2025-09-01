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
    // ���߽��������а�
    void showReaderBorrowRanking() const;
    // ͼ����������а�
    void showBookBorrowRanking() const;
    

    // ͼ���������а�
    void showBookRatingRanking() const;
    // ����������а�
    void showCategoryBorrowRanking() const;
    // �����������а�
    void showCategoryRatingRanking() const;

    // ��ʾ�ض�����Ľ������а�
    void showCategorySpecificBorrowRanking(BookCategory category) const;
	void showCategorySpecificRatingRanking(BookCategory category) const;
};