#pragma once
#include<iostream>
#include"Date.h"
#include"Serializable.h"
#include<fstream>
using namespace std;
// 借阅信息类
class BorrowInfo : public Serializable {
private:
    Date borrowDate;
    string readerUsername;
    string bookISBN;
    bool returned;
    Date returnDate;
    bool renewed;  // 是否已续借
    Date dueDate;  // 应还日期
    double fineAmount;  // 罚款金额
    bool isReservation;          // 是否是预约记录
    int reservationSequence;     // 预约排队序号
    Date expectedAvailableDate;  // 预计可借日期

public:
    static const int BORROW_PERIOD = 14;  // 借期14天
    static const double DAILY_FINE_RATE;  // 每日罚款率
    static const double MAX_FINE_BEFORE_SUSPENSION;  // 暂停借书权限的罚款阈值

    BorrowInfo(const string& uname = "", const string& isbn = "", bool ret = false)
        : readerUsername(uname), bookISBN(isbn), returned(ret), renewed(false), fineAmount(0.0), 
    borrowDate(Date::currentdate()),isReservation(false){
       
        dueDate = calculateDueDate(borrowDate);  // 计算应还日期
    }

    // 计算应还日期
    static Date calculateDueDate(const Date& borrowDate, bool renewed = false) {
        Date dueDate = borrowDate;
        dueDate.addDays(BORROW_PERIOD * (renewed ? 2 : 1));  // 续借则加28天，否则加14天
        return dueDate;
    }

    // 计算罚款金额
    double calculateFine(const Date& currentDate) const {
        if (returned || currentDate < dueDate) {
            return 0.0;
        }

        int daysOverdue = currentDate-dueDate;
        return daysOverdue * DAILY_FINE_RATE;
    }

    // 更新罚款金额
    void updateFine(const Date& currentDate) {
        fineAmount = calculateFine(currentDate);
    }

    bool loadFromStream(std::istream& file) override {
        try {
            if (!getline(file, readerUsername)) return false;
            if (!getline(file, bookISBN)) return false;

            std::string status;
            if (!getline(file, status)) return false;
            returned = (status == "1");

            std::string renewedStatus;
            if (!getline(file, renewedStatus)) return false;
            renewed = (renewedStatus == "1");

            std::string dateStr;
            if (!getline(file, dateStr)) return false;
            if (!dateStr.empty()) {
                size_t pos1 = dateStr.find('/');
                if (pos1 == string::npos) return false;

                size_t pos2 = dateStr.find('/', pos1 + 1);
                if (pos2 == string::npos) return false;

                borrowDate = Date(stoi(dateStr.substr(0, pos1)),
                    stoi(dateStr.substr(pos1 + 1, pos2 - pos1 - 1)),
                    stoi(dateStr.substr(pos2 + 1)));
            }

            // 读取应还日期
            if (!getline(file, dateStr)) return false;
            if (!dateStr.empty()) {
                size_t pos1 = dateStr.find('/');
                if (pos1 == string::npos) return false;

                size_t pos2 = dateStr.find('/', pos1 + 1);
                if (pos2 == string::npos) return false;

                dueDate = Date(stoi(dateStr.substr(0, pos1)),
                    stoi(dateStr.substr(pos1 + 1, pos2 - pos1 - 1)),
                    stoi(dateStr.substr(pos2 + 1)));
            }

            // 读取并更新罚款金额
            std::string fineStr;
            if (!getline(file, fineStr)) return false;
            fineAmount = stod(fineStr);
            updateFine(Date::currentdate());
            // 读取预约状态
            std::string reserved;
            if (!getline(file, reserved)) return false;
            isReservation = (reserved == "1");

            // 读取预约序号
            std::string seqStr;
            if (isReservation)
            {
                if (!getline(file, seqStr)) return false;
                reservationSequence = stoi(seqStr);
                if (!getline(file, dateStr)) return false;
                if (!dateStr.empty()) {
                    size_t pos1 = dateStr.find('/');
                    size_t pos2 = dateStr.find('/', pos1 + 1);
                    expectedAvailableDate = Date(stoi(dateStr.substr(0, pos1)),
                        stoi(dateStr.substr(pos1 + 1, pos2 - pos1 - 1)),
                        stoi(dateStr.substr(pos2 + 1)));
                }
            }
            if (returned) {
                if (!getline(file, dateStr)) return false;
                if (!dateStr.empty()) {
                    size_t pos1 = dateStr.find('/');
                    if (pos1 == string::npos) return false;

                    size_t pos2 = dateStr.find('/', pos1 + 1);
                    if (pos2 == string::npos) return false;

                    returnDate = Date(stoi(dateStr.substr(0, pos1)),
                        stoi(dateStr.substr(pos1 + 1, pos2 - pos1 - 1)),
                        stoi(dateStr.substr(pos2 + 1)));
                }
            }
            return true;
        }
        catch (...) {
            return false;
        }
    }

