#include<string>
#include"Page.h"
#include"Action.h"
#include<vector>
#include"Reader.h"
#include"LoginPage.h"
#include"clearscreen.h"
#include"Book.h"
#include"BorrowInfo.h"
#include"BorrowStatisticsPage.h"
#include"Bookpage.h"
#include"ReaderInfoPage.h"
using namespace std;
ReaderInfoPage::ReaderInfoPage(vector<Reader>& rdrs, User* user)
        : readers(rdrs), currentUser(user) {
    }

bool ReaderInfoPage::display() const {
    bool isAdmin = currentUser->get_isadmin();

    clearscreen();
    if (isAdmin) {
        std::cout << "=== 读者管理 ===" << std::endl;
        std::cout << "1. 查看所有读者信息" << std::endl;
        std::cout << "2. 注册新用户" << std::endl;
        std::cout << "3. 修改读者毕业日期" << std::endl;
        std::cout << "4. 处理罚款缴纳" << std::endl;
        std::cout << "5. 删除读者账号" << std::endl;
        std::cout << "6. 返回主菜单" << std::endl;
    }
    else {
        std::cout << "=== 个人信息管理 ===" << std::endl;
        std::cout << "1. 查看个人信息" << std::endl;
        std::cout << "2. 修改用户名" << std::endl;
        std::cout << "3. 修改密码" << std::endl;
        std::cout << "4. 返回主菜单" << std::endl;
    }

    return isAdmin;
}

void ReaderInfoPage::performAction() {
    while (true) {
        bool isAdmin = display();

        int choice;
        std::cout << "请选择操作: ";
        if (validateInt(choice)) {
            if (isAdmin) {
                switch (choice) {
                case 1: showAllReaders(); system("pause"); break;
                case 2: registerNewUser(); system("pause"); break;
                case 3: modifyGraduationDate(); system("pause"); break;
                case 4: processFinePay(); system("pause"); break;
                case 5: deleteReader(); system("pause"); break;
                case 6: return;
                default: std::cout << "无效选择!" << std::endl;
                }
            }
            else {
                switch (choice) {
                case 1: showPersonalInfo(); system("pause"); break;
                case 2: modifyUsername(); system("pause"); break;
                case 3: modifyPassword(); system("pause"); break;
                case 4: return;
                default: std::cout << "无效选择!" << std::endl;
                }
            }
        }
    }
}

// 管理员功能：查看所有读者信息
void ReaderInfoPage::showAllReaders() const {
    clearscreen();
    std::cout << "=== 所有读者信息 ===" << std::endl;
    std::cout << std::left << std::setw(15) << "用户名"
        << std::setw(12) << "姓名"
        << std::setw(15) << "院系"
        << std::setw(10) << "类型"
        << std::setw(12) << "毕业日期"
        << std::setw(8) << "借书数"
        << std::setw(10) << "罚款(元)" << std::endl;
    std::cout << std::string(80, '-') << std::endl;

    for (const auto& reader : readers) {
        std::cout << std::left << std::setw(15) << reader.getUsername()
            << std::setw(12) << reader.getName()
            << std::setw(15) << reader.getDepartmentName()
            << std::setw(10) << reader.getStudentTypeName()
            << std::setw(12) << reader.getGraduationDate().toString()
            << std::setw(8) << reader.getBorrowedCount()
            << std::setw(10) << std::fixed << std::setprecision(2)
            << reader.getFineAmount() << std::endl;
    }
}

// 管理员功能：注册新用户
void ReaderInfoPage::registerNewUser() {
    clearscreen();
    std::cout << "=== 注册新用户 ===" << std::endl;
    if (std::cin.rdbuf()->in_avail() > 0) {
        cin.ignore();
    }
    std::string username, password, name;
    int deptChoice, typeChoice;


    std::cout << "输入用户名: ";
    std::getline(std::cin, username);

    // 检查用户名是否已存在
    auto it = std::find_if(readers.begin(), readers.end(),
        [&username](const Reader& r) { return r.getUsername() == username; });

    if (it != readers.end()) {
        std::cout << "用户名已存在!" << std::endl;
        return;
    }

    std::cout << "输入密码: ";
    std::getline(std::cin, password);
    std::cout << "输入真实姓名: ";
    std::getline(std::cin, name);

    // 选择院系
    std::cout << "\n选择院系:\n";
    std::cout << "0. 计算机科学  1. 数学      2. 物理\n";
    std::cout << "3. 化学        4. 生物      5. 文学\n";
    std::cout << "6. 历史        7. 经济学    8. 法学\n";
    std::cout << "9. 医学        10. 工程学   11. 其他\n";
    std::cout << "请选择 (0-11): ";

    if (!validateInt(deptChoice) || deptChoice < 0 || deptChoice > 11) {
        std::cout << "无效的院系选择!" << std::endl;
        return;
    }

    // 选择学生类型
    std::cout << "\n选择学生类型:\n";
    std::cout << "0. 本科生\n";
    std::cout << "1. 研究生\n";
    std::cout << "请选择 (0-1): ";

    if (!validateInt(typeChoice) || (typeChoice != 0 && typeChoice != 1)) {
        std::cout << "无效的学生类型选择!" << std::endl;
        return;
    }

    // 创建新读者
    Department dept = static_cast<Department>(deptChoice);
    StudentType type = static_cast<StudentType>(typeChoice);

    readers.emplace_back(username, password, name, dept, type);

    std::cout << "用户注册成功!" << std::endl;
    std::cout << "用户名: " << username << std::endl;
    std::cout << "预计毕业日期: " << readers.back().getGraduationDate().toString() << std::endl;
}

// 管理员功能：修改读者毕业日期
void ReaderInfoPage::modifyGraduationDate() {
    clearscreen();
    std::cout << "=== 修改读者毕业日期 ===" << std::endl;

    std::string username;
    std::cin.ignore();
    std::cout << "输入要修改的用户名: ";
    std::getline(std::cin, username);

    auto it = std::find_if(readers.begin(), readers.end(),
        [&username](const Reader& r) { return r.getUsername() == username; });

    if (it == readers.end()) {
        std::cout << "找不到该用户!" << std::endl;
        return;
    }

    Reader& reader = *it;
    std::cout << "当前信息:\n" << reader.getDetailedInfo() << std::endl;

    std::cout << "\n修改选项:\n";
    std::cout << "1. 重新设置学生类型（会自动重新计算毕业日期）\n";
    std::cout << "2. 手动设置毕业日期\n";
    std::cout << "3. 取消\n";
    std::cout << "请选择: ";

    int option;
    if (!validateInt(option)) return;

    switch (option) {
    case 1: {
        std::cout << "选择新的学生类型:\n";
        std::cout << "0. 本科生（注册日期+4年）\n";
        std::cout << "1. 研究生（注册日期+3年）\n";
        std::cout << "请选择: ";

        int typeChoice;
        if (validateInt(typeChoice) && (typeChoice == 0 || typeChoice == 1)) {
            reader.setStudentType(static_cast<StudentType>(typeChoice));
            std::cout << "学生类型已更新，新的毕业日期: "
                << reader.getGraduationDate().toString() << std::endl;
        }
        else {
            std::cout << "无效选择!" << std::endl;
        }
        break;
    }
    case 2: {
        int year, month, day;
        std::cout << "输入新的毕业日期 (年 月 日): ";
        if (std::cin >> year >> month >> day) {
            Date newDate(year, month, day);
            if (newDate > Date::currentdate()) {
                reader.setGraduationDate(newDate);
                std::cout << "毕业日期已更新为: " << newDate.toString() << std::endl;
            }
            else {
                std::cout << "毕业日期不能是过去的日期!" << std::endl;
            }
        }
        else {
            std::cout << "日期格式错误!" << std::endl;
        }
        break;
    }
    case 3:
        return;
    default:
        std::cout << "无效选择!" << std::endl;
    }
}

// 管理员功能：处理罚款缴纳
void ReaderInfoPage::processFinePay() {
    clearscreen();
    std::cout << "=== 处理罚款缴纳 ===" << std::endl;

    std::string username;
    std::cin.ignore();
    std::cout << "输入要处理的用户名: ";
    std::getline(std::cin, username);

    auto it = std::find_if(readers.begin(), readers.end(),
        [&username](const Reader& r) { return r.getUsername() == username; });

    if (it == readers.end()) {
        std::cout << "找不到该用户!" << std::endl;
        return;
    }

    Reader& reader = *it;
    double currentFine = reader.getFineAmount();

    if (currentFine <= 0) {
        std::cout << "该用户没有未缴纳的罚款。" << std::endl;
        return;
    }

    std::cout << "用户: " << reader.getName() << " (" << reader.getUsername() << ")\n";
    std::cout << "当前罚款金额: " << std::fixed << std::setprecision(2)
        << currentFine << " 元\n";

    std::cout << "\n选择操作:\n";
    std::cout << "1. 全额缴纳\n";
    std::cout << "2. 部分缴纳\n";
    std::cout << "3. 免除罚款\n";
    std::cout << "4. 取消\n";
    std::cout << "请选择: ";

    int choice;
    if (!validateInt(choice)) return;

    switch (choice) {
    case 1: {
        reader.setFineAmount(0.0);
        std::cout << "已全额缴纳罚款 " << currentFine << " 元。" << std::endl;

        // 标记该用户的罚款通知为已读
        markFineNotificationsAsPaid(reader);
        break;
    }
    case 2: {
        double payAmount;
        std::cout << "输入缴纳金额: ";
        if (std::cin >> payAmount && payAmount > 0 && payAmount <= currentFine) {
            if (reader.payFine(payAmount)) {
                std::cout << "已缴纳 " << payAmount << " 元，剩余罚款: "
                    << reader.getFineAmount() << " 元。" << std::endl;

                if (reader.getFineAmount() == 0) {
                    markFineNotificationsAsPaid(reader);
                }
            }
        }
        else {
            std::cout << "无效的缴纳金额!" << std::endl;
        }
        break;
    }
    case 3: {
        reader.setFineAmount(0.0);
        std::cout << "已免除罚款 " << currentFine << " 元。" << std::endl;
        markFineNotificationsAsPaid(reader);
        break;
    }
    case 4:
        return;
    default:
        std::cout << "无效选择!" << std::endl;
    }
}

// 管理员功能：删除读者账号
void ReaderInfoPage::deleteReader() {
    clearscreen();
    std::cout << "=== 删除读者账号 ===" << std::endl;

    std::string username;
    std::cin.ignore();
    std::cout << "输入要删除的用户名: ";
    std::getline(std::cin, username);

    auto it = std::find_if(readers.begin(), readers.end(),
        [&username](const Reader& r) { return r.getUsername() == username; });

    if (it == readers.end()) {
        std::cout << "找不到该用户!" << std::endl;
        return;
    }

    const Reader& reader = *it;
    std::cout << "即将删除用户:\n" << reader.getDetailedInfo() << std::endl;

    std::cout << "\n确认删除? (输入 'DELETE' 确认): ";
    std::string confirmation;
    std::getline(std::cin, confirmation);

    if (confirmation == "DELETE") {
        readers.erase(it);
        std::cout << "用户账号已删除。" << std::endl;
    }
    else {
        std::cout << "删除操作已取消。" << std::endl;
    }
}

// 普通用户功能：查看个人信息
void ReaderInfoPage::showPersonalInfo() const {
    clearscreen();
    std::cout << "=== 个人信息 ===" << std::endl;

    // 查找当前用户的详细信息
    auto it = std::find_if(readers.begin(), readers.end(),
        [this](const Reader& r) { return r.getUsername() == currentUser->getUsername(); });

    if (it != readers.end()) {
        std::cout << it->getDetailedInfo() << std::endl;
    }
    else {
        std::cout << "无法找到您的详细信息。" << std::endl;
    }
}

// 普通用户功能：修改用户名
void ReaderInfoPage::modifyUsername() {
    clearscreen();
    std::cout << "=== 修改用户名 ===" << std::endl;

    std::string newUsername;
    clearInputBuffer();
    std::cout << "输入新的用户名: ";
    std::getline(std::cin, newUsername);

    // 检查新用户名是否已存在
    auto it = std::find_if(readers.begin(), readers.end(),
        [&newUsername](const Reader& r) { return r.getUsername() == newUsername; });

    if (it != readers.end()) {
        std::cout << "用户名已存在，请选择其他用户名!" << std::endl;
        return;
    }

    // 查找当前用户并修改用户名
    auto currentIt = std::find_if(readers.begin(), readers.end(),
        [this](const Reader& r) { return r.getUsername() == currentUser->getUsername(); });

    if (currentIt != readers.end()) {
		currentIt->changeUsername(newUsername);
    }
}

// 普通用户功能：修改密码
void ReaderInfoPage::modifyPassword() {
    clearscreen();
    std::cout << "=== 修改密码 ===" << std::endl;

    std::string oldPassword, newPassword;
    clearInputBuffer();
    std::cout << "输入当前密码: ";
    std::getline(std::cin, oldPassword);

    if (oldPassword != currentUser->getPassword()) {
        std::cout << "当前密码错误!" << std::endl;
        return;
    }

    std::cout << "输入新密码: ";
    std::getline(std::cin, newPassword);

    if (newPassword.length() < 6) {
        std::cout << "密码长度至少为6位!" << std::endl;
        return;
    }

    currentUser->changePassword(newPassword);
    std::cout << "密码修改成功!" << std::endl;
}

// 辅助方法：标记罚款通知为已缴纳
void ReaderInfoPage::markFineNotificationsAsPaid(Reader& reader) {
    // 清除该用户的所有罚款通知
    auto notifications = reader.getAllNotifications();
    for (auto& noti : notifications) {
        if (noti.getType() == Notification::OVERDUE_FINE) {
            noti.markAsRead(); // 标记为已读
        }
    }
    std::cout << "相关罚款通知已处理。" << std::endl;
}
