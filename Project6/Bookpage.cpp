#include<iostream>
#include<string>
#include"Page.h"
#include"Action.h"
#include<vector>
#include"Reader.h"
#include"LoginPage.h"
#include"clearscreen.h"
#include"Book.h"
#include"BorrowInfo.h"
#include"Bookpage.h"
using namespace std;
BookPage::BookPage(vector<Book>& bs, vector<BorrowInfo>& bis, vector<Reader*>& us,User* user, ReservationManager& rm )
        : books(bs), borrowInfos(bis), users(us),currentUser(user), reservationManager(rm) {
    }

bool BookPage::display() const{
        
        bool i = currentUser->get_isadmin();
        if (!i)
        {
            cout << "===图书借阅===" << endl;
            cout << "1. 查看所有图书" << endl;
            cout << "2. 查询图书" << endl;
            cout << "3. 借书/预约图书" << endl;
            cout << "4. 续借" << endl;
            cout << "5. 还书" << endl;
            cout << "6. 返回主菜单" << endl;
        }
        if(i)
        {
            cout << "===图书管理===" << endl;
            cout << "1. 查看所有图书" << endl;
            cout << "2. 查询图书" << endl;
            cout << "3. 编辑图书信息" << endl;
            cout << "4. 返回主菜单" << endl;
        }
        return i;
    }

void  BookPage::performAction() {
    while (true)
    {
        if (!display()) {

            int choice;
            cout << "请选择操作: ";
            if (validateInt(choice))
            {
                switch (choice) {
                case 1:
                    showAllBooks();
                    break;
                case 2:
                    searchbooks();
                    break;
                case 3:
                    borrowBook();
                    break;
                case 4:
                    renewBook();
                    break;
                case 5:
                    returnBook();
                    break;
                case 6:
                    return;
                default:
                    cout << "无效选择!" << endl;
                }
            }
        }
        else
        {
            int choice;
            cout << "请选择操作: ";
            if (validateInt(choice))
            {

                switch (choice) {
                case 1:
                    showAllBooks();
                    break;
                case 2:
                    searchbooks();
                    break;
                case 3:
                    editbooks();
                    break;
                case 4:
                    return;
                default:
                    cout << "无效选择!" << endl;
                }
            }
        }
    }
}
void BookPage::editbooks()
{
    clearscreen();
    cout << "请选择操作" << endl << "1.增加图书" << endl << "2.删除图书" << endl << "3.修改图书信息" << endl<<"4.返回上一页"<<endl;
    int choice;
    if (validateInt(choice))
    {
        switch (choice) {
        case 1:
            addbook();
            return;
        case 2:
            deletebook();
            return;
        case 3:
            modifybook();
            return;
        case 4:
            return;
        default:
            cout << "无效选择!" << endl;
        }
    }

}
void BookPage::addbook(){
    string isbn, title, author, keywords, intro;
    int quantity;

    cout << "输入ISBN: "; getline(cin, isbn);
    cout << "输入书名: "; getline(cin, title);
    cout << "输入作者: "; getline(cin, author);
    cout << "输入关键词(空格分隔): "; getline(cin, keywords);
    cout << "输入简介: "; getline(cin, intro);
    cout << "输入数量: "; cin >> quantity;

    // 创建新图书并加入列表
    books.emplace_back(isbn, title, author, keywords, intro, quantity);
}
void BookPage::deletebook() {
    cout << "输入要删除的图书ISBN: ";
    string isbn;
    getline(cin, isbn);

    // 搜索图书
    auto it = std::find_if(books.begin(), books.end(),
        [&isbn](const Book& b) { return b.getISBN() == isbn; });
    if (it == books.end())
    {
        cout << "未找到该图书" << endl;
        return;
    }

    cout << "当前信息:\n" << it->tostring() << endl;

    // 修改字段
    cout << "是否确认删除 " << endl << "1.是" << endl << "2.否" << endl;
    int choice;
    if (validateInt(choice))
    {
        switch (choice)
        {
        case 1:

            books.erase(it);  // 正确：传入迭代器
            cout << "删除成功！" << endl;
            return;
        case 2:
            return;
        default:
            cout << "无效选择!" << endl;
            return;

        }
    }
}
void BookPage::modifybook() {
    cout << "输入要修改的图书ISBN: ";
    string isbn;
    getline(cin, isbn);

    // 搜索图书
    auto it = find_if(books.begin(), books.end(),
        [&isbn](const Book& b) { return b.getISBN() == isbn; });

    if (it == books.end()) {
        cout << "未找到该图书！" << endl;
        return;
    }

    // 显示当前信息
    cout << "当前信息:\n" << it->tostring() << endl;

    // 选择修改字段
    cout << "选择修改字段:\n"
        << "1. 书名\n2. 作者\n3. 关键词\n4. 简介\n5. 数量\n";
    int choice;
    if (validateInt(choice))
    {
        // 通用修改逻辑
        switch (choice) {
        case 1: { // 修改书名
            cout << "输入新书名: ";
            string newTitle;
            getline(cin, newTitle);
            it->modifyField(&Book::setTitle, newTitle);
            break;
        }
        case 2: { // 修改作者
            cout << "输入新作者: ";
            string newAuthor;
            getline(cin, newAuthor);
            it->modifyField(&Book::setAuthor, newAuthor);
            break;
        }
        case 3: { // 修改关键词
            cout << "输入新关键词（空格分隔）: ";
            string newKeywords;
            getline(cin, newKeywords);

            it->splitkeywords(newKeywords);
            break;
        }
        case 4: { // 修改简介
            cout << "输入新简介: ";
            string newIntro;
            getline(cin, newIntro);
            it->modifyField(&Book::setIntro, newIntro);
            break;
        }
        case 5: { // 修改数量
            cout << "输入新数量: ";
            int newQty;
            cin >> newQty;
            cin.ignore();
            it->modifyField(&Book::setquantity, newQty);
            break;
        }

        default:
            cout << "无效选项！" << endl;
            return;
        }


        cout << "修改成功！" << endl;
    }
}