    void saveToStream(ostream& file) const override {
        file << readerUsername << '\n'
            << bookISBN << '\n'
            << (returned ? "1" : "0") << '\n'
            << (renewed ? "1" : "0") << '\n'
            << borrowDate.toString() << '\n'
            << dueDate.toString() << '\n'
            << fineAmount << '\n'
            << isReservation << endl;
        if(isReservation)
        {
            file << reservationSequence << '\n'
                << expectedAvailableDate.toString() << '\n';
        }

        if (returned) {
            file << returnDate.toString() << '\n';
        }
    }

    void loadFromFile(const string& filename) override {
        ifstream file(filename);
        loadFromStream(file);
    }

    void saveToFile(const string& filename) const override {
        ofstream file(filename, ios::app);
        saveToStream(file);
    }

    // 标记还书，更新还书时间
    void markReturned() {
        returned = true;
        returnDate.currentDate();
        fineAmount = calculateFine(returnDate);  // 计算最终罚款
    }

    // 续借图书
    bool renew() {
        if (renewed) {
            return false;  // 已经续借过，不能再次续借
        }
        renewed = true;
        dueDate = calculateDueDate(borrowDate, true);  // 从当前应还日期再加14天
        return true;
    }
      
      string getReaderUsername()const;
      string getBookISBN() const;
      bool isReturned() const;
      Date getBorrowDate() const;
      Date getReturnDate() const;
      bool is_Reservation() const { return isReservation; }
      int getReservationSequence() const { return reservationSequence; }
      Date getExpectedAvailableDate() const { return expectedAvailableDate; }

      // 设置为预约记录
      void setAsReservation(int seq, const Date& availDate) {
          isReservation = true;
          reservationSequence = seq;
          expectedAvailableDate = availDate;
      }
      void converttoborrow() {
          if (!isReservation) {
              cout << "警告：尝试转换非预约记录" << endl;
              return;
          }

          cout << "转换预约记录为借阅记录：" << endl;
          cout << "  - 用户: " << readerUsername << endl;
          cout << "  - 图书ISBN: " << bookISBN << endl;
          cout << "  - 原预约序号: " << reservationSequence << endl;

          // 重置预约相关字段
          isReservation = false;
          reservationSequence = 0;
          expectedAvailableDate = Date(); // 重置为默认日期

          // 借阅日期和应还日期应该在调用此方法前设置
          cout << "  - 转换完成，现在是正常借阅记录" << endl;
      }
      void setReservationSequence(int seq) {
          if (seq >= 0) {  // 验证序号有效性
              reservationSequence = seq;
          }
      }
      void setDueDate(const Date& date) {
          dueDate = date;
      }
      void setBorrowDate(const Date& date) {
          borrowDate = date;
      }
      // 设置预计可借日期
      void setExpectedAvailableDate(const Date& date) {
          expectedAvailableDate = date;
      }

      

      // 是否已续借
      bool isRenewed() const { return renewed; }

      // 获取应还日期
      Date getDueDate() const { return dueDate; }

      // 获取罚款金额
      double getFineAmount() const { return fineAmount; }
};
