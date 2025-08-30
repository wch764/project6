#pragma once
#include<iostream>
#include"Page.h"
#include"Action.h"
#include<vector>
#include"Reader.h"
#include"Admin.h"
#include"clearscreen.h"
using namespace std;
// µÇÂ¼Ò³ÃæÀà
class LoginPage : public Page, public Action {
private:
    vector<Reader>& readers;
    vector<Admin>& admins;
    User* currentUser;

public:
    LoginPage(vector<Reader>& rdrs,vector<Admin>&adm);

    bool display() const override final;
    void performAction() override final;
    User* getCurrentUser()const;
};