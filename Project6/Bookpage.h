#pragma once
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
#include <locale>
#include <codecvt>
#include <cctype>
#include <unordered_map>
#include <algorithm>
#include"failmanage.h"
#include"reservemanager.h"


// ͼ��ҳ����
class BookPage : public Page, public Action {
private:
    vector<Book>& books;
    vector<BorrowInfo>& borrowInfos;
    User* currentUser;
    ReservationManager& reservationManager;
    vector<Reader*>& users;

public:
    BookPage(vector<Book>& bs, vector<BorrowInfo>& bis, vector<Reader*>& us, User* user, ReservationManager& rm);
    bool display() const override final;
    void performAction() override final;
    void editbooks();
    void addbook();
    void deletebook();
    void modifybook();
    // �����û�����
    User* finduser(const string& username) {
        cout << "���ڲ����û�: " << username << endl;
        cout << "�û��б��С: " << users.size() << endl;

        for (size_t i = 0; i < users.size(); ++i) {
            if (users[i] != nullptr) {
                cout << "����û� " << i << ": " << users[i]->getUsername() << endl;

                // ʹ���ַ����Ƚϣ������ִ�Сд
                string currentUsername = users[i]->getUsername();
                if (currentUsername == username) {
                    cout << "�ҵ��û�: " << currentUsername << endl;
                    return static_cast<User*>(users[i]);
                }
            }
            else {
                cout << "���ֿ�ָ���û�������: " << i << endl;
            }
        }

        cout << "δ�ҵ��û�: " << username << endl;
        cout << "ϵͳ�������û��б�" << endl;
        for (size_t i = 0; i < users.size(); ++i) {
            if (users[i] != nullptr) {
                cout << "  [" << i << "] " << users[i]->getUsername() << endl;
            }
        }

        return nullptr;
    }

    std::vector<std::string> tokenize(const std::string& str) const {
        std::vector<std::string> tokens;
        std::string currentToken;

        for (size_t i = 0; i < str.size(); ) {
            unsigned char c = static_cast<unsigned char>(str[i]);

            // ����Ӣ����ĸ��ASCII ��Χ��
            if (isalpha(c) || c == '\'') {
                currentToken += tolower(c);
                i++;
            }
            else if (isdigit(c)) {
                if (!currentToken.empty()) {
                    tokens.push_back(currentToken);
                    currentToken.clear();
                }
                tokens.push_back(std::string(1, c)); // ��ǰ������Ϊ����token
                i++;
            }
            // �����֣�GBK/UTF-8 �жϣ�
            else if (c >= 0x80) {
                if (!currentToken.empty()) {
                    tokens.push_back(currentToken);
                    currentToken.clear();
                }
                // UTF-8 ����ռ 3 �ֽڣ�GBK ռ 2 �ֽ�
                size_t charLen = (c >= 0xE0) ? 3 : 2;  // ���ж� UTF-8 ����
                tokens.push_back(str.substr(i, charLen));
                i += charLen;
            }
            // ����ո�ͱ�����
            else if (isspace(c) || ispunct(c)) {
                if (!currentToken.empty()) {
                    tokens.push_back(currentToken);
                    currentToken.clear();
                }
                i++;
            }
            else {
                i++;  // ���������������ַ���
            }
        }

        if (!currentToken.empty()) {
            tokens.push_back(currentToken);
        }

        return tokens;
    }

    // ���㹫���ַ�����Ӣ�ĵ��� + ���ֵ��֣�
    int countCommonTokens(const std::string& str1, const std::string& str2) const {
        auto tokens1 = tokenize(str1);
        auto tokens2 = tokenize(str2);

        std::unordered_map<std::string, int> freq1, freq2;

        // ͳ�� str1 �Ĵ�Ƶ
        for (const auto& token : tokens1) {
            freq1[token]++;
        }

        // ͳ�� str2 �Ĵ�Ƶ
        for (const auto& token : tokens2) {
            freq2[token]++;
        }

        // ���㹫���ַ���
        int commonCount = 0;
        for (const auto& entry : freq1) {
            const std::string& token = entry.first;
            int count = entry.second;
            if (freq2.count(token)) {

                commonCount += std::max(count, freq2[token]);//ǿ����Ƶ��
            }
        }

        return commonCount;
    }



