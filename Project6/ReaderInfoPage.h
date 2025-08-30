#pragma once
#include<iostream>
#include<string>
#include"Page.h"
#include"Action.h"
#include<vector>
#include"Reader.h"
#include"LoginPage.h"
#include"clearscreen.h"
#include"Book.h"
#include"BorrowInfo.h"

// 读者信息页面类
class ReaderInfoPage : public Page, public Action {
private:
    vector<Reader>& readers;
    User* currentUser;

public:
    ReaderInfoPage(vector<Reader>& rdrs, User* user);
    bool display() const override final;
    void performAction() override final;
    void showPersonalInfo() const;
    void showAllReaders() const;
};