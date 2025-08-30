#pragma once
#include<iostream>
#include"Date.h"
#include"Serializable.h"
#include<fstream>
using namespace std;
// ������Ϣ��
class BorrowInfo : public Serializable {
private:
    Date borrowDate;
    string readerUsername;
    string bookISBN;
    bool returned;
    Date returnDate;
    bool renewed;  // �Ƿ�������
    Date dueDate;  // Ӧ������
    double fineAmount;  // ������
    bool isReservation;          // �Ƿ���ԤԼ��¼
    int reservationSequence;     // ԤԼ�Ŷ����
    Date expectedAvailableDate;  // Ԥ�ƿɽ�����

public:
    static const int BORROW_PERIOD = 14;  // ����14��
    static const double DAILY_FINE_RATE;  // ÿ�շ�����
    static const double MAX_FINE_BEFORE_SUSPENSION;  // ��ͣ����Ȩ�޵ķ�����ֵ

    BorrowInfo(const string& uname = "", const string& isbn = "", bool ret = false)
        : readerUsername(uname), bookISBN(isbn), returned(ret), renewed(false), fineAmount(0.0), 
    borrowDate(Date::currentdate()),isReservation(false){
       
        dueDate = calculateDueDate(borrowDate);  // ����Ӧ������
    }

    // ����Ӧ������
    static Date calculateDueDate(const Date& borrowDate, bool renewed = false) {
        Date dueDate = borrowDate;
        dueDate.addDays(BORROW_PERIOD * (renewed ? 2 : 1));  // �������28�죬�����14��
        return dueDate;
    }

    // ���㷣����
    double calculateFine(const Date& currentDate) const {
        if (returned || currentDate < dueDate) {
            return 0.0;
        }

        int daysOverdue = currentDate-dueDate;
        return daysOverdue * DAILY_FINE_RATE;
    }

    // ���·�����
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

            // ��ȡӦ������
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

            // ��ȡ�����·�����
            std::string fineStr;
            if (!getline(file, fineStr)) return false;
            fineAmount = stod(fineStr);
            updateFine(Date::currentdate());
            // ��ȡԤԼ״̬
            std::string reserved;
            if (!getline(file, reserved)) return false;
            isReservation = (reserved == "1");

            // ��ȡԤԼ���
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

    // ��ǻ��飬���»���ʱ��
    void markReturned() {
        returned = true;
        returnDate.currentDate();
        fineAmount = calculateFine(returnDate);  // �������շ���
    }

    // ����ͼ��
    bool renew() {
        if (renewed) {
            return false;  // �Ѿ�������������ٴ�����
        }
        renewed = true;
        dueDate = calculateDueDate(borrowDate, true);  // �ӵ�ǰӦ�������ټ�14��
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

      // ����ΪԤԼ��¼
      void setAsReservation(int seq, const Date& availDate) {
          isReservation = true;
          reservationSequence = seq;
          expectedAvailableDate = availDate;
      }
      void converttoborrow() {
          if (!isReservation) {
              cout << "���棺����ת����ԤԼ��¼" << endl;
              return;
          }

          cout << "ת��ԤԼ��¼Ϊ���ļ�¼��" << endl;
          cout << "  - �û�: " << readerUsername << endl;
          cout << "  - ͼ��ISBN: " << bookISBN << endl;
          cout << "  - ԭԤԼ���: " << reservationSequence << endl;

          // ����ԤԼ����ֶ�
          isReservation = false;
          reservationSequence = 0;
          expectedAvailableDate = Date(); // ����ΪĬ������

          // �������ں�Ӧ������Ӧ���ڵ��ô˷���ǰ����
          cout << "  - ת����ɣ��������������ļ�¼" << endl;
      }
      void setReservationSequence(int seq) {
          if (seq >= 0) {  // ��֤�����Ч��
              reservationSequence = seq;
          }
      }
      void setDueDate(const Date& date) {
          dueDate = date;
      }
      void setBorrowDate(const Date& date) {
          borrowDate = date;
      }
      // ����Ԥ�ƿɽ�����
      void setExpectedAvailableDate(const Date& date) {
          expectedAvailableDate = date;
      }

      

      // �Ƿ�������
      bool isRenewed() const { return renewed; }

      // ��ȡӦ������
      Date getDueDate() const { return dueDate; }

      // ��ȡ������
      double getFineAmount() const { return fineAmount; }
};
