
#pragma once
#include "User.h"
#include "Date.h"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

// 新增枚举类型
enum class StudentType {
    UNDERGRADUATE = 0,  // 本科生
    GRADUATE = 1        // 研究生
};

enum class Department {
    COMPUTER_SCIENCE = 0,    // 计算机科学
    MATHEMATICS = 1,         // 数学
    PHYSICS = 2,            // 物理
    CHEMISTRY = 3,          // 化学
    BIOLOGY = 4,            // 生物
    LITERATURE = 5,         // 文学
    HISTORY = 6,            // 历史
    ECONOMICS = 7,          // 经济学
    LAW = 8,               // 法学
    MEDICINE = 9,          // 医学
    ENGINEERING = 10,      // 工程学
    OTHER = 11             // 其他
};

class Reader : public User {
private:
    Date graduationDate;          // 毕业日期
    Department department;        // 院系
    StudentType studentType;      // 学生类型
    int borrowedCount;           // 借书数目
    double fineAmount;           // 罚款金额

    // 辅助函数：根据学生类型计算毕业日期
    Date calculateGraduationDate(const Date& registrationDate, StudentType type) {
        Date graduation = registrationDate;
        if (type == StudentType::UNDERGRADUATE) {
            graduation.addDays(4 * 365); // 本科生4年
        }
        else {
            graduation.addDays(3 * 365); // 研究生3年
        }
        return graduation;
    }

public:
    Reader(const std::string& uname = "", const std::string& pwd = "",
        const std::string& nm = "", Department dept = Department::OTHER,
        StudentType sType = StudentType::UNDERGRADUATE)
        : User(uname, pwd, nm), department(dept), studentType(sType),
        borrowedCount(0), fineAmount(0.0) {

        // 自动计算毕业日期
        graduationDate = calculateGraduationDate(getRegistrationDate(), studentType);

        std::cout << "创建Reader: " << uname << std::endl;
    }

    virtual ~Reader() {
        std::cout << "销毁Reader: " << getUsername() << std::endl;
    }

    bool get_isadmin() override { return false; }

    // 重写加载和保存函数
    bool loadFromStream(std::istream& file) override {
        try {
            // 先加载基类数据
            if (!User::loadFromStream(file)) return false;

            // 加载毕业日期
            std::string dateStr;
            if (!getline(file, dateStr)) return false;
            if (!dateStr.empty()) {
                size_t pos1 = dateStr.find('/');
                size_t pos2 = dateStr.find('/', pos1 + 1);
                if (pos1 != std::string::npos && pos2 != std::string::npos) {
                    graduationDate = Date(std::stoi(dateStr.substr(0, pos1)),
                        std::stoi(dateStr.substr(pos1 + 1, pos2 - pos1 - 1)),
                        std::stoi(dateStr.substr(pos2 + 1)));
                }
            }

            // 加载院系
            std::string deptStr;
            if (!getline(file, deptStr)) return false;
            department = static_cast<Department>(std::stoi(deptStr));

            // 加载学生类型
            std::string typeStr;
            if (!getline(file, typeStr)) return false;
            studentType = static_cast<StudentType>(std::stoi(typeStr));

            // 加载借书数目
            std::string borrowStr;
            if (!getline(file, borrowStr)) return false;
            borrowedCount = std::stoi(borrowStr);

            // 加载罚款金额
            std::string fineStr;
            if (!getline(file, fineStr)) return false;
            fineAmount = std::stod(fineStr);

            return true;
        }
        catch (...) {
            return false;
        }
    }

    void saveToStream(std::ostream& file) const override {
        // 先保存基类数据
        User::saveToStream(file);

        // 保存Reader特有数据
        file << graduationDate.toString() << '\n';
        file << static_cast<int>(department) << '\n';
        file << static_cast<int>(studentType) << '\n';
        file << borrowedCount << '\n';
        file << fineAmount << '\n';
    }

    // Getter和Setter方法
    Date getGraduationDate() const { return graduationDate; }
    void setGraduationDate(const Date& date) { graduationDate = date; }

    Department getDepartment() const { return department; }
    void setDepartment(Department dept) { department = dept; }

    StudentType getStudentType() const { return studentType; }
    void setStudentType(StudentType type) {
        studentType = type;
        // 重新计算毕业日期
        graduationDate = calculateGraduationDate(getRegistrationDate(), studentType);
    }

    int getBorrowedCount() const { return borrowedCount; }
    void addBorrowedCount()  override{ borrowedCount++ ; }
    void incrementBorrowedCount() { borrowedCount++; }

    double getFineAmount() const { return fineAmount; }
    void setFineAmount(double amount) { fineAmount = amount; }
    void addFine(double amount) { fineAmount += amount; }
    bool payFine(double amount) {
        if (amount <= fineAmount) {
            fineAmount -= amount;
            return true;
        }
        return false;
    }

    // 辅助函数：获取院系名称
    std::string getDepartmentName() const {
        switch (department) {
        case Department::COMPUTER_SCIENCE: return "计算机科学";
        case Department::MATHEMATICS: return "数学";
        case Department::PHYSICS: return "物理";
        case Department::CHEMISTRY: return "化学";
        case Department::BIOLOGY: return "生物";
        case Department::LITERATURE: return "文学";
        case Department::HISTORY: return "历史";
        case Department::ECONOMICS: return "经济学";
        case Department::LAW: return "法学";
        case Department::MEDICINE: return "医学";
        case Department::ENGINEERING: return "工程学";
        case Department::OTHER: return "其他";
        default: return "未知";
        }
    }

    // 辅助函数：获取学生类型名称
    std::string getStudentTypeName() const {
        switch (studentType) {
        case StudentType::UNDERGRADUATE: return "本科生";
        case StudentType::GRADUATE: return "研究生";
        default: return "未知";
        }
    }

    // 显示详细信息
    std::string getDetailedInfo() const {
        std::ostringstream oss;
        oss << "用户名: " << getUsername() << "\n"
            << "姓名: " << getName() << "\n"
            << "注册日期: " << getRegistrationDate().toString() << "\n"
            << "毕业日期: " << graduationDate.toString() << "\n"
            << "院系: " << getDepartmentName() << "\n"
            << "学生类型: " << getStudentTypeName() << "\n"
            << "借书数量: " << borrowedCount << "\n"
            << "罚款金额: " << fineAmount << " 元";
        return oss.str();
    }
};