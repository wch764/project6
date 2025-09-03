#include"bookdetailpage.h"
BookDetailPage::BookDetailPage(const Book& b, std::function<void(const string&)> callback) : book(b),  borrowBookCallback(callback){}
bool BookDetailPage::display() const {
    clearscreen();
    cout << "=== 书籍详细信息 ===\n\n";
    cout << book.tostring() << "\n\n";

    cout << "请选择操作：\n";
    cout << "0. 返回\n";
    cout << "1. 查看评论详情\n";
	cout << "2.借阅本书\n"; 
    cout << "请输入选项：";

    int choice;
    if (validateInt(choice)) {
        switch (choice) {
        case 2:
        {  borrowBookCallback(book.getISBN()); // 调用回调函数
            cout << "借阅操作完成！\n";
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
            cout << "无效选项！\n";
            system("pause");
            break;
        }
    }
}
void BookDetailPage::displayReviews() const {
    
    cout << book.getReviewsDetail() << "\n";
    system("pause");
   
}