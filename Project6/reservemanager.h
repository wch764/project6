#pragma once
#include <map>
#include <queue>
#include <mutex>
#include <algorithm>
#include "BorrowInfo.h"

class ReservationManager {
private:
    std::mutex mtx;
    // �洢borrowInfos�е�������������ָ��
    std::map<std::string, std::queue<size_t>> reservations;
    // ��ҪborrowInfos����������������
    std::vector<BorrowInfo>& borrowInfosRef;

public:
    // ���캯����ҪborrowInfos����
    ReservationManager(std::vector<BorrowInfo>& borrowInfos)
        : borrowInfosRef(borrowInfos) {
    }

    // ���ԤԼ - ʹ������
    bool addReservation(size_t borrowInfoIndex) {
        if (borrowInfoIndex >= borrowInfosRef.size()) {
            std::cout << "������Ч��ԤԼ��¼����" << std::endl;
            return false;
        }

        BorrowInfo& info = borrowInfosRef[borrowInfoIndex];
        if (!info.is_Reservation()) {
            std::cout << "���󣺲���ԤԼ��¼" << std::endl;
            return false;
        }

        std::lock_guard<std::mutex> lock(mtx);
        const std::string& isbn = info.getBookISBN();
        const std::string& username = info.getReaderUsername();

        std::cout << "=== ���ԤԼ���� ===" << std::endl;
        std::cout << "  - ISBN: [" << isbn << "]" << std::endl;
        std::cout << "  - �û���: [" << username << "]" << std::endl;
        std::cout << "  - ����: " << borrowInfoIndex << std::endl;
        std::cout << "  - ���: " << info.getReservationSequence() << std::endl;

        // ����Ƿ��Ѵ�����ͬ�û���ԤԼ
        auto it = reservations.find(isbn);
        if (it != reservations.end()) {
            std::cout << "  - ��ISBN����ԤԼ���У���С: " << it->second.size() << std::endl;

            // ���ƶ���������ظ�
            std::queue<size_t> tempQueue = it->second;
            while (!tempQueue.empty()) {
                size_t existingIndex = tempQueue.front();
                if (existingIndex < borrowInfosRef.size() &&
                    borrowInfosRef[existingIndex].getReaderUsername() == username) {
                    std::cout << "�����û� " << username << " ����ԤԼ��¼" << std::endl;
                    return false;
                }
                tempQueue.pop();
            }
        }

        // ��ӵ�����
        reservations[isbn].push(borrowInfoIndex);
        std::cout << "ԤԼ��ӳɹ�����ǰ���д�С: " << reservations[isbn].size() << std::endl;

        return true;
    }

    // �鿴��һ��ԤԼ - ��������
    size_t peekNextIndex(const std::string& isbn) {
        std::lock_guard<std::mutex> lock(mtx);

        cout << "=== peekNextIndex ������Ϣ ===" << endl;
        cout << "����ԤԼISBN: [" << isbn << "]" << endl;
        cout << "��ǰԤԼ��������: " << reservations.size() << endl;

        // ��ӡ���ж��еļ�
        for (const auto& entry : reservations) {
            cout << "���м�: [" << entry.first << "], ��С: " << entry.second.size() << endl;
        }

        auto it = reservations.find(isbn);
        if (it == reservations.end()) {
            cout << "û���ҵ���ISBN��ԤԼ����" << endl;
            return SIZE_MAX;
        }

        if (it->second.empty()) {
            cout << "ԤԼ����Ϊ�գ�ɾ���ն���" << endl;
            reservations.erase(it);
            return SIZE_MAX;
        }

        // �ҵ���һ����Ч��ԤԼ��¼
        while (!it->second.empty()) {
            size_t currentIndex = it->second.front();
            cout << "���ԤԼ����: " << currentIndex << endl;

            if (currentIndex >= borrowInfosRef.size()) {
                cout << "��Ч�������Ƴ�" << endl;
                it->second.pop();
                continue;
            }

            BorrowInfo& info = borrowInfosRef[currentIndex];
            cout << "ԤԼ��¼����: �û�=" << info.getReaderUsername()
                << ", ISBN=" << info.getBookISBN()
                << ", �Ƿ�ԤԼ=" << (info.is_Reservation() ? "��" : "��") << endl;

            // ���ԤԼ�Ƿ���Ч
            if (info.is_Reservation() && info.getBookISBN() == isbn) {
                cout << "�ҵ���ЧԤԼ: �û�=" << info.getReaderUsername() << endl;
                return currentIndex;
            }
            else {
                cout << "��ЧԤԼ��¼���Ƴ�: ";
                if (!info.is_Reservation()) cout << "����ԤԼ��¼ ";
                if (info.getBookISBN() != isbn) cout << "ISBN��ƥ�� ";
                cout << endl;
                it->second.pop();
            }
        }

        cout << "�����ѿգ�ɾ��" << endl;
        reservations.erase(it);
        return SIZE_MAX;
    }
    // ��ȡ��һ��ԤԼ��BorrowInfoָ��
    BorrowInfo* peekNext(const std::string& isbn) {
        size_t index = peekNextIndex(isbn);
        if (index == SIZE_MAX || index >= borrowInfosRef.size()) {
            return nullptr;
        }
        return &borrowInfosRef[index];
    }

