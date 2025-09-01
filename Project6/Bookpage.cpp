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
            cout << "===ͼ�����===" << endl;
            cout << "1. �鿴����ͼ��" << endl;
            cout << "2. ��ѯͼ��" << endl;
            cout << "3. ����/ԤԼͼ��" << endl;
            cout << "4. ����" << endl;
            cout << "5. ����" << endl;
            cout << "6. �������˵�" << endl;
        }
        if(i)
        {
            cout << "===ͼ�����===" << endl;
            cout << "1. �鿴����ͼ��" << endl;
            cout << "2. ��ѯͼ��" << endl;
            cout << "3. �༭ͼ����Ϣ" << endl;
            cout << "4. �������˵�" << endl;
        }
        return i;
    }

void  BookPage::performAction() {
    while (true)
    {
        if (!display()) {

            int choice;
            cout << "��ѡ�����: ";
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
                    cout << "��Чѡ��!" << endl;
                }
            }
        }
        else
        {
            int choice;
            cout << "��ѡ�����: ";
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
                    cout << "��Чѡ��!" << endl;
                }
            }
        }
    }
}
void BookPage::editbooks()
{
    clearscreen();
    cout << "��ѡ�����" << endl << "1.����ͼ��" << endl << "2.ɾ��ͼ��" << endl << "3.�޸�ͼ����Ϣ" << endl<<"4.������һҳ"<<endl;
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
            cout << "��Чѡ��!" << endl;
        }
    }

}
void BookPage::addbook(){
    string isbn, title, author, keywords, intro;
    int quantity;

    cout << "����ISBN: "; getline(cin, isbn);
    cout << "��������: "; getline(cin, title);
    cout << "��������: "; getline(cin, author);
    cout << "����ؼ���(�ո�ָ�): "; getline(cin, keywords);
    cout << "������: "; getline(cin, intro);
    cout << "��������: "; cin >> quantity;

    // ������ͼ�鲢�����б�
    books.emplace_back(isbn, title, author, keywords, intro, quantity);
}
void BookPage::deletebook() {
    cout << "����Ҫɾ����ͼ��ISBN: ";
    string isbn;
    getline(cin, isbn);

    // ����ͼ��
    auto it = std::find_if(books.begin(), books.end(),
        [&isbn](const Book& b) { return b.getISBN() == isbn; });
    if (it == books.end())
    {
        cout << "δ�ҵ���ͼ��" << endl;
        return;
    }

    cout << "��ǰ��Ϣ:\n" << it->tostring() << endl;

    // �޸��ֶ�
    cout << "�Ƿ�ȷ��ɾ�� " << endl << "1.��" << endl << "2.��" << endl;
    int choice;
    if (validateInt(choice))
    {
        switch (choice)
        {
        case 1:

            books.erase(it);  // ��ȷ�����������
            cout << "ɾ���ɹ���" << endl;
            return;
        case 2:
            return;
        default:
            cout << "��Чѡ��!" << endl;
            return;

        }
    }
}
void BookPage::modifybook() {
    cout << "����Ҫ�޸ĵ�ͼ��ISBN: ";
    string isbn;
    getline(cin, isbn);

    // ����ͼ��
    auto it = find_if(books.begin(), books.end(),
        [&isbn](const Book& b) { return b.getISBN() == isbn; });

    if (it == books.end()) {
        cout << "δ�ҵ���ͼ�飡" << endl;
        return;
    }

    // ��ʾ��ǰ��Ϣ
    cout << "��ǰ��Ϣ:\n" << it->tostring() << endl;

    // ѡ���޸��ֶ�
    cout << "ѡ���޸��ֶ�:\n"
        << "1. ����\n2. ����\n3. �ؼ���\n4. ���\n5. ����\n";
    int choice;
    if (validateInt(choice))
    {
        // ͨ���޸��߼�
        switch (choice) {
        case 1: { // �޸�����
            cout << "����������: ";
            string newTitle;
            getline(cin, newTitle);
            it->modifyField(&Book::setTitle, newTitle);
            break;
        }
        case 2: { // �޸�����
            cout << "����������: ";
            string newAuthor;
            getline(cin, newAuthor);
            it->modifyField(&Book::setAuthor, newAuthor);
            break;
        }
        case 3: { // �޸Ĺؼ���
            cout << "�����¹ؼ��ʣ��ո�ָ���: ";
            string newKeywords;
            getline(cin, newKeywords);

            it->splitkeywords(newKeywords);
            break;
        }
        case 4: { // �޸ļ��
            cout << "�����¼��: ";
            string newIntro;
            getline(cin, newIntro);
            it->modifyField(&Book::setIntro, newIntro);
            break;
        }
        case 5: { // �޸�����
            cout << "����������: ";
            int newQty;
            cin >> newQty;
            cin.ignore();
            it->modifyField(&Book::setquantity, newQty);
            break;
        }

        default:
            cout << "��Чѡ�" << endl;
            return;
        }


        cout << "�޸ĳɹ���" << endl;
    }
}

void  BookPage::showAllBooks() const {
    clearscreen();
    cout << "=== ͼ���б� ===" << endl;
    for (const auto& book : books) {
        cout << "ISBN: " << book.getISBN()
            << ", ����: " << book.getTitle()
            << ", ����: " << book.getAuthor()
            << ", ����: " << book.getQuantity()
            << ", �ɽ�: " << book.getAvailable() << endl;
        cout << "�ؼ��ʣ�";
        if (book.getkeys().empty()) {
            cout << "(��)";
        }
        else {
            for (const string& key : book.getkeys())
            {
                cout << key<<' ';
            }
        }
            cout << "��飺" << book.getintro() << endl;

        
    }
}
void  BookPage::borrowBook() {
        clearscreen();
        string isbn;
        cout << "����Ҫ���ĵ�ͼ��ISBN: ";
        getline(cin, isbn);
        auto bookIt = find_if(books.begin(), books.end(),
            [&isbn](const Book& b) { return b.getISBN() == isbn; });//find_if�㷨��lambda���ʽ

        if (bookIt != books.end()) {
            if (bookIt->borrowBook()) {
                borrowInfos.emplace_back(currentUser->getUsername(), isbn, false);//��βֱ�ӹ�����Ԫ��
				currentUser->addBorrowedCount();
                cout << "����ɹ�!" << endl;
            }
            else {
                cout << "������ȫ�����!�Ƿ�Ҫ����ԤԼ(Y/N)" << endl;
                char choice;
                cin >> choice;

                if (choice == 'y' || choice == 'Y') {
                    reserveBook(isbn);
                }
            }
        }
        else {
            cout << "δ�ҵ���ͼ��!" << endl;
        }
    }
void BookPage::returnBook() {
    clearscreen();
    string isbn;
    cout << "����Ҫ�黹��ͼ��ISBN: ";
    getline(cin, isbn);

    auto bookIt = find_if(books.begin(), books.end(),
        [&isbn](const Book& b) { return b.getISBN() == isbn; });

    if (bookIt == books.end()) {
        cout << "δ�ҵ���ͼ��!" << endl;
        LOG_ERROR("RETURN", "���Թ黹�����ڵ�ͼ��: " + isbn);
        return;
    }

    Book& returnedBook = *bookIt;

    // ���ҵ�ǰ�û��Ľ��ļ�¼
    auto borrowIt = find_if(borrowInfos.begin(), borrowInfos.end(),
        [&](const BorrowInfo& bi) {
            return bi.getBookISBN() == isbn &&
                bi.getReaderUsername() == currentUser->getUsername() &&
                !bi.isReturned() &&
                !bi.is_Reservation();
        });

    if (borrowIt == borrowInfos.end()) {
        cout << "��û�н��Ĵ�ͼ����ѹ黹!" << endl;
        return;
    }

    // ִ�л������
    returnedBook.returnBook();
    const_cast<BorrowInfo&>(*borrowIt).markReturned();

    LOG_INFO("RETURN", "�û� " + currentUser->getUsername() + " �黹ͼ�� " + isbn + " " + returnedBook.getTitle());
    cout << "����ɹ�!" << endl;

    // ѯ���Ƿ�Ҫ����
    cout << "\n�Ƿ�Ҫ���Ȿ��������ֺ����ۣ�(y/n): ";
    char choice;
    cin >> choice;
    cin.ignore();

    if (choice == 'y' || choice == 'Y') {
        addBookReview(returnedBook);
    }

    // ���ԤԼ���У�����ԭ���߼���
    cout << "���ISBN " << isbn << " ��ԤԼ����..." << endl;
    size_t nextIndex = reservationManager.peekNextIndex(isbn);

    if (nextIndex != SIZE_MAX && nextIndex < borrowInfos.size()) {
        BorrowInfo& nextReservation = borrowInfos[nextIndex];
        cout << "�ҵ�ԤԼ��¼��ԤԼ�û�: " << nextReservation.getReaderUsername() << endl;

        User* reservedUser = finduser(nextReservation.getReaderUsername());
        if (reservedUser != nullptr) {
            Date expiryDate = Date::currentdate();
            expiryDate.addDays(7);

            Notification noti(Notification::RESERVATION_AVAILABLE, isbn, findBookTitle(isbn), expiryDate);
            reservedUser->addNotification(noti);

            cout << "��֪ͨԤԼ�û���" << nextReservation.getReaderUsername() << endl;
            LOG_INFO("NOTIFICATION", "ԤԼ֪ͨ���͸��û�: " + nextReservation.getReaderUsername());
        }
    }
}
void BookPage::addBookReview(Book& book) {
    cout << "=== ͼ������ ===" << endl;
    cout << "ͼ��: " << book.getTitle() << " by " << book.getAuthor() << endl;

    int rating;
    string comment;

    cout << "����Ȿ������ (1-5��): ";
    if (!validateInt(rating) || rating < 1 || rating > 5) {
        cout << "��Ч���֣��������ۡ�" << endl;
        return;
    }

    cout << "��д���������� (��ѡ��ֱ�ӻس�����): ";
    getline(cin, comment);

    // ������۵�ͼ��
    book.addReview(currentUser->getUsername(), rating, comment);

    LOG_INFO("REVIEW", "�û� " + currentUser->getUsername() + " ��ͼ�� " + book.getISBN() + " ����: " + to_string(rating) + "��");

    cout << "��л�������ۣ�" << endl;
    cout << "��ǰƽ������: " << fixed << setprecision(1) << book.getAverageRating() << " ��" << endl;
}
// ��������ʾͼ��
void BookPage::showBooksByCategory() const {
    clearscreen();
    cout << "=== ������鿴ͼ�� ===" << endl;
    cout << "��ѡ��Ҫ�鿴�ķ���:" << endl;
    cout << "0. �������ѧ��֪ʶѧ������" << endl;
    cout << "1. ��ѧ������ѧ" << endl;
    cout << "2. �ڽ�" << endl;
    cout << "3. ����ѧ" << endl;
    cout << "4. ����" << endl;
    cout << "5. ��Ȼ��ѧ����ѧ" << endl;
    cout << "6. ������Ӧ�ÿ�ѧ��" << endl;
    cout << "7. ���������֡�����" << endl;
    cout << "8. ��ѧ" << endl;
    cout << "9. ��ʷ�����ǡ�����" << endl;
    cout << "10. �鿴����ͼ��" << endl;
    cout << "11. ����" << endl;

    int categoryChoice;
    cout << "��ѡ�� (0-11): ";
    if (!validateInt(categoryChoice) || categoryChoice < 0 || categoryChoice > 11) {
        cout << "��Чѡ��!" << endl;
        return;
    }

    if (categoryChoice == 11) return;

    clearscreen();

    if (categoryChoice == 10) {
        // ��ʾ����ͼ��
        showAllBooks();
    }
    else {
        // ��ʾ�ض������ͼ��
        BookCategory category = static_cast<BookCategory>(categoryChoice);
        showCategoryBooks(category);
    }
}
// ��ʾ�ض������ͼ��
void BookPage::showCategoryBooks(BookCategory category) const {
    // ��ȡ��������
    string categoryName;
    switch (category) {
    case BookCategory::COMPUTER_SCIENCE_INFORMATION: categoryName = "�������ѧ��֪ʶѧ������"; break;
    case BookCategory::PHILOSOPHY_PSYCHOLOGY: categoryName = "��ѧ������ѧ"; break;
    case BookCategory::RELIGION: categoryName = "�ڽ�"; break;
    case BookCategory::SOCIAL_SCIENCES: categoryName = "����ѧ"; break;
    case BookCategory::LANGUAGE: categoryName = "����"; break;
    case BookCategory::NATURAL_SCIENCES_MATHEMATICS: categoryName = "��Ȼ��ѧ����ѧ"; break;
    case BookCategory::TECHNOLOGY_APPLIED_SCIENCES: categoryName = "������Ӧ�ÿ�ѧ��"; break;
    case BookCategory::THE_ARTS_RECREATION: categoryName = "���������֡�����"; break;
    case BookCategory::LITERATURE_RHETORIC: categoryName = "��ѧ"; break;
    case BookCategory::HISTORY_GEOGRAPHY: categoryName = "��ʷ�����ǡ�����"; break;
    default: categoryName = "δ֪����"; break;
    }

    cout << "=== " << categoryName << " ��ͼ�� ===" << endl;

    bool found = false;
    for (const auto& book : books) {
        if (book.getCategory() == category) {
            found = true;
            cout << "ISBN: " << book.getISBN()
                << ", ����: " << book.getTitle()
                << ", ����: " << book.getAuthor()
                << ", ����: " << book.getQuantity()
                << ", �ɽ�: " << book.getAvailable()
                << ", �ܽ���: " << book.getTotalBorrowCount();

            if (book.getReviewCount() > 0) {
                cout << ", ����: " << fixed << setprecision(1) << book.getAverageRating() << "��";
            }
            cout << endl;

            // ��ʾ�ؼ��ʺͼ��
            cout << "�ؼ���: ";
            if (book.getkeys().empty()) {
                cout << "(��)";
            }
            else {
                for (const string& key : book.getkeys()) {
                    cout << key << " ";
                }
            }
            cout << endl;
            cout << "���: " << book.getintro() << endl;
            cout << "----------------------------" << endl;
        }
    }

    if (!found) {
        cout << "�÷���������ͼ�顣" << endl;
    }
}
        
 
void BookPage::renewBook() {
    

    string readerUsername = currentUser->getUsername();
    vector<BorrowInfo*> currentBorrows;

    // ���ҵ�ǰ�û�������δ�黹���ļ�¼
    for (auto& info : borrowInfos) {
        if (info.getReaderUsername() == readerUsername && !info.isReturned()) {
            currentBorrows.push_back(&info);
        }
    }

    if (currentBorrows.empty()) {
        cout << "����ǰû�н����κ�ͼ�顣\n";
        return;
    }

    // ��ʾ��ǰ�����б�
    cout << "=== ���ĵ�ǰ���� ===\n";
    for (size_t i = 0; i < currentBorrows.size(); i++) {
        const auto& info = *currentBorrows[i];
        cout << i + 1 << ". ISBN: " << info.getBookISBN()
            << ", ����: " << findBookTitle(info.getBookISBN())
            << ", Ӧ������: ";
        info.getDueDate().displayDate();
        cout << ", " << (info.isRenewed() ? "������" : "������") << "\n";
    }

    // ѡ��Ҫ�����ͼ��
    cout << "��ѡ��Ҫ�����ͼ���� (��0�˳�): ";
    int choice;
    if (!validateInt(choice)) {
        return;
    }

    if (choice == 0 || choice > currentBorrows.size()) {
        return;
    }

    BorrowInfo& selected = *currentBorrows[choice - 1];

        // ����Ƿ��������
        if (selected.isRenewed()) {
            cout << "ÿ����ֻ������һ�Ρ�\n";
            return;
        }

    // ����Ƿ��з���
    selected.updateFine(Date::currentdate());
    if (selected.getFineAmount() > 0) {
        cout << "������δ֧���ķ��� " << selected.getFineAmount()
            << " Ԫ������֧�����\n";
        return;
    }

    // ִ������
    if (selected.renew()) {
        cout << "����ɹ����µ�Ӧ������Ϊ: ";
        selected.getDueDate().displayDate();
        cout << "\n";
    }
    else {
        cout << "����ʧ�ܡ�\n";
    }
}
void BookPage::searchBooksWithCategory() const {
    cout << "�Ƿ�Ҫ�޶�������ͼ����ࣿ(y/n): ";
    char useCategory;
    cin >> useCategory;
    cin.ignore();

    BookCategory selectedCategory;
    bool filterByCategory = false;

    if (useCategory == 'y' || useCategory == 'Y') {
        cout << "��ѡ��Ҫ�����ķ���:" << endl;
        cout << "0. �������ѧ��֪ʶѧ������" << endl;
        cout << "1. ��ѧ������ѧ" << endl;
        cout << "2. �ڽ�" << endl;
        cout << "3. ����ѧ" << endl;
        cout << "4. ����" << endl;
        cout << "5. ��Ȼ��ѧ����ѧ" << endl;
        cout << "6. ������Ӧ�ÿ�ѧ��" << endl;
        cout << "7. ���������֡�����" << endl;
        cout << "8. ��ѧ" << endl;
        cout << "9. ��ʷ�����ǡ�����" << endl;

        int categoryChoice;
        cout << "��ѡ����� (0-9): ";
        if (validateInt(categoryChoice) && categoryChoice >= 0 && categoryChoice <= 9) {
            selectedCategory = static_cast<BookCategory>(categoryChoice);
            filterByCategory = true;
        }
        else {
            cout << "��Чѡ�񣬽������з�����������" << endl;
        }
    }

    // ִ��ԭ�е������߼�������ӷ���ɸѡ
    cout << "��ѡ��������ʽ��\n";
    cout << "1. ��ȷ��������ȫƥ�䣩\n";
    cout << "2. ģ�����������ƶ�ƥ�䣩\n";
    cout << "������ѡ�";

    int searchType;
    if (validateInt(searchType)) {
        if (searchType != 1 && searchType != 2) {
            cerr << "��Ч����" << endl;
            return;
        }
        clearscreen();
        cout << "��ѡ�������ֶΣ�\n";
        cout << "1. ISBN\n";
        cout << "2. ����\n";
        cout << "3. ����\n";
        cout << "4. �ؼ���\n";
        cout << "5. ���\n";
        cout << "������ѡ�";
        int fieldChoice;

        if (validateInt(fieldChoice, "������ѡ�")) {
            if (fieldChoice < 1 || fieldChoice > 5) {
                cerr << "��Чѡ�" << endl;
                return;
            }
            cout << "�������������ݣ�";
            cin.ignore();
            string query;
            getline(cin, query);

            vector<Book> results;

            // ʹ�ô�����ɸѡ����������
            switch (fieldChoice) {
            case 1: // ISBN
                if (searchType == 1) {
                    results = exactSearchWithCategory(query, [](const Book& b) { return b.getISBN(); }, filterByCategory, selectedCategory);
                }
                else {
                    results = fuzzySearchWithCategory(query, [](const Book& b) { return b.getISBN(); }, filterByCategory, selectedCategory);
                }
                break;
            case 2: // ����
                if (searchType == 1) {
                    results = exactSearchWithCategory(query, [](const Book& b) { return b.getTitle(); }, filterByCategory, selectedCategory);
                }
                else {
                    results = fuzzySearchWithCategory(query, [](const Book& b) { return b.getTitle(); }, filterByCategory, selectedCategory);
                }
                break;
            case 3: // ����
                if (searchType == 1) {
                    results = exactSearchWithCategory(query, [](const Book& b) { return b.getAuthor(); }, filterByCategory, selectedCategory);
                }
                else {
                    results = fuzzySearchWithCategory(query, [](const Book& b) { return b.getAuthor(); }, filterByCategory, selectedCategory);
                }
                break;
            case 4: // �ؼ���
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
            case 5: // ���
                if (searchType == 1) {
                    results = exactSearchWithCategory(query, [](const Book& b) { return b.getintro(); }, filterByCategory, selectedCategory);
                }
                else {
                    results = fuzzySearchWithCategory(query, [](const Book& b) { return b.getintro(); }, filterByCategory, selectedCategory);
                }
                break;
            default:
                cerr << "��Чѡ�\n";
                return;
            }

            // ��ʾ�������
            if (results.empty()) {
                cout << "δ�ҵ�ƥ����鼮��\n";
            }
            else {
                cout << "�ҵ� " << results.size() << " ���飺\n";
                for (const Book& book : results) {
                    cout << "ISBN: " << book.getISBN() << "\n";
                    cout << "����: " << book.getTitle() << "\n";
                    cout << "����: " << book.getAuthor() << "\n";
                    cout << "����: " << book.getCategoryName() << "\n";
                    cout << "�ɽ�����: " << book.getAvailable() << "\n";
                    cout << "�ܽ��Ĵ���: " << book.getTotalBorrowCount() << "\n";

                    if (book.getReviewCount() > 0) {
                        cout << "ƽ������: " << fixed << setprecision(1) << book.getAverageRating() << " �� (" << book.getReviewCount() << " ������)\n";
                    }
                    else {
                        cout << "��������\n";
                    }
                    cout << "----------------------------\n";
                }
            }
        }
    }
}
// ������ɸѡ�ľ�ȷ����
template<typename FieldGetter>
vector<Book> BookPage::exactSearchWithCategory(
    const string& query,
    FieldGetter fieldGetter,
    bool filterByCategory,
    BookCategory category
) const {
    vector<Book> results;
    for (const Book& book : books) {
        // ����ɸѡ
        if (filterByCategory && book.getCategory() != category) {
            continue;
        }

        string fieldValue = fieldGetter(book);
        if (match(query, fieldValue)) {
            results.push_back(book);
        }
    }

    // ����������������
    sort(results.begin(), results.end(),
        [](const Book& a, const Book& b) {
            return a.getBorrowed() > b.getBorrowed();
        });

    return results;
}

// ������ɸѡ��ģ������
template<typename FieldGetter>
vector<Book> BookPage::fuzzySearchWithCategory(
    const string& query,
    FieldGetter fieldGetter,
    bool filterByCategory,
    BookCategory category
) const {
    vector<pair<size_t, Book>> scoredBooks;

    for (const Book& book : books) {
        // ����ɸѡ
        if (filterByCategory && book.getCategory() != category) {
            continue;
        }

        const string fieldValue = fieldGetter(book);
        size_t score = countCommonTokens(query, fieldValue);
        if (score > 0) {
            scoredBooks.emplace_back(score, book);
        }
    }

    // ��������������
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