    string removeSpaces(const string& str) const {
        string result;
        for (char c : str) {
            if (!isspace(static_cast<unsigned char>(c))) {
                result += c;
            }
        }
        return result;
    }
    bool match(const string& str1, const string& str2) const {
        // �Ƴ��ո����ַ���
        string s1 = removeSpaces(str1);
        string s2 = removeSpaces(str2);
        if (s1.size() != s2.size()) return false;

        return equal(s1.begin(), s1.end(), s2.begin(),
            [](char a, char b) {
                return tolower(static_cast<unsigned char>(a)) ==
                    tolower(static_cast<unsigned char>(b));
            });
    }
    template<typename FieldGetter>
    vector<Book> exactSearch(
        const string& query,
        FieldGetter fieldGetter//����ָ�루ָ��book.getTitle()�ȣ�

    ) const {
        vector<Book> results;
        for (const Book& book : books) {
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
    template<typename FieldGetter>
    vector<Book> fuzzySearch(
        const string& query,
        FieldGetter fieldGetter
    ) const {
        vector<pair<size_t, Book>> scoredBooks; // (LCS ����, Book)

        for (const Book& book : books) {
            const string fieldValue = fieldGetter(book);
            size_t score = countCommonTokens(query, fieldValue); // ���� LCS ����
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
        for (size_t i = 0; i < scoredBooks.size(); i++) {
            results.push_back(scoredBooks[i].second);

        }


        return results;
    }

    void searchbooks() const
    {
        cout << "��ѡ��������ʽ��\n";
        cout << "1. ��ȷ��������ȫƥ�䣩\n";
        cout << "2. ģ�����������ƶ�ƥ�䣩\n";
        cout << "������ѡ�";

        int searchType;


        if (validateInt(searchType))
        {
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


            if (validateInt(fieldChoice, "������ѡ�"))
            {
                if (fieldChoice < 1 || fieldChoice > 5) {
                    cerr << "��Чѡ�" << endl;
                    return;
                }
                cout << "�������������ݣ�";
                cin.ignore();
                string query;
                getline(cin, query);

                vector<Book> results;

                switch (fieldChoice) {
                case 1: // ISBN
                    if (searchType == 1) {
                        results = exactSearch(query, [](const Book& b) { return b.getISBN(); });
                    }
                    else {
                        results = fuzzySearch(query, [](const Book& b) { return b.getISBN(); });
                    }

                    break;
                case 2: // ����
                    if (searchType == 1) {
                        results = exactSearch(query, [](const Book& b) { return b.getTitle(); });
                    }
                    else {
                        results = fuzzySearch(query, [](const Book& b) { return b.getTitle(); })
                            ;
                    }
                    break;
                case 3: // ����
                    if (searchType == 1) {
                        results = exactSearch(query, [](const Book& b) { return b.getAuthor(); });
                    }
                    else {
                        results = fuzzySearch(query, [](const Book& b) { return b.getAuthor(); });
                    }
                    break;
                case 4: // �ؼ���
                    if (searchType == 1) {
                        results = exactSearch(query, [](const Book& b) {
                            string keywordsStr;
                            for (const auto& kw : b.getkeys()) {
                                keywordsStr += kw + " ";
                            }
                            return keywordsStr;
                            });
                    }
                    else {
                        results = fuzzySearch(query, [](const Book& b) {
                            string keywordsStr;
                            for (const auto& kw : b.getkeys()) {
                                keywordsStr += kw + " ";
                            }
                            return keywordsStr;
                            });
                    }
                    break;
                case 5: // ���
                    if (searchType == 1) {
                        results = exactSearch(query, [](const Book& b) { return b.getintro(); });
                    }
                    else {
                        results = fuzzySearch(query, [](const Book& b) { return b.getintro(); });
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
                        cout << "�ɽ�����: " << book.getAvailable() << "\n";
                        cout << "----------------------------\n";
                    }
                }
            }

        }
    }

    void showAllBooks() const;
    void borrowBook();
    void renewBook();
    void returnBook();
    // ����������ͨ��ISBN��������
    string findBookTitle(const string& isbn) const {
        for (const auto& book : books) {
            if (book.getISBN() == isbn) {
                return book.getTitle();
            }
        }
        return "δ֪ͼ��";
    }
    // �޸�ԤԼ�������̣�ȷ����ȷ��ӵ�ReservationManager
    void reserveBook(const string isbn) {
       

        // ����ͼ��
        auto bookIt = find_if(books.begin(), books.end(),
            [&isbn](const Book& b) { return b.getISBN() == isbn; });

        if (bookIt == books.end()) {
            cout << "ͼ�鲻���ڣ�" << endl;
            return;
        }

        Book& book = *bookIt;
        cout << "�ҵ�ͼ��: " << book.getTitle() << endl;

        // ����Ƿ��ԤԼ
        if (book.isAvailable()) {
            cout << "��ͼ�鵱ǰ�ɽ裬����ԤԼ����ֱ�ӽ��ġ�" << endl;
            return;
        }

        // ����û��Ƿ��Ѿ�ԤԼ������
        for (const auto& info : borrowInfos) {
            if (info.getBookISBN() == isbn &&
                info.getReaderUsername() == currentUser->getUsername() &&
                info.is_Reservation()) {
                cout << "���Ѿ�ԤԼ�����飡" << endl;
                return;
            }
        }

        cout << "��ʼ����ԤԼ��¼..." << endl;

        // ��ȡ��ǰ���д�С
        int queueSize = reservationManager.getQueueSize(isbn);
        cout << "��ǰԤԼ���д�С: " << queueSize << endl;

        // ����ԤԼ��¼
        BorrowInfo newReservation(currentUser->getUsername(), isbn);
        Date expectedDate = calculateAvailableDate(isbn);
        newReservation.setAsReservation(queueSize + 1, expectedDate);

        cout << "����ԤԼ��¼���:" << endl;
        cout << "  - ԤԼ���: " << newReservation.getReservationSequence() << endl;
        cout << "  - �Ƿ�ԤԼ��¼: " << (newReservation.is_Reservation() ? "��" : "��") << endl;

        // ��ӵ�borrowInfos����¼����
        size_t newIndex = borrowInfos.size();
        borrowInfos.push_back(newReservation);
        cout << "����ӵ�borrowInfos������: " << newIndex << endl;

        // ʹ��������ӵ�ԤԼ������
        if (reservationManager.addReservation(newIndex)) {
            cout << "ԤԼ�ɹ��������Ŷ����: "
                << newReservation.getReservationSequence() << endl;
            cout << "Ԥ�ƿɽ�����: "
                << newReservation.getExpectedAvailableDate().toString() << endl;
        }
        else {
            cout << "ԤԼʧ�ܣ���borrowInfos���Ƴ���¼" << endl;
            borrowInfos.pop_back();
        }
    }

   

    Date calculateAvailableDate(const string& isbn) {
        // ʵ�ֲ�������ɽ����ڵ��߼�
        // ʾ�����ҵ��������絽�ڵĽ��ļ�¼
        Date earliest(2100, 1, 1);
        for (const auto& info : borrowInfos) {
            if (info.getBookISBN() == isbn && !info.isReturned()) {
                if (info.getDueDate() < earliest) {
                    earliest = info.getDueDate();
                }
            }
        }
        earliest.addDays(1); // �����մ��տɽ�
        return earliest;
    
    }
    void showNotifications() {
        cout << "=== ��Ϣ���� ===" << endl;

        // ��ȡ����֪ͨ
        auto allNotifications = currentUser->getAllNotifications();

        if (allNotifications.empty()) {
            cout << "û����֪ͨ\n";
            return;
        }

        // �����ͷ���֪ͨ
        vector<Notification> reservationNotifications;
        vector<Notification> dueSoonNotifications;
        vector<Notification> fineNotifications;
        vector<Notification> otherNotifications;

        for (const auto& noti : allNotifications) {
            switch (noti.getType()) {
            case Notification::RESERVATION_AVAILABLE:
            case Notification::RESERVATION_EXPIRED:
                reservationNotifications.push_back(noti);
                break;
            case Notification::DUE_SOON:
                dueSoonNotifications.push_back(noti);
                break;
            case Notification::OVERDUE_FINE:
                fineNotifications.push_back(noti);
                break;
            default:
                otherNotifications.push_back(noti);
                break;
            }
        }

        // ��ʾԤԼ֪ͨ
        if (!reservationNotifications.empty()) {
            cout << "\n��ԤԼ֪ͨ��" << endl;
            vector<string> availableBooks;

            for (const auto& noti : reservationNotifications) {
                cout << "�� " << noti.getMessage();
                if (!noti.getIsRead()) cout << " [��]";
                cout << endl;

                if (noti.getType() == Notification::RESERVATION_AVAILABLE && !noti.getIsRead()) {
                    availableBooks.push_back(noti.getBookIsbn());
                }
            }

            // ����ԤԼȷ��
            if (!availableBooks.empty()) {
                cout << "\n�Ƿ�Ҫ������ЩԤԼͼ�飿(y/n): ";
                char choice;
                cin >> choice;

                if (choice == 'y' || choice == 'Y') {
                    cout << "��ʼ����ԤԼ����..." << endl;
                    for (const auto& isbn : availableBooks) {
                        bool result = confirmborrow(isbn, currentUser->getUsername());
                        if (result) {
                            cout << "ISBN " << isbn << " ����ȷ�ϳɹ�" << endl;
                        }
                        else {
                            cout << "ISBN " << isbn << " ����ȷ��ʧ��" << endl;
                        }
                    }

                    // ����Ѵ����ԤԼ֪ͨ
                    currentUser->getAvailableReservations();
                }
            }
        }

        // ��ʾ��������
        if (!dueSoonNotifications.empty()) {
            cout << "\n���������ѡ�" << endl;
            for (const auto& noti : dueSoonNotifications) {
                cout << "�� " << noti.getMessage();
                if (!noti.getIsRead()) cout << " [��]";
                cout << endl;
            }

            // ��ǵ���֪ͨΪ�Ѷ����û�����֪ͨҳ���
            currentUser->markDueNotificationsAsRead();
            cout << "���������ѱ��Ϊ�Ѷ�" << endl;
        }

        // ��ʾ����֪ͨ����Զ��ʾΪδ����
        if (!fineNotifications.empty()) {
            cout << "\n������֪ͨ��" << endl;
            double totalFine = 0.0;

            for (const auto& noti : fineNotifications) {
                cout << "�� " << noti.getMessage() << " [δ����]" << endl;
                totalFine += noti.getFineAmount();
            }

            cout << "����ǰ�ܼ�δ������: " << totalFine << " Ԫ" << endl;
            cout << "�뾡����ɷ���������Ӱ�����Ȩ�ޣ�" << endl;
        }

        // ��ʾ����֪ͨ
        if (!otherNotifications.empty()) {
            cout << "\n������֪ͨ��" << endl;
            for (const auto& noti : otherNotifications) {
                cout << "�� " << noti.getMessage();
                if (!noti.getIsRead()) cout << " [��]";
                cout << endl;
            }
        }

        cout << "\n֪ͨ�鿴��ϡ�" << endl;
    }
    bool confirmborrow(const string& isbn, const string& username) {
        // ���������Ϊ��һ�У�ȷ������������
        cout << "*** CONFIRMBORROW ���������� *** ISBN=" << isbn << " �û�=" << username << endl;

        cout << "=== ȷ�Ͻ������̿�ʼ ===" << endl;
        cout << "ISBN: " << isbn << ", �û�: " << username << endl;

        // 1. ����ͼ��
        auto bookIt = find_if(books.begin(), books.end(),
            [&isbn](const Book& b) { return b.getISBN() == isbn; });

        if (bookIt == books.end()) {
            cout << "����ͼ�鲻����" << endl;
            return false;
        }

        Book& book = *bookIt;
        cout << "�ҵ�ͼ��: " << book.getTitle() << ", �ɽ�����: " << book.getAvailable() << endl;

        // 2. ���ͼ���Ƿ�ɽ�
        if (!book.isAvailable()) {
            cout << "���鵱ǰ���ɽ裬��������: " << book.getAvailable() << endl;
            return false;
        }

        // 3. �����û���ԤԼ��¼
        cout << "��ʼ����ԤԼ��¼..." << endl;
        auto reservationIt = find_if(borrowInfos.begin(), borrowInfos.end(),
            [&](BorrowInfo& bi) {
                cout << "����¼: �û�=" << bi.getReaderUsername()
                    << " ISBN=" << bi.getBookISBN()
                    << " ԤԼ=" << (bi.is_Reservation() ? "��" : "��")
                    << " �ѻ�=" << (bi.isReturned() ? "��" : "��") << endl;

                return bi.getBookISBN() == isbn &&
                    bi.getReaderUsername() == username &&
                    bi.is_Reservation() &&
                    !bi.isReturned();
            });

        if (reservationIt == borrowInfos.end()) {
            cout << "����δ�ҵ����û�����ЧԤԼ��¼" << endl;
            cout << "borrowInfos����: " << borrowInfos.size() << endl;
            return false;
        }

        cout << "*** �ҵ�ԤԼ��¼����ʼת��... ***" << endl;

        // 4. ת��ԤԼ��¼Ϊ���ļ�¼
        BorrowInfo& reservation = *reservationIt;

        // �����µĽ������ں�Ӧ������
        Date currentDate = Date::currentdate();
        reservation.setBorrowDate(currentDate);
        reservation.setDueDate(BorrowInfo::calculateDueDate(currentDate));

        // ת��Ϊ�������ļ�¼
        reservation.converttoborrow();

        cout << "ԤԼ��¼��ת��Ϊ���ļ�¼" << endl;
        cout << "  - ��������: " << reservation.getBorrowDate().toString() << endl;
        cout << "  - Ӧ������: " << reservation.getDueDate().toString() << endl;

        // 5. ����ͼ��״̬
        if (!book.borrowBook()) {
            cout << "����ͼ�����״̬����ʧ��" << endl;
            return false;
        }

        // 6. ��ԤԼ�������Ƴ�
        if (reservationManager.fulfillReservation(isbn)) {
            cout << "�Ѵ�ԤԼ�������Ƴ�" << endl;
        }
        else {
            cout << "���棺��ԤԼ�����Ƴ�ʧ��" << endl;
        }

        cout << "*** CONFIRMBORROW �ɹ���� *** Ӧ�����ڣ�" << reservation.getDueDate().toString() << endl;
        return true;
    }
       
};