void  BookPage::showAllBooks() const {
    clearscreen();
    cout << "=== 图书列表 ===" << endl;
    for (const auto& book : books) {
        cout << "ISBN: " << book.getISBN()
            << ", 书名: " << book.getTitle()
            << ", 作者: " << book.getAuthor()
            << ", 总数: " << book.getQuantity()
            << ", 可借: " << book.getAvailable() << endl;
        cout << "关键词：";
        if (book.getkeys().empty()) {
            cout << "(无)";
        }
        else {
            for (const string& key : book.getkeys())
            {
                cout << key<<' ';
            }
        }
            cout << "简介：" << book.getintro() << endl;

        
    }
}
void  BookPage::borrowBook() {
        clearscreen();
        string isbn;
        cout << "输入要借阅的图书ISBN: ";
        getline(cin, isbn);
        auto bookIt = find_if(books.begin(), books.end(),
            [&isbn](const Book& b) { return b.getISBN() == isbn; });//find_if算法加lambda表达式

        if (bookIt != books.end()) {
            if (bookIt->borrowBook()) {
                borrowInfos.emplace_back(currentUser->getUsername(), isbn, false);//在尾直接构造新元素
                cout << "借书成功!" << endl;
            }
            else {
                cout << "该书已全部借出!是否要进行预约(Y/N)" << endl;
                char choice;
                cin >> choice;

                if (choice == 'y' || choice == 'Y') {
                    reserveBook(isbn);
                }
                   /*1. 获取该书所有未归还记录（包括正常借阅和预约）
                    vector<BorrowInfo*> activeRecords;
                    for (auto& info : borrowInfos) {
                        if (info.getBookISBN() == isbn && !info.isReturned()) {
                            activeRecords.push_back(&info);
                        }
                    }

                    // 2. 分离出正常借阅记录（非预约）
                    vector<BorrowInfo*> normalBorrows;
                    vector<BorrowInfo*> existingReservations;

                    for (auto record : activeRecords) {
                        if (record->is_Reservation()) {
                            existingReservations.push_back(record);
                        }
                        else {
                            normalBorrows.push_back(record);
                        }
                    }

                    // 3. 按归还时间排序正常借阅记录
                    sort(normalBorrows.begin(), normalBorrows.end(),
                        [](BorrowInfo* a, BorrowInfo* b) {
                            return a->getDueDate() < b->getDueDate();
                        });

                    // 4. 计算新的预约序号和预计可借日期
                    int newSeq = existingReservations.size() + 1;
                    Date expectedDate;

                    if (normalBorrows.size() >= newSeq) {
                        // 有足够多的正常借阅记录对应预约序号
                        expectedDate = normalBorrows[newSeq - 1]->getDueDate();
                    }
                    else {
                        // 没有足够借阅记录，使用最后一个借阅记录
                        expectedDate = normalBorrows.back()->getDueDate();
                        // 追加缓冲期（如每增加一个预约加3天）
                        expectedDate.addDays((newSeq - normalBorrows.size()) * 3);
                    }

                    expectedDate.addDays(1); // 应还日期次日可借

                    // 5. 创建预约记录
                    borrowInfos.emplace_back(currentUser->getUsername(), isbn);
                    BorrowInfo& newReservation = borrowInfos.back();
                    newReservation.setAsReservation(newSeq, expectedDate);
                    if (reservationManager.addReservation(&newReservation)) {
                        cout << "预约成功！\n";
                        cout << "您的排队序号: " << newSeq << "\n";
                        cout << "预计可借日期: ";
                        expectedDate.displayDate();
                        cout << "\n";
                    }
                    else {
                        cout << "预约失败！\n";
                        borrowInfos.pop_back(); // 移除失败的预约记录
                    }
                    
                }*/
            }
        }
        else {
            cout << "未找到该图书!" << endl;
        }
    }