    // ���ԤԼ
    bool fulfillReservation(const std::string& isbn) {
        std::lock_guard<std::mutex> lock(mtx);

        auto it = reservations.find(isbn);
        if (it == reservations.end() || it->second.empty()) {
            return false;
        }

        it->second.pop();
        if (it->second.empty()) {
            reservations.erase(it);
        }
        return true;
    }
    void rebuildFromBorrowInfos() {
        std::lock_guard<std::mutex> lock(mtx);

        cout << "=== ��ʼ�ؽ�ԤԼ���� ===" << endl;
        reservations.clear(); // ������ж���

        // �ռ�������Ч��ԤԼ��¼
        std::vector<std::pair<size_t, const BorrowInfo*>> validReservations;

        for (size_t i = 0; i < borrowInfosRef.size(); ++i) {
            const BorrowInfo& info = borrowInfosRef[i];

            if (info.is_Reservation() && !info.isReturned()) {
                validReservations.push_back({ i, &info });
                cout << "�ҵ���ЧԤԼ: ����=" << i
                    << ", �û�=" << info.getReaderUsername()
                    << ", ISBN=" << info.getBookISBN()
                    << ", ���=" << info.getReservationSequence() << endl;
            }
        }

        // ��ISBN���飬Ȼ��ԤԼ�������
        std::map<std::string, std::vector<std::pair<size_t, int>>> groupedReservations;

        for (const auto& pair : validReservations) {
            const BorrowInfo* info = pair.second;
            groupedReservations[info->getBookISBN()].push_back({ pair.first, info->getReservationSequence() });
        }

        // Ϊÿ��ISBN�ؽ�����
        for (auto& group : groupedReservations) {
            const std::string& isbn = group.first;
            auto& indices = group.second;

            // ��ԤԼ�������
            std::sort(indices.begin(), indices.end(),
                [](const auto& a, const auto& b) {
                    return a.second < b.second; // �������������
                });

            cout << "�ؽ�ISBN " << isbn << " ��ԤԼ����:" << endl;

            // ��ӵ�����
            for (const auto& indexPair : indices) {
                size_t index = indexPair.first;
                int sequence = indexPair.second;
                reservations[isbn].push(index);

                cout << "  - ���ԤԼ: ����=" << index
                    << ", ���=" << sequence
                    << ", �û�=" << borrowInfosRef[index].getReaderUsername() << endl;
            }
        }

        cout << "ԤԼ�����ؽ���ɣ����ؽ� " << reservations.size() << " ������" << endl;
        debugPrint();
    }
    // ��ȡ���д�С
    size_t getQueueSize(const std::string& isbn) {
        std::lock_guard<std::mutex> lock(mtx);
        auto it = reservations.find(isbn);
        return (it != reservations.end()) ? it->second.size() : 0;
    }

    // ���Դ�ӡ
    void debugPrint() const {
        
        std::cout << "=== ԤԼ���е�����Ϣ ===" << std::endl;
        for (const auto& entry : reservations) {
            std::cout << "ISBN: " << entry.first
                << ", ԤԼ��: " << entry.second.size() << std::endl;

            std::queue<size_t> tempQueue = entry.second;
            while (!tempQueue.empty()) {
                size_t index = tempQueue.front();
                if (index < borrowInfosRef.size()) {
                    const BorrowInfo& info = borrowInfosRef[index];
                    std::cout << "  - ����: " << index
                        << ", �û�: " << info.getReaderUsername()
                        << ", ���: " << info.getReservationSequence() << std::endl;
                }
                tempQueue.pop();
            }
        }
    }

    void processReturnedBook(const std::string& isbn, const std::string& title, User& reservedUser) {
        Date expiryDate = Date::currentdate();
        expiryDate.addDays(7);

        Notification noti(Notification::RESERVATION_AVAILABLE, isbn, title, expiryDate);
        reservedUser.addNotification(noti);
        std::cout << "�����û� " << reservedUser.getUsername() << " ����ԤԼ����֪ͨ" << std::endl;
    }
};