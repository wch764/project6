#pragma once
#include<iostream>
#include"Date.h"
#include<string>
#include <iomanip>
#pragma warning(disable : 4996)
using namespace std;
Date::Date(int y,int m , int d ) :year(y), month(m), day(d) {};

bool Date::isValid() const {
    if (month < 1 || month>12)return false;
    if (month == 2) {
        bool leap = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
        return day <= (leap ? 29 : 28);
    }
    int monthDay[] = { 31,28,31,30,31,30,31,31,30,31,30,31 };
    return day <= monthDay[month - 1];
}

bool Date::operator>(const Date& cpr) const {
    if (year != cpr.year)return year > cpr.year;
    if (month != cpr.month)return month > cpr.month;
    return day > cpr.day;
}

bool Date::operator<(const Date& cpr) const {
    return cpr > *this;
}

bool Date::operator==(const Date& cpr)const {
    return this->year == cpr.year && this->month == cpr.month && this->day == cpr.day;
}

int Date::operator-(const Date& cpr)const {
    auto dayInMonth = [](int year, int month)->int {//Lambda函数
        switch (month) {
        case 1: return 31;
        case 2:
            if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0))
                return 29;
            else
                return 28;
        case 3: return 31;
        case 4: return 30;
        case 5: return 31;
        case 6: return 30;
        case 7: return 31;
        case 8: return 31;
        case 9: return 30;
        case 10: return 31;
        case 11: return 30;
        case 12: return 31;
        default: return 0;
        }
        };//返回该年该月的天数
    auto totalday = [dayInMonth](int year, int month, int day)->long long {
        long long days = (year - 1) * 365;
        days += (year - 1) / 4;
        days -= (year - 1) / 100;
        days += (year - 1) / 400;
        for (int i = 1; i < month; i++) {
            days += dayInMonth(year, i);
        }
        days += day;
        return days; };//返回从公元元年1月1日到该日期的天数
    return (totalday(year, month, day) - totalday(cpr.year, cpr.month, cpr.day));

}
// 添加天数
void Date::addDays(int days) {
    if (days < 0) {
        throw std::invalid_argument("Days to add must be non-negative");
    }

    while (days > 0) {
        int daysInCurrentMonth = [](int year, int month)->int {//Lambda函数
            switch (month) {
            case 1: return 31;
            case 2:
                if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0))
                    return 29;
                else
                    return 28;
            case 3: return 31;
            case 4: return 30;
            case 5: return 31;
            case 6: return 30;
            case 7: return 31;
            case 8: return 31;
            case 9: return 30;
            case 10: return 31;
            case 11: return 30;
            case 12: return 31;
            default: return 0;
            }
            }(year, month);//返回该年该月的天数
        int daysRemainingInMonth = daysInCurrentMonth - day + 1;

        if (days >= daysRemainingInMonth) {
            days -= daysRemainingInMonth;
            day = 1;
            month++;
            if (month > 12) {
                month = 1;
                year++;
            }
        }
        else {
            day += days;
            days = 0;
        }
    }
}
 void Date::currentDate() {
    time_t now = time(nullptr);
    tm* ltm = localtime(&now);
    day = ltm->tm_mday;
    month = 1 + ltm->tm_mon;
    year = 1900 + ltm->tm_year;
}
void Date::displayDate()const
{
    cout <<setfill('0') << setw(2) << day << "/"
        << setw(2) << month << "/" << year;
 }
std::string Date::toString()const {
    return std::to_string(year) + "/" + std::to_string(month) + "/" + std::to_string(day);
}

