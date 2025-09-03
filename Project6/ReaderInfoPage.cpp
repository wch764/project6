#include<string>
#include"Page.h"
#include"Action.h"
#include<vector>
#include"Reader.h"
#include"LoginPage.h"
#include"clearscreen.h"
#include"Book.h"
#include"BorrowInfo.h"
#include"BorrowStatisticsPage.h"
#include"Bookpage.h"
#include"ReaderInfoPage.h"
using namespace std;
ReaderInfoPage::ReaderInfoPage(vector<Reader>& rdrs, User* user)
        : readers(rdrs), currentUser(user) {
    }

bool ReaderInfoPage::display() const {
    bool isAdmin = currentUser->get_isadmin();

    clearscreen();
    if (isAdmin) {
        std::cout << "=== ���߹��� ===" << std::endl;
        std::cout << "1. �鿴���ж�����Ϣ" << std::endl;
        std::cout << "2. ע�����û�" << std::endl;
        std::cout << "3. �޸Ķ��߱�ҵ����" << std::endl;
        std::cout << "4. ���������" << std::endl;
        std::cout << "5. ɾ�������˺�" << std::endl;
        std::cout << "6. �������˵�" << std::endl;
    }
    else {
        std::cout << "=== ������Ϣ���� ===" << std::endl;
        std::cout << "1. �鿴������Ϣ" << std::endl;
        std::cout << "2. �޸��û���" << std::endl;
        std::cout << "3. �޸�����" << std::endl;
        std::cout << "4. �������˵�" << std::endl;
    }

    return isAdmin;
}

void ReaderInfoPage::performAction() {
    while (true) {
        bool isAdmin = display();

        int choice;
        std::cout << "��ѡ�����: ";
        if (validateInt(choice)) {
            if (isAdmin) {
                switch (choice) {
                case 1: showAllReaders(); system("pause"); break;
                case 2: registerNewUser(); system("pause"); break;
                case 3: modifyGraduationDate(); system("pause"); break;
                case 4: processFinePay(); system("pause"); break;
                case 5: deleteReader(); system("pause"); break;
                case 6: return;
                default: std::cout << "��Чѡ��!" << std::endl;
                }
            }
            else {
                switch (choice) {
                case 1: showPersonalInfo(); system("pause"); break;
                case 2: modifyUsername(); system("pause"); break;
                case 3: modifyPassword(); system("pause"); break;
                case 4: return;
                default: std::cout << "��Чѡ��!" << std::endl;
                }
            }
        }
    }
}

// ����Ա���ܣ��鿴���ж�����Ϣ
void ReaderInfoPage::showAllReaders() const {
    clearscreen();
    std::cout << "=== ���ж�����Ϣ ===" << std::endl;
    std::cout << std::left << std::setw(15) << "�û���"
        << std::setw(12) << "����"
        << std::setw(15) << "Ժϵ"
        << std::setw(10) << "����"
        << std::setw(12) << "��ҵ����"
        << std::setw(8) << "������"
        << std::setw(10) << "����(Ԫ)" << std::endl;
    std::cout << std::string(80, '-') << std::endl;

    for (const auto& reader : readers) {
        std::cout << std::left << std::setw(15) << reader.getUsername()
            << std::setw(12) << reader.getName()
            << std::setw(15) << reader.getDepartmentName()
            << std::setw(10) << reader.getStudentTypeName()
            << std::setw(12) << reader.getGraduationDate().toString()
            << std::setw(8) << reader.getBorrowedCount()
            << std::setw(10) << std::fixed << std::setprecision(2)
            << reader.getFineAmount() << std::endl;
    }
}

// ����Ա���ܣ�ע�����û�
void ReaderInfoPage::registerNewUser() {
    clearscreen();
    std::cout << "=== ע�����û� ===" << std::endl;
    if (std::cin.rdbuf()->in_avail() > 0) {
        cin.ignore();
    }
    std::string username, password, name;
    int deptChoice, typeChoice;


    std::cout << "�����û���: ";
    std::getline(std::cin, username);

    // ����û����Ƿ��Ѵ���
    auto it = std::find_if(readers.begin(), readers.end(),
        [&username](const Reader& r) { return r.getUsername() == username; });

    if (it != readers.end()) {
        std::cout << "�û����Ѵ���!" << std::endl;
        return;
    }

    std::cout << "��������: ";
    std::getline(std::cin, password);
    std::cout << "������ʵ����: ";
    std::getline(std::cin, name);

    // ѡ��Ժϵ
    std::cout << "\nѡ��Ժϵ:\n";
    std::cout << "0. �������ѧ  1. ��ѧ      2. ����\n";
    std::cout << "3. ��ѧ        4. ����      5. ��ѧ\n";
    std::cout << "6. ��ʷ        7. ����ѧ    8. ��ѧ\n";
    std::cout << "9. ҽѧ        10. ����ѧ   11. ����\n";
    std::cout << "��ѡ�� (0-11): ";

    if (!validateInt(deptChoice) || deptChoice < 0 || deptChoice > 11) {
        std::cout << "��Ч��Ժϵѡ��!" << std::endl;
        return;
    }

    // ѡ��ѧ������
    std::cout << "\nѡ��ѧ������:\n";
    std::cout << "0. ������\n";
    std::cout << "1. �о���\n";
    std::cout << "��ѡ�� (0-1): ";

    if (!validateInt(typeChoice) || (typeChoice != 0 && typeChoice != 1)) {
        std::cout << "��Ч��ѧ������ѡ��!" << std::endl;
        return;
    }

    // �����¶���
    Department dept = static_cast<Department>(deptChoice);
    StudentType type = static_cast<StudentType>(typeChoice);

    readers.emplace_back(username, password, name, dept, type);

    std::cout << "�û�ע��ɹ�!" << std::endl;
    std::cout << "�û���: " << username << std::endl;
    std::cout << "Ԥ�Ʊ�ҵ����: " << readers.back().getGraduationDate().toString() << std::endl;
}

// ����Ա���ܣ��޸Ķ��߱�ҵ����
void ReaderInfoPage::modifyGraduationDate() {
    clearscreen();
    std::cout << "=== �޸Ķ��߱�ҵ���� ===" << std::endl;

    std::string username;
    std::cin.ignore();
    std::cout << "����Ҫ�޸ĵ��û���: ";
    std::getline(std::cin, username);

    auto it = std::find_if(readers.begin(), readers.end(),
        [&username](const Reader& r) { return r.getUsername() == username; });

    if (it == readers.end()) {
        std::cout << "�Ҳ������û�!" << std::endl;
        return;
    }

    Reader& reader = *it;
    std::cout << "��ǰ��Ϣ:\n" << reader.getDetailedInfo() << std::endl;

    std::cout << "\n�޸�ѡ��:\n";
    std::cout << "1. ��������ѧ�����ͣ����Զ����¼����ҵ���ڣ�\n";
    std::cout << "2. �ֶ����ñ�ҵ����\n";
    std::cout << "3. ȡ��\n";
    std::cout << "��ѡ��: ";

    int option;
    if (!validateInt(option)) return;

    switch (option) {
    case 1: {
        std::cout << "ѡ���µ�ѧ������:\n";
        std::cout << "0. ��������ע������+4�꣩\n";
        std::cout << "1. �о�����ע������+3�꣩\n";
        std::cout << "��ѡ��: ";

        int typeChoice;
        if (validateInt(typeChoice) && (typeChoice == 0 || typeChoice == 1)) {
            reader.setStudentType(static_cast<StudentType>(typeChoice));
            std::cout << "ѧ�������Ѹ��£��µı�ҵ����: "
                << reader.getGraduationDate().toString() << std::endl;
        }
        else {
            std::cout << "��Чѡ��!" << std::endl;
        }
        break;
    }
    case 2: {
        int year, month, day;
        std::cout << "�����µı�ҵ���� (�� �� ��): ";
        if (std::cin >> year >> month >> day) {
            Date newDate(year, month, day);
            if (newDate > Date::currentdate()) {
                reader.setGraduationDate(newDate);
                std::cout << "��ҵ�����Ѹ���Ϊ: " << newDate.toString() << std::endl;
            }
            else {
                std::cout << "��ҵ���ڲ����ǹ�ȥ������!" << std::endl;
            }
        }
        else {
            std::cout << "���ڸ�ʽ����!" << std::endl;
        }
        break;
    }
    case 3:
        return;
    default:
        std::cout << "��Чѡ��!" << std::endl;
    }
}

// ����Ա���ܣ����������
void ReaderInfoPage::processFinePay() {
    clearscreen();
    std::cout << "=== ��������� ===" << std::endl;

    std::string username;
    std::cin.ignore();
    std::cout << "����Ҫ������û���: ";
    std::getline(std::cin, username);

    auto it = std::find_if(readers.begin(), readers.end(),
        [&username](const Reader& r) { return r.getUsername() == username; });

    if (it == readers.end()) {
        std::cout << "�Ҳ������û�!" << std::endl;
        return;
    }

    Reader& reader = *it;
    double currentFine = reader.getFineAmount();

    if (currentFine <= 0) {
        std::cout << "���û�û��δ���ɵķ��" << std::endl;
        return;
    }

    std::cout << "�û�: " << reader.getName() << " (" << reader.getUsername() << ")\n";
    std::cout << "��ǰ������: " << std::fixed << std::setprecision(2)
        << currentFine << " Ԫ\n";

    std::cout << "\nѡ�����:\n";
    std::cout << "1. ȫ�����\n";
    std::cout << "2. ���ֽ���\n";
    std::cout << "3. �������\n";
    std::cout << "4. ȡ��\n";
    std::cout << "��ѡ��: ";

    int choice;
    if (!validateInt(choice)) return;

    switch (choice) {
    case 1: {
        reader.setFineAmount(0.0);
        std::cout << "��ȫ����ɷ��� " << currentFine << " Ԫ��" << std::endl;

        // ��Ǹ��û��ķ���֪ͨΪ�Ѷ�
        markFineNotificationsAsPaid(reader);
        break;
    }
    case 2: {
        double payAmount;
        std::cout << "������ɽ��: ";
        if (std::cin >> payAmount && payAmount > 0 && payAmount <= currentFine) {
            if (reader.payFine(payAmount)) {
                std::cout << "�ѽ��� " << payAmount << " Ԫ��ʣ�෣��: "
                    << reader.getFineAmount() << " Ԫ��" << std::endl;

                if (reader.getFineAmount() == 0) {
                    markFineNotificationsAsPaid(reader);
                }
            }
        }
        else {
            std::cout << "��Ч�Ľ��ɽ��!" << std::endl;
        }
        break;
    }
    case 3: {
        reader.setFineAmount(0.0);
        std::cout << "��������� " << currentFine << " Ԫ��" << std::endl;
        markFineNotificationsAsPaid(reader);
        break;
    }
    case 4:
        return;
    default:
        std::cout << "��Чѡ��!" << std::endl;
    }
}

// ����Ա���ܣ�ɾ�������˺�
void ReaderInfoPage::deleteReader() {
    clearscreen();
    std::cout << "=== ɾ�������˺� ===" << std::endl;

    std::string username;
    std::cin.ignore();
    std::cout << "����Ҫɾ�����û���: ";
    std::getline(std::cin, username);

    auto it = std::find_if(readers.begin(), readers.end(),
        [&username](const Reader& r) { return r.getUsername() == username; });

    if (it == readers.end()) {
        std::cout << "�Ҳ������û�!" << std::endl;
        return;
    }

    const Reader& reader = *it;
    std::cout << "����ɾ���û�:\n" << reader.getDetailedInfo() << std::endl;

    std::cout << "\nȷ��ɾ��? (���� 'DELETE' ȷ��): ";
    std::string confirmation;
    std::getline(std::cin, confirmation);

    if (confirmation == "DELETE") {
        readers.erase(it);
        std::cout << "�û��˺���ɾ����" << std::endl;
    }
    else {
        std::cout << "ɾ��������ȡ����" << std::endl;
    }
}

// ��ͨ�û����ܣ��鿴������Ϣ
void ReaderInfoPage::showPersonalInfo() const {
    clearscreen();
    std::cout << "=== ������Ϣ ===" << std::endl;

    // ���ҵ�ǰ�û�����ϸ��Ϣ
    auto it = std::find_if(readers.begin(), readers.end(),
        [this](const Reader& r) { return r.getUsername() == currentUser->getUsername(); });

    if (it != readers.end()) {
        std::cout << it->getDetailedInfo() << std::endl;
    }
    else {
        std::cout << "�޷��ҵ�������ϸ��Ϣ��" << std::endl;
    }
}

// ��ͨ�û����ܣ��޸��û���
void ReaderInfoPage::modifyUsername() {
    clearscreen();
    std::cout << "=== �޸��û��� ===" << std::endl;

    std::string newUsername;
    clearInputBuffer();
    std::cout << "�����µ��û���: ";
    std::getline(std::cin, newUsername);

    // ������û����Ƿ��Ѵ���
    auto it = std::find_if(readers.begin(), readers.end(),
        [&newUsername](const Reader& r) { return r.getUsername() == newUsername; });

    if (it != readers.end()) {
        std::cout << "�û����Ѵ��ڣ���ѡ�������û���!" << std::endl;
        return;
    }

    // ���ҵ�ǰ�û����޸��û���
    auto currentIt = std::find_if(readers.begin(), readers.end(),
        [this](const Reader& r) { return r.getUsername() == currentUser->getUsername(); });

    if (currentIt != readers.end()) {
		currentIt->changeUsername(newUsername);
    }
}

// ��ͨ�û����ܣ��޸�����
void ReaderInfoPage::modifyPassword() {
    clearscreen();
    std::cout << "=== �޸����� ===" << std::endl;

    std::string oldPassword, newPassword;
    clearInputBuffer();
    std::cout << "���뵱ǰ����: ";
    std::getline(std::cin, oldPassword);

    if (oldPassword != currentUser->getPassword()) {
        std::cout << "��ǰ�������!" << std::endl;
        return;
    }

    std::cout << "����������: ";
    std::getline(std::cin, newPassword);

    if (newPassword.length() < 6) {
        std::cout << "���볤������Ϊ6λ!" << std::endl;
        return;
    }

    currentUser->changePassword(newPassword);
    std::cout << "�����޸ĳɹ�!" << std::endl;
}

// ������������Ƿ���֪ͨΪ�ѽ���
void ReaderInfoPage::markFineNotificationsAsPaid(Reader& reader) {
    // ������û������з���֪ͨ
    auto notifications = reader.getAllNotifications();
    for (auto& noti : notifications) {
        if (noti.getType() == Notification::OVERDUE_FINE) {
            noti.markAsRead(); // ���Ϊ�Ѷ�
        }
    }
    std::cout << "��ط���֪ͨ�Ѵ���" << std::endl;
}