void BookPage::returnBook() {
    clearscreen();
    string isbn;
    cout << "输入要归还的图书ISBN: ";
    getline(cin, isbn);

    auto bookIt = find_if(books.begin(), books.end(),
        [&isbn](const Book& b) { return b.getISBN() == isbn; });

    if (bookIt == books.end()) {
        cout << "未找到该图书!" << endl;
        return;
    }

    Book& returnedBook = *bookIt;

    // 查找当前用户的借阅记录
    auto borrowIt = find_if(borrowInfos.begin(), borrowInfos.end(),
        [&](const BorrowInfo& bi) {
            return bi.getBookISBN() == isbn &&
                bi.getReaderUsername() == currentUser->getUsername() &&
                !bi.isReturned() &&
                !bi.is_Reservation(); // 确保不是预约记录
        });

    if (borrowIt == borrowInfos.end()) {
        cout << "您没有借阅此图书或已归还!" << endl;
        return;
    }

    // 执行还书操作
    returnedBook.returnBook();
    borrowIt->markReturned();
    cout << "还书成功!" << endl;

    // 检查预约队列
    cout << "检查ISBN " << isbn << " 的预约队列..." << endl;
    size_t nextIndex = reservationManager.peekNextIndex(isbn);

    if (nextIndex != SIZE_MAX && nextIndex < borrowInfos.size()) {
        BorrowInfo& nextReservation = borrowInfos[nextIndex];
        cout << "找到预约记录，预约用户: " << nextReservation.getReaderUsername() << endl;

        // 查找预约用户
        User* reservedUser = finduser(nextReservation.getReaderUsername());
        if (reservedUser != nullptr) {
            cout << "成功找到预约用户，准备发送通知..." << endl;

            // 发送通知给预约用户
            Date expiryDate = Date::currentdate();
            expiryDate.addDays(7); // 7天内确认借阅

            Notification noti(Notification::RESERVATION_AVAILABLE, isbn, findBookTitle(isbn), expiryDate);
            reservedUser->addNotification(noti);

            cout << "已通知预约用户：" << nextReservation.getReaderUsername() << endl;
        }
        else {
            cout << "错误：无法找到预约用户 " << nextReservation.getReaderUsername() << endl;
        }
    }
    else {
        cout << "该书没有预约记录" << endl;
    }
}

            
            /*BorrowInfo* reservation = reservationManager.peekNext(isbn);
            reservationManager.debugPrint();
            if (reservation) {
                cout << 1;
                // 1. 设置图书保留状态
                returnedBook.reserveFor(reservation->getReaderUsername());

                // 2. 发送通知
                User* reservedUser = finduser(reservation->getReaderUsername());
                if (reservedUser) {
                    reservationManager.processReturnedBook(isbn, returnedBook.getTitle(),
                        *reservedUser);

                    cout << "该书已为预约者保留，已发送通知\n";
                }
                else {
                    cout << "错误：预约用户不存在\n";
                }
            }
            else {
                returnedBook.releaseReservation();
            }
        }*/
        
 
void BookPage::renewBook() {
    

    string readerUsername = currentUser->getUsername();
    vector<BorrowInfo*> currentBorrows;

    // 查找当前用户的所有未归还借阅记录
    for (auto& info : borrowInfos) {
        if (info.getReaderUsername() == readerUsername && !info.isReturned()) {
            currentBorrows.push_back(&info);
        }
    }

    if (currentBorrows.empty()) {
        cout << "您当前没有借阅任何图书。\n";
        return;
    }

    // 显示当前借阅列表
    cout << "=== 您的当前借阅 ===\n";
    for (size_t i = 0; i < currentBorrows.size(); i++) {
        const auto& info = *currentBorrows[i];
        cout << i + 1 << ". ISBN: " << info.getBookISBN()
            << ", 书名: " << findBookTitle(info.getBookISBN())
            << ", 应还日期: ";
        info.getDueDate().displayDate();
        cout << ", " << (info.isRenewed() ? "已续借" : "可续借") << "\n";
    }

    // 选择要续借的图书
    cout << "请选择要续借的图书编号 (按0退出): ";
    int choice;
    if (!validateInt(choice)) {
        return;
    }

    if (choice == 0 || choice > currentBorrows.size()) {
        return;
    }

    BorrowInfo& selected = *currentBorrows[choice - 1];

        // 检查是否可以续借
        if (selected.isRenewed()) {
            cout << "每本书只能续借一次。\n";
            return;
        }

    // 检查是否有罚款
    selected.updateFine(Date::currentdate());
    if (selected.getFineAmount() > 0) {
        cout << "该书有未支付的罚款 " << selected.getFineAmount()
            << " 元，请先支付罚款。\n";
        return;
    }

    // 执行续借
    if (selected.renew()) {
        cout << "续借成功！新的应还日期为: ";
        selected.getDueDate().displayDate();
        cout << "\n";
    }
    else {
        cout << "续借失败。\n";
    }
}


