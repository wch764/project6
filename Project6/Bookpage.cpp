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
                cout << "����ɹ�!" << endl;
            }
            else {
                cout << "������ȫ�����!�Ƿ�Ҫ����ԤԼ(Y/N)" << endl;
                char choice;
                cin >> choice;

                if (choice == 'y' || choice == 'Y') {
                    reserveBook(isbn);
                }
                   /*1. ��ȡ��������δ�黹��¼�������������ĺ�ԤԼ��
                    vector<BorrowInfo*> activeRecords;
                    for (auto& info : borrowInfos) {
                        if (info.getBookISBN() == isbn && !info.isReturned()) {
                            activeRecords.push_back(&info);
                        }
                    }

                    // 2. ������������ļ�¼����ԤԼ��
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

                    // 3. ���黹ʱ�������������ļ�¼
                    sort(normalBorrows.begin(), normalBorrows.end(),
                        [](BorrowInfo* a, BorrowInfo* b) {
                            return a->getDueDate() < b->getDueDate();
                        });

                    // 4. �����µ�ԤԼ��ź�Ԥ�ƿɽ�����
                    int newSeq = existingReservations.size() + 1;
                    Date expectedDate;

                    if (normalBorrows.size() >= newSeq) {
                        // ���㹻����������ļ�¼��ӦԤԼ���
                        expectedDate = normalBorrows[newSeq - 1]->getDueDate();
                    }
                    else {
                        // û���㹻���ļ�¼��ʹ�����һ�����ļ�¼
                        expectedDate = normalBorrows.back()->getDueDate();
                        // ׷�ӻ����ڣ���ÿ����һ��ԤԼ��3�죩
                        expectedDate.addDays((newSeq - normalBorrows.size()) * 3);
                    }

                    expectedDate.addDays(1); // Ӧ�����ڴ��տɽ�

                    // 5. ����ԤԼ��¼
                    borrowInfos.emplace_back(currentUser->getUsername(), isbn);
                    BorrowInfo& newReservation = borrowInfos.back();
                    newReservation.setAsReservation(newSeq, expectedDate);
                    if (reservationManager.addReservation(&newReservation)) {
                        cout << "ԤԼ�ɹ���\n";
                        cout << "�����Ŷ����: " << newSeq << "\n";
                        cout << "Ԥ�ƿɽ�����: ";
                        expectedDate.displayDate();
                        cout << "\n";
                    }
                    else {
                        cout << "ԤԼʧ�ܣ�\n";
                        borrowInfos.pop_back(); // �Ƴ�ʧ�ܵ�ԤԼ��¼
                    }
                    
                }*/
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
        return;
    }

    Book& returnedBook = *bookIt;

    // ���ҵ�ǰ�û��Ľ��ļ�¼
    auto borrowIt = find_if(borrowInfos.begin(), borrowInfos.end(),
        [&](const BorrowInfo& bi) {
            return bi.getBookISBN() == isbn &&
                bi.getReaderUsername() == currentUser->getUsername() &&
                !bi.isReturned() &&
                !bi.is_Reservation(); // ȷ������ԤԼ��¼
        });

    if (borrowIt == borrowInfos.end()) {
        cout << "��û�н��Ĵ�ͼ����ѹ黹!" << endl;
        return;
    }

    // ִ�л������
    returnedBook.returnBook();
    borrowIt->markReturned();
    cout << "����ɹ�!" << endl;

    // ���ԤԼ����
    cout << "���ISBN " << isbn << " ��ԤԼ����..." << endl;
    size_t nextIndex = reservationManager.peekNextIndex(isbn);

    if (nextIndex != SIZE_MAX && nextIndex < borrowInfos.size()) {
        BorrowInfo& nextReservation = borrowInfos[nextIndex];
        cout << "�ҵ�ԤԼ��¼��ԤԼ�û�: " << nextReservation.getReaderUsername() << endl;

        // ����ԤԼ�û�
        User* reservedUser = finduser(nextReservation.getReaderUsername());
        if (reservedUser != nullptr) {
            cout << "�ɹ��ҵ�ԤԼ�û���׼������֪ͨ..." << endl;

            // ����֪ͨ��ԤԼ�û�
            Date expiryDate = Date::currentdate();
            expiryDate.addDays(7); // 7����ȷ�Ͻ���

            Notification noti(Notification::RESERVATION_AVAILABLE, isbn, findBookTitle(isbn), expiryDate);
            reservedUser->addNotification(noti);

            cout << "��֪ͨԤԼ�û���" << nextReservation.getReaderUsername() << endl;
        }
        else {
            cout << "�����޷��ҵ�ԤԼ�û� " << nextReservation.getReaderUsername() << endl;
        }
    }
    else {
        cout << "����û��ԤԼ��¼" << endl;
    }
}

            
            /*BorrowInfo* reservation = reservationManager.peekNext(isbn);
            reservationManager.debugPrint();
            if (reservation) {
                cout << 1;
                // 1. ����ͼ�鱣��״̬
                returnedBook.reserveFor(reservation->getReaderUsername());

                // 2. ����֪ͨ
                User* reservedUser = finduser(reservation->getReaderUsername());
                if (reservedUser) {
                    reservationManager.processReturnedBook(isbn, returnedBook.getTitle(),
                        *reservedUser);

                    cout << "������ΪԤԼ�߱������ѷ���֪ͨ\n";
                }
                else {
                    cout << "����ԤԼ�û�������\n";
                }
            }
            else {
                returnedBook.releaseReservation();
            }
        }*/
        
 
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


