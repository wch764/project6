#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <iomanip>
#include "Page.h"
#include "Action.h"
#include "Reader.h"
#include "User.h"
#include "clearscreen.h"
#include "failmanager.h"

class ReaderInfoPage : public Page, public Action {
private:
    std::vector<Reader>& readers;
    User* currentUser;

public:
    ReaderInfoPage(std::vector<Reader>& rdrs, User* user);
    bool display() const override final;
    void performAction() override final;

private:
    // 管理员功能：查看所有读者信息
    void showAllReaders() const;
    // 管理员功能：注册新用户
    void registerNewUser();

    // 管理员功能：修改读者毕业日期
    void modifyGraduationDate();

    // 管理员功能：处理罚款缴纳
    void processFinePay();

    // 管理员功能：删除读者账号
    void deleteReader();

    // 普通用户功能：查看个人信息
    void showPersonalInfo() const;

    // 普通用户功能：修改用户名
    void modifyUsername();

    // 普通用户功能：修改密码
    void modifyPassword();

    // 辅助方法：标记罚款通知为已缴纳
    void markFineNotificationsAsPaid(Reader& reader);
};