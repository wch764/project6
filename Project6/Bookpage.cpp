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
#include"systemlogger.h"
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
				currentUser->addBorrowedCount();
                cout << "借书成功!" << endl;
            }
            else {
                cout << "该书已全部借出!是否要进行预约(Y/N)" << endl;
                char choice;
                cin >> choice;

                if (choice == 'y' || choice == 'Y') {
                    reserveBook(isbn);
                }
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
        LOG_ERROR("RETURN", "尝试归还不存在的图书: " + isbn);
        return;
    }

    Book& returnedBook = *bookIt;

    // 查找当前用户的借阅记录
    auto borrowIt = find_if(borrowInfos.begin(), borrowInfos.end(),
        [&](const BorrowInfo& bi) {
            return bi.getBookISBN() == isbn &&
                bi.getReaderUsername() == currentUser->getUsername() &&
                !bi.isReturned() &&
                !bi.is_Reservation();
        });

    if (borrowIt == borrowInfos.end()) {
        cout << "您没有借阅此图书或已归还!" << endl;
        return;
    }

    // 执行还书操作
    returnedBook.returnBook();
    const_cast<BorrowInfo&>(*borrowIt).markReturned();

    LOG_INFO("RETURN", "用户 " + currentUser->getUsername() + " 归还图书 " + isbn + " " + returnedBook.getTitle());
    cout << "还书成功!" << endl;

    // 询问是否要评分
    cout << "\n是否要对这本书进行评分和评论？(y/n): ";
    char choice;
    cin >> choice;
    cin.ignore();

    if (choice == 'y' || choice == 'Y') {
        addBookReview(returnedBook);
    }

    // 检查预约队列（保持原有逻辑）
    cout << "检查ISBN " << isbn << " 的预约队列..." << endl;
    size_t nextIndex = reservationManager.peekNextIndex(isbn);

    if (nextIndex != SIZE_MAX && nextIndex < borrowInfos.size()) {
        BorrowInfo& nextReservation = borrowInfos[nextIndex];
        cout << "找到预约记录，预约用户: " << nextReservation.getReaderUsername() << endl;

        User* reservedUser = finduser(nextReservation.getReaderUsername());
        if (reservedUser != nullptr) {
            Date expiryDate = Date::currentdate();
            expiryDate.addDays(7);

            Notification noti(Notification::RESERVATION_AVAILABLE, isbn, findBookTitle(isbn), expiryDate);
            reservedUser->addNotification(noti);

            cout << "已通知预约用户：" << nextReservation.getReaderUsername() << endl;
            LOG_INFO("NOTIFICATION", "预约通知发送给用户: " + nextReservation.getReaderUsername());
        }
    }
}
void BookPage::addBookReview(Book& book) {
    cout << "=== 图书评分 ===" << endl;
    cout << "图书: " << book.getTitle() << " by " << book.getAuthor() << endl;

    int rating;
    string comment;

    cout << "请给这本书评分 (1-5星): ";
    if (!validateInt(rating) || rating < 1 || rating > 5) {
        cout << "无效评分，跳过评价。" << endl;
        return;
    }

    cout << "请写下您的评论 (可选，直接回车跳过): ";
    getline(cin, comment);

    // 添加评论到图书
    book.addReview(currentUser->getUsername(), rating, comment);

    LOG_INFO("REVIEW", "用户 " + currentUser->getUsername() + " 对图书 " + book.getISBN() + " 评分: " + to_string(rating) + "星");

    cout << "感谢您的评价！" << endl;
    cout << "当前平均评分: " << fixed << setprecision(1) << book.getAverageRating() << " 星" << endl;
}
// 按分类显示图书
void BookPage::showBooksByCategory() const {
    clearscreen();
    cout << "=== 按分类查看图书 ===" << endl;
    cout << "请选择要查看的分类:" << endl;
    cout << "0. 计算机科学、知识学、总论" << endl;
    cout << "1. 哲学、心理学" << endl;
    cout << "2. 宗教" << endl;
    cout << "3. 社会科学" << endl;
    cout << "4. 语言" << endl;
    cout << "5. 自然科学、数学" << endl;
    cout << "6. 技术（应用科学）" << endl;
    cout << "7. 艺术、娱乐、体育" << endl;
    cout << "8. 文学" << endl;
    cout << "9. 历史、传记、地理" << endl;
    cout << "10. 查看所有图书" << endl;
    cout << "11. 返回" << endl;

    int categoryChoice;
    cout << "请选择 (0-11): ";
    if (!validateInt(categoryChoice) || categoryChoice < 0 || categoryChoice > 11) {
        cout << "无效选择!" << endl;
        return;
    }

    if (categoryChoice == 11) return;

    clearscreen();

    if (categoryChoice == 10) {
        // 显示所有图书
        showAllBooks();
    }
    else {
        // 显示特定分类的图书
        BookCategory category = static_cast<BookCategory>(categoryChoice);
        showCategoryBooks(category);
    }
}
// 显示特定分类的图书
void BookPage::showCategoryBooks(BookCategory category) const {
    // 获取分类名称
    string categoryName;
    switch (category) {
    case BookCategory::COMPUTER_SCIENCE_INFORMATION: categoryName = "计算机科学、知识学、总论"; break;
    case BookCategory::PHILOSOPHY_PSYCHOLOGY: categoryName = "哲学、心理学"; break;
    case BookCategory::RELIGION: categoryName = "宗教"; break;
    case BookCategory::SOCIAL_SCIENCES: categoryName = "社会科学"; break;
    case BookCategory::LANGUAGE: categoryName = "语言"; break;
    case BookCategory::NATURAL_SCIENCES_MATHEMATICS: categoryName = "自然科学、数学"; break;
    case BookCategory::TECHNOLOGY_APPLIED_SCIENCES: categoryName = "技术（应用科学）"; break;
    case BookCategory::THE_ARTS_RECREATION: categoryName = "艺术、娱乐、体育"; break;
    case BookCategory::LITERATURE_RHETORIC: categoryName = "文学"; break;
    case BookCategory::HISTORY_GEOGRAPHY: categoryName = "历史、传记、地理"; break;
    default: categoryName = "未知分类"; break;
    }

    cout << "=== " << categoryName << " 类图书 ===" << endl;

    bool found = false;
    for (const auto& book : books) {
        if (book.getCategory() == category) {
            found = true;
            cout << "ISBN: " << book.getISBN()
                << ", 书名: " << book.getTitle()
                << ", 作者: " << book.getAuthor()
                << ", 总数: " << book.getQuantity()
                << ", 可借: " << book.getAvailable()
                << ", 总借阅: " << book.getTotalBorrowCount();

            if (book.getReviewCount() > 0) {
                cout << ", 评分: " << fixed << setprecision(1) << book.getAverageRating() << "星";
            }
            cout << endl;

            // 显示关键词和简介
            cout << "关键词: ";
            if (book.getkeys().empty()) {
                cout << "(无)";
            }
            else {
                for (const string& key : book.getkeys()) {
                    cout << key << " ";
                }
            }
            cout << endl;
            cout << "简介: " << book.getintro() << endl;
            cout << "----------------------------" << endl;
        }
    }

    if (!found) {
        cout << "该分类下暂无图书。" << endl;
    }
}
        
 
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
void BookPage::searchBooksWithCategory() const {
    cout << "是否要限定搜索的图书分类？(y/n): ";
    char useCategory;
    cin >> useCategory;
    cin.ignore();

    BookCategory selectedCategory;
    bool filterByCategory = false;

    if (useCategory == 'y' || useCategory == 'Y') {
        cout << "请选择要搜索的分类:" << endl;
        cout << "0. 计算机科学、知识学、总论" << endl;
        cout << "1. 哲学、心理学" << endl;
        cout << "2. 宗教" << endl;
        cout << "3. 社会科学" << endl;
        cout << "4. 语言" << endl;
        cout << "5. 自然科学、数学" << endl;
        cout << "6. 技术（应用科学）" << endl;
        cout << "7. 艺术、娱乐、体育" << endl;
        cout << "8. 文学" << endl;
        cout << "9. 历史、传记、地理" << endl;

        int categoryChoice;
        cout << "请选择分类 (0-9): ";
        if (validateInt(categoryChoice) && categoryChoice >= 0 && categoryChoice <= 9) {
            selectedCategory = static_cast<BookCategory>(categoryChoice);
            filterByCategory = true;
        }
        else {
            cout << "无效选择，将在所有分类中搜索。" << endl;
        }
    }

    // 执行原有的搜索逻辑，但添加分类筛选
    cout << "请选择搜索方式：\n";
    cout << "1. 精确搜索（完全匹配）\n";
    cout << "2. 模糊搜索（相似度匹配）\n";
    cout << "请输入选项：";

    int searchType;
    if (validateInt(searchType)) {
        if (searchType != 1 && searchType != 2) {
            cerr << "无效输入" << endl;
            return;
        }
        clearscreen();
        cout << "请选择搜索字段：\n";
        cout << "1. ISBN\n";
        cout << "2. 书名\n";
        cout << "3. 作者\n";
        cout << "4. 关键词\n";
        cout << "5. 简介\n";
        cout << "请输入选项：";
        int fieldChoice;

        if (validateInt(fieldChoice, "请输入选项：")) {
            if (fieldChoice < 1 || fieldChoice > 5) {
                cerr << "无效选项！" << endl;
                return;
            }
            cout << "请输入搜索内容：";
            cin.ignore();
            string query;
            getline(cin, query);

            vector<Book> results;

            // 使用带分类筛选的搜索方法
            switch (fieldChoice) {
            case 1: // ISBN
                if (searchType == 1) {
                    results = exactSearchWithCategory(query, [](const Book& b) { return b.getISBN(); }, filterByCategory, selectedCategory);
                }
                else {
                    results = fuzzySearchWithCategory(query, [](const Book& b) { return b.getISBN(); }, filterByCategory, selectedCategory);
                }
                break;
            case 2: // 书名
                if (searchType == 1) {
                    results = exactSearchWithCategory(query, [](const Book& b) { return b.getTitle(); }, filterByCategory, selectedCategory);
                }
                else {
                    results = fuzzySearchWithCategory(query, [](const Book& b) { return b.getTitle(); }, filterByCategory, selectedCategory);
                }
                break;
            case 3: // 作者
                if (searchType == 1) {
                    results = exactSearchWithCategory(query, [](const Book& b) { return b.getAuthor(); }, filterByCategory, selectedCategory);
                }
                else {
                    results = fuzzySearchWithCategory(query, [](const Book& b) { return b.getAuthor(); }, filterByCategory, selectedCategory);
                }
                break;
            case 4: // 关键词
                if (searchType == 1) {
                    results = exactSearchWithCategory(query, [](const Book& b) {
                        string keywordsStr;
                        for (const auto& kw : b.getkeys()) {
                            keywordsStr += kw + " ";
                        }
                        return keywordsStr;
                        }, filterByCategory, selectedCategory);
                }
                else {
                    results = fuzzySearchWithCategory(query, [](const Book& b) {
                        string keywordsStr;
                        for (const auto& kw : b.getkeys()) {
                            keywordsStr += kw + " ";
                        }
                        return keywordsStr;
                        }, filterByCategory, selectedCategory);
                }
                break;
            case 5: // 简介
                if (searchType == 1) {
                    results = exactSearchWithCategory(query, [](const Book& b) { return b.getintro(); }, filterByCategory, selectedCategory);
                }
                else {
                    results = fuzzySearchWithCategory(query, [](const Book& b) { return b.getintro(); }, filterByCategory, selectedCategory);
                }
                break;
            default:
                cerr << "无效选项！\n";
                return;
            }

            // 显示搜索结果
            if (results.empty()) {
                cout << "未找到匹配的书籍。\n";
            }
            else {
                cout << "找到 " << results.size() << " 本书：\n";
                for (const Book& book : results) {
                    cout << "ISBN: " << book.getISBN() << "\n";
                    cout << "书名: " << book.getTitle() << "\n";
                    cout << "作者: " << book.getAuthor() << "\n";
                    cout << "分类: " << book.getCategoryName() << "\n";
                    cout << "可借数量: " << book.getAvailable() << "\n";
                    cout << "总借阅次数: " << book.getTotalBorrowCount() << "\n";

                    if (book.getReviewCount() > 0) {
                        cout << "平均评分: " << fixed << setprecision(1) << book.getAverageRating() << " 星 (" << book.getReviewCount() << " 条评论)\n";
                    }
                    else {
                        cout << "暂无评分\n";
                    }
                    cout << "----------------------------\n";
                }
            }
        }
    }
}
// 带分类筛选的精确搜索
template<typename FieldGetter>
vector<Book> BookPage::exactSearchWithCategory(
    const string& query,
    FieldGetter fieldGetter,
    bool filterByCategory,
    BookCategory category
) const {
    vector<Book> results;
    for (const Book& book : books) {
        // 分类筛选
        if (filterByCategory && book.getCategory() != category) {
            continue;
        }

        string fieldValue = fieldGetter(book);
        if (match(query, fieldValue)) {
            results.push_back(book);
        }
    }

    // 按借阅量降序排序
    sort(results.begin(), results.end(),
        [](const Book& a, const Book& b) {
            return a.getBorrowed() > b.getBorrowed();
        });

    return results;
}

// 带分类筛选的模糊搜索
template<typename FieldGetter>
vector<Book> BookPage::fuzzySearchWithCategory(
    const string& query,
    FieldGetter fieldGetter,
    bool filterByCategory,
    BookCategory category
) const {
    vector<pair<size_t, Book>> scoredBooks;

    for (const Book& book : books) {
        // 分类筛选
        if (filterByCategory && book.getCategory() != category) {
            continue;
        }

        const string fieldValue = fieldGetter(book);
        size_t score = countCommonTokens(query, fieldValue);
        if (score > 0) {
            scoredBooks.emplace_back(score, book);
        }
    }

    // 按分数降序排序
    sort(scoredBooks.begin(), scoredBooks.end(),
        [](const auto& a, const auto& b) {
            return a.first > b.first;
        });

    vector<Book> results;
    for (const auto& scoredBook : scoredBooks) {
        results.push_back(scoredBook.second);
    }

    return results;
}


