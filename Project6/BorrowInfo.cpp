#include<iostream>
#include<fstream>
#include<string>
#include"Date.h"
#include"Serializable.h"
#include"BorrowInfo.h"
using namespace std;


const double BorrowInfo::DAILY_FINE_RATE = 0.5;
const double BorrowInfo::MAX_FINE_BEFORE_SUSPENSION = 10.0;

    string BorrowInfo::getReaderUsername() const { return readerUsername; }
    string BorrowInfo::getBookISBN() const { return bookISBN; }
    bool BorrowInfo::isReturned() const { return returned; }
    Date BorrowInfo::getBorrowDate() const { return borrowDate; }
    Date BorrowInfo::getReturnDate() const { return returnDate; }
