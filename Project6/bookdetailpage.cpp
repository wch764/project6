#include"bookdetailpage.h"
BookDetailPage::BookDetailPage(const Book& b, std::function<void(const string&)> callback) : book(b),  borrowBookCallback(callback){}
bool BookDetailPage::display() const {
    clearscreen();
    cout << "=== �鼮��ϸ��Ϣ ===\n\n";
    cout << book.tostring() << "\n\n";

    cout << "��ѡ�������\n";
    cout << "0. ����\n";
    cout << "1. �鿴��������\n";
	cout << "2.���ı���\n"; 
    cout << "������ѡ�";

    int choice;
    if (validateInt(choice)) {
        switch (choice) {
        case 2:
        {  borrowBookCallback(book.getISBN()); // ���ûص�����
            cout << "���Ĳ�����ɣ�\n";
            system("pause");
            return display();
        }
        case 1:
            displayReviews();
            return display();
        case 0:
			clearscreen();
            return 0;
        default:
            cout << "��Чѡ�\n";
            system("pause");
            break;
        }
    }
}
void BookDetailPage::displayReviews() const {
    
    cout << book.getReviewsDetail() << "\n";
    system("pause");
   
}