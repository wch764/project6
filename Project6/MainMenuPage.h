// MainMenuPage.h - 更新版本
#pragma once
#include<iostream>
#include<string>
#include"Page.h"
#include"Action.h"
#include<vector>
#include"Reader.h"
#include"Book.h"
#include"BorrowInfo.h"
#include"User.h"
#include"reservemanager.h"
#include"notificationcenter.h"

class MainMenuPage : public Page, public Action {
private:
    vector<Book>& books;
    vector<BorrowInfo>& borrowInfos;
    User* currentUser;
    vector<Reader>& readers;
    ReservationManager& reservationManager;
    NotificationManager& notificationManager; // 添加通知管理器

public:
    // 构造函数接受NotificationManager引用
    MainMenuPage(vector<Book>& bs, vector<BorrowInfo>& bis, User* user,
        vector<Reader>& rdrs, ReservationManager& rm, NotificationManager& nm);
    bool display() const override final;
    void performAction() override final;

    // 指针转换函数保持不变
    static void convertToPointerVector(vector<Reader>& readers, vector<Reader*>& pointers) {
        pointers.clear();
        pointers.reserve(readers.size());

        cout << "转换指针向量，源数据大小: " << readers.size() << endl;

        for (size_t i = 0; i < readers.size(); ++i) {
            pointers.push_back(&readers[i]);
            cout << "添加指针 " << i << ": " << readers[i].getUsername() << endl;
        }

        cout << "指针向量转换完成，大小: " << pointers.size() << endl;
    }
};

