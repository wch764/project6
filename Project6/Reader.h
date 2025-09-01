
#pragma once
#include "User.h"
#include "Date.h"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

// ����ö������
enum class StudentType {
    UNDERGRADUATE = 0,  // ������
    GRADUATE = 1        // �о���
};

enum class Department {
    COMPUTER_SCIENCE = 0,    // �������ѧ
    MATHEMATICS = 1,         // ��ѧ
    PHYSICS = 2,            // ����
    CHEMISTRY = 3,          // ��ѧ
    BIOLOGY = 4,            // ����
    LITERATURE = 5,         // ��ѧ
    HISTORY = 6,            // ��ʷ
    ECONOMICS = 7,          // ����ѧ
    LAW = 8,               // ��ѧ
    MEDICINE = 9,          // ҽѧ
    ENGINEERING = 10,      // ����ѧ
    OTHER = 11             // ����
};

class Reader : public User {
private:
    Date graduationDate;          // ��ҵ����
    Department department;        // Ժϵ
    StudentType studentType;      // ѧ������
    int borrowedCount;           // ������Ŀ
    double fineAmount;           // ������

    // ��������������ѧ�����ͼ����ҵ����
    Date calculateGraduationDate(const Date& registrationDate, StudentType type) {
        Date graduation = registrationDate;
        if (type == StudentType::UNDERGRADUATE) {
            graduation.addDays(4 * 365); // ������4��
        }
        else {
            graduation.addDays(3 * 365); // �о���3��
        }
        return graduation;
    }

public:
    Reader(const std::string& uname = "", const std::string& pwd = "",
        const std::string& nm = "", Department dept = Department::OTHER,
        StudentType sType = StudentType::UNDERGRADUATE)
        : User(uname, pwd, nm), department(dept), studentType(sType),
        borrowedCount(0), fineAmount(0.0) {

        // �Զ������ҵ����
        graduationDate = calculateGraduationDate(getRegistrationDate(), studentType);

        std::cout << "����Reader: " << uname << std::endl;
    }

    virtual ~Reader() {
        std::cout << "����Reader: " << getUsername() << std::endl;
    }

    bool get_isadmin() override { return false; }

    // ��д���غͱ��溯��
    bool loadFromStream(std::istream& file) override {
        try {
            // �ȼ��ػ�������
            if (!User::loadFromStream(file)) return false;

            // ���ر�ҵ����
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

            // ����Ժϵ
            std::string deptStr;
            if (!getline(file, deptStr)) return false;
            department = static_cast<Department>(std::stoi(deptStr));

            // ����ѧ������
            std::string typeStr;
            if (!getline(file, typeStr)) return false;
            studentType = static_cast<StudentType>(std::stoi(typeStr));

            // ���ؽ�����Ŀ
            std::string borrowStr;
            if (!getline(file, borrowStr)) return false;
            borrowedCount = std::stoi(borrowStr);

            // ���ط�����
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
        // �ȱ����������
        User::saveToStream(file);

        // ����Reader��������
        file << graduationDate.toString() << '\n';
        file << static_cast<int>(department) << '\n';
        file << static_cast<int>(studentType) << '\n';
        file << borrowedCount << '\n';
        file << fineAmount << '\n';
    }

    // Getter��Setter����
    Date getGraduationDate() const { return graduationDate; }
    void setGraduationDate(const Date& date) { graduationDate = date; }

    Department getDepartment() const { return department; }
    void setDepartment(Department dept) { department = dept; }

    StudentType getStudentType() const { return studentType; }
    void setStudentType(StudentType type) {
        studentType = type;
        // ���¼����ҵ����
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

    // ������������ȡԺϵ����
    std::string getDepartmentName() const {
        switch (department) {
        case Department::COMPUTER_SCIENCE: return "�������ѧ";
        case Department::MATHEMATICS: return "��ѧ";
        case Department::PHYSICS: return "����";
        case Department::CHEMISTRY: return "��ѧ";
        case Department::BIOLOGY: return "����";
        case Department::LITERATURE: return "��ѧ";
        case Department::HISTORY: return "��ʷ";
        case Department::ECONOMICS: return "����ѧ";
        case Department::LAW: return "��ѧ";
        case Department::MEDICINE: return "ҽѧ";
        case Department::ENGINEERING: return "����ѧ";
        case Department::OTHER: return "����";
        default: return "δ֪";
        }
    }

    // ������������ȡѧ����������
    std::string getStudentTypeName() const {
        switch (studentType) {
        case StudentType::UNDERGRADUATE: return "������";
        case StudentType::GRADUATE: return "�о���";
        default: return "δ֪";
        }
    }

    // ��ʾ��ϸ��Ϣ
    std::string getDetailedInfo() const {
        std::ostringstream oss;
        oss << "�û���: " << getUsername() << "\n"
            << "����: " << getName() << "\n"
            << "ע������: " << getRegistrationDate().toString() << "\n"
            << "��ҵ����: " << graduationDate.toString() << "\n"
            << "Ժϵ: " << getDepartmentName() << "\n"
            << "ѧ������: " << getStudentTypeName() << "\n"
            << "��������: " << borrowedCount << "\n"
            << "������: " << fineAmount << " Ԫ";
        return oss.str();
    }
};