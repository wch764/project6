#pragma once
#include<iostream>
#include"Page.h"
#include"Action.h"
#include<vector>
#include"Book.h"
#include"BorrowInfo.h"
#include"User.h"
class BorrowStatisticsPage : public Page, public Action {
private:
    vector<BorrowInfo>& borrowInfos;
    vector<Book>& books;
    User* currentUser;

public:
    BorrowStatisticsPage(vector<BorrowInfo>& bis, vector<Book>& bs,User*u);
    bool display() const override final;
    void performAction() override final;
    void showAllRecords() const;
    void showCurrentAllBorrows() const;
    string findBookTitle(const string& isbn) const;
    void showUserRecords()const;
    void showUserCurrentBorrows()const;
};