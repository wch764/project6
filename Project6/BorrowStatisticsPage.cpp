#include<iostream>
#include<string>
#include"Page.h"
#include"Action.h"
#include<vector>
#include"Reader.h"
#include"BorrowStatisticsPage.h"
#include"clearscreen.h"
#include"Book.h"
#include"BorrowInfo.h"
#include"failmanage.h"
using namespace std;
BorrowStatisticsPage::BorrowStatisticsPage(vector<BorrowInfo>& bis, vector<Book>& bs,User*u)
        : borrowInfos(bis), books(bs),currentUser(u) {
    }

bool BorrowStatisticsPage::display() const{
        bool i = currentUser->get_isadmin();
        if (!i)
        {
            cout << "=== 借阅统计 ===" << endl;
            cout << "1. 查看本人所有借阅记录" << endl;
            cout << "2. 查看本人未还借阅记录" << endl;
            cout << "3. 返回主菜单" << endl;
        }
        else {
            cout << "=== 借阅统计 ===" << endl;
            cout << "1. 查看所有借阅记录" << endl;
            cout << "2. 查看未还借阅记录" << endl;
            cout << "3. 返回主菜单" << endl;

        }
        return i;
    }

void BorrowStatisticsPage::performAction() {

    while (true) {
        if (display())
        {
            int choice;
            cout << "请选择操作: ";
            if (validateInt(choice))
            {
                switch (choice) {
                case 1:
                    showAllRecords();
                    break;
                case 2:
                    showCurrentAllBorrows();
                    break;
                case 3:
                    return;
                default:
                    cout << "无效选择!" << endl;
                }
            }
        }
        else {
            int choice;
            cout << "请选择操作: ";
            if (validateInt(choice))
            {
                switch (choice) {
                case 1:
                    showUserRecords();
                    break;
                case 2:
                    showUserCurrentBorrows();
                    break;
                case 3:
                    return;
                default:
                    cout << "无效选择!" << endl;
                }
            }
        }
    }
}
void BorrowStatisticsPage::showUserRecords() const {
    clearscreen();
    cout << "=== 我的借阅记录 ===" << endl;
    string name = currentUser->getUsername();
    bool found = false;

    for (const auto& info : borrowInfos) {
        if (info.getReaderUsername() ==name) {
            found = true;
            cout << "图书ISBN: " << info.getBookISBN()
                << ", 书名: " << findBookTitle(info.getBookISBN())
                << ", 借出日期: ";
            info.getBorrowDate().displayDate();
            if (info.isReturned()) {
                cout << ", 归还日期: ";
                info.getReturnDate().displayDate();
            }
            cout << ", 罚款:" << info.getFineAmount();
            cout << endl;
        }
    }

    if (!found) {
        cout << "没有找到您的借阅记录。" << endl;
    }
}
void  BorrowStatisticsPage::showUserCurrentBorrows()const
{
    clearscreen();
    cout << "=== 我的未还记录 ===" << endl;
    string name = currentUser->getUsername();
    bool found = false;

    for (const auto& info : borrowInfos) {
        if (info.getReaderUsername() == name&&!info.isReturned()) {
            found = true;
            cout << "图书ISBN: " << info.getBookISBN()
                << ", 书名: " << findBookTitle(info.getBookISBN())
                << ", 借出日期: ";
            info.getBorrowDate().displayDate();
            cout << ", 罚款:" << info.getFineAmount();
          
            
            cout << endl;
        }
    }

    if (!found) {
        cout << "没有未还图书。" << endl;
    }
}
void BorrowStatisticsPage::showAllRecords() const {
        clearscreen();
        cout << "=== 所有借阅记录 ===" << endl;
        for (const auto& info : borrowInfos) {//遍历借阅信息
            cout << "读者: " << info.getReaderUsername()
                << ", 图书ISBN: " << info.getBookISBN()
                << ", 书名: " << findBookTitle(info.getBookISBN())
                << ", 借出日期: ";
            info.getBorrowDate().displayDate();
            if (info.isReturned()) {
                cout << ", 归还日期: ";
                info.getReturnDate().displayDate();
            }
            cout << ", 罚款:" << info.getFineAmount();
            cout << endl;
        }
    }

void BorrowStatisticsPage::showCurrentAllBorrows()const{
        clearscreen();
        cout << "=== 当前在借 ===" << endl;
        for (const auto& info : borrowInfos) {
            if (!info.isReturned()) {
                cout << "读者: " << info.getReaderUsername()
                    << ", 图书ISBN: " << info.getBookISBN()
                    << ", 书名: " << findBookTitle(info.getBookISBN())
                    << ", 借出日期: ";
                info.getBorrowDate().displayDate();
                cout << ", 罚款:" << info.getFineAmount();
                cout << endl;
            }
        }
    }

string BorrowStatisticsPage::findBookTitle(const string& isbn) const {
         
        for (const auto& book : books) {
            if (book.getISBN() == isbn) {
                return book.getTitle();
            }
        }
        return "未知图书";
    }
