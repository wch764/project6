// MainMenuPage.h - ���°汾
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
    NotificationManager& notificationManager; // ���֪ͨ������

public:
    // ���캯������NotificationManager����
    MainMenuPage(vector<Book>& bs, vector<BorrowInfo>& bis, User* user,
        vector<Reader>& rdrs, ReservationManager& rm, NotificationManager& nm);
    bool display() const override final;
    void performAction() override final;

    // ָ��ת���������ֲ���
    static void convertToPointerVector(vector<Reader>& readers, vector<Reader*>& pointers) {
        pointers.clear();
        pointers.reserve(readers.size());

        cout << "ת��ָ��������Դ���ݴ�С: " << readers.size() << endl;

        for (size_t i = 0; i < readers.size(); ++i) {
            pointers.push_back(&readers[i]);
            cout << "���ָ�� " << i << ": " << readers[i].getUsername() << endl;
        }

        cout << "ָ������ת����ɣ���С: " << pointers.size() << endl;
    }
};

