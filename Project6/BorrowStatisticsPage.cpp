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
            cout << "=== ����ͳ�� ===" << endl;
            cout << "1. �鿴�������н��ļ�¼" << endl;
            cout << "2. �鿴����δ�����ļ�¼" << endl;
            cout << "3. �������˵�" << endl;
        }
        else {
            cout << "=== ����ͳ�� ===" << endl;
            cout << "1. �鿴���н��ļ�¼" << endl;
            cout << "2. �鿴δ�����ļ�¼" << endl;
            cout << "3. �������˵�" << endl;

        }
        return i;
    }

void BorrowStatisticsPage::performAction() {

    while (true) {
        if (display())
        {
            int choice;
            cout << "��ѡ�����: ";
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
                    cout << "��Чѡ��!" << endl;
                }
            }
        }
        else {
            int choice;
            cout << "��ѡ�����: ";
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
                    cout << "��Чѡ��!" << endl;
                }
            }
        }
    }
}
void BorrowStatisticsPage::showUserRecords() const {
    clearscreen();
    cout << "=== �ҵĽ��ļ�¼ ===" << endl;
    string name = currentUser->getUsername();
    bool found = false;

    for (const auto& info : borrowInfos) {
        if (info.getReaderUsername() ==name) {
            found = true;
            cout << "ͼ��ISBN: " << info.getBookISBN()
                << ", ����: " << findBookTitle(info.getBookISBN())
                << ", �������: ";
            info.getBorrowDate().displayDate();
            if (info.isReturned()) {
                cout << ", �黹����: ";
                info.getReturnDate().displayDate();
            }
            cout << ", ����:" << info.getFineAmount();
            cout << endl;
        }
    }

    if (!found) {
        cout << "û���ҵ����Ľ��ļ�¼��" << endl;
    }
}
void  BorrowStatisticsPage::showUserCurrentBorrows()const
{
    clearscreen();
    cout << "=== �ҵ�δ����¼ ===" << endl;
    string name = currentUser->getUsername();
    bool found = false;

    for (const auto& info : borrowInfos) {
        if (info.getReaderUsername() == name&&!info.isReturned()) {
            found = true;
            cout << "ͼ��ISBN: " << info.getBookISBN()
                << ", ����: " << findBookTitle(info.getBookISBN())
                << ", �������: ";
            info.getBorrowDate().displayDate();
            cout << ", ����:" << info.getFineAmount();
          
            
            cout << endl;
        }
    }

    if (!found) {
        cout << "û��δ��ͼ�顣" << endl;
    }
}
void BorrowStatisticsPage::showAllRecords() const {
        clearscreen();
        cout << "=== ���н��ļ�¼ ===" << endl;
        for (const auto& info : borrowInfos) {//����������Ϣ
            cout << "����: " << info.getReaderUsername()
                << ", ͼ��ISBN: " << info.getBookISBN()
                << ", ����: " << findBookTitle(info.getBookISBN())
                << ", �������: ";
            info.getBorrowDate().displayDate();
            if (info.isReturned()) {
                cout << ", �黹����: ";
                info.getReturnDate().displayDate();
            }
            cout << ", ����:" << info.getFineAmount();
            cout << endl;
        }
    }

void BorrowStatisticsPage::showCurrentAllBorrows()const{
        clearscreen();
        cout << "=== ��ǰ�ڽ� ===" << endl;
        for (const auto& info : borrowInfos) {
            if (!info.isReturned()) {
                cout << "����: " << info.getReaderUsername()
                    << ", ͼ��ISBN: " << info.getBookISBN()
                    << ", ����: " << findBookTitle(info.getBookISBN())
                    << ", �������: ";
                info.getBorrowDate().displayDate();
                cout << ", ����:" << info.getFineAmount();
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
        return "δ֪ͼ��";
    }
