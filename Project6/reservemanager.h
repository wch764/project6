#pragma once
#include <map>
#include <queue>
#include <mutex>
#include <algorithm>
#include "BorrowInfo.h"

class ReservationManager {
private:
    std::mutex mtx;
    // 存储borrowInfos中的索引，而不是指针
    std::map<std::string, std::queue<size_t>> reservations;
    // 需要borrowInfos的引用来访问数据
    std::vector<BorrowInfo>& borrowInfosRef;

public:
    // 构造函数需要borrowInfos引用
    ReservationManager(std::vector<BorrowInfo>& borrowInfos)
        : borrowInfosRef(borrowInfos) {
    }

    // 添加预约 - 使用索引
    bool addReservation(size_t borrowInfoIndex) {
        if (borrowInfoIndex >= borrowInfosRef.size()) {
            std::cout << "错误：无效的预约记录索引" << std::endl;
            return false;
        }

        BorrowInfo& info = borrowInfosRef[borrowInfoIndex];
        if (!info.is_Reservation()) {
            std::cout << "错误：不是预约记录" << std::endl;
            return false;
        }

        std::lock_guard<std::mutex> lock(mtx);
        const std::string& isbn = info.getBookISBN();
        const std::string& username = info.getReaderUsername();

        std::cout << "=== 添加预约详情 ===" << std::endl;
        std::cout << "  - ISBN: [" << isbn << "]" << std::endl;
        std::cout << "  - 用户名: [" << username << "]" << std::endl;
        std::cout << "  - 索引: " << borrowInfoIndex << std::endl;
        std::cout << "  - 序号: " << info.getReservationSequence() << std::endl;

        // 检查是否已存在相同用户的预约
        auto it = reservations.find(isbn);
        if (it != reservations.end()) {
            std::cout << "  - 该ISBN已有预约队列，大小: " << it->second.size() << std::endl;

            // 复制队列来检查重复
            std::queue<size_t> tempQueue = it->second;
            while (!tempQueue.empty()) {
                size_t existingIndex = tempQueue.front();
                if (existingIndex < borrowInfosRef.size() &&
                    borrowInfosRef[existingIndex].getReaderUsername() == username) {
                    std::cout << "错误：用户 " << username << " 已有预约记录" << std::endl;
                    return false;
                }
                tempQueue.pop();
            }
        }

        // 添加到队列
        reservations[isbn].push(borrowInfoIndex);
        std::cout << "预约添加成功！当前队列大小: " << reservations[isbn].size() << std::endl;

        return true;
    }

    // 查看下一个预约 - 返回索引
    size_t peekNextIndex(const std::string& isbn) {
        std::lock_guard<std::mutex> lock(mtx);

        cout << "=== peekNextIndex 调试信息 ===" << endl;
        cout << "查找预约ISBN: [" << isbn << "]" << endl;
        cout << "当前预约队列总数: " << reservations.size() << endl;

        // 打印所有队列的键
        for (const auto& entry : reservations) {
            cout << "队列键: [" << entry.first << "], 大小: " << entry.second.size() << endl;
        }

        auto it = reservations.find(isbn);
        if (it == reservations.end()) {
            cout << "没有找到该ISBN的预约队列" << endl;
            return SIZE_MAX;
        }

        if (it->second.empty()) {
            cout << "预约队列为空，删除空队列" << endl;
            reservations.erase(it);
            return SIZE_MAX;
        }

        // 找到第一个有效的预约记录
        while (!it->second.empty()) {
            size_t currentIndex = it->second.front();
            cout << "检查预约索引: " << currentIndex << endl;

            if (currentIndex >= borrowInfosRef.size()) {
                cout << "无效索引，移除" << endl;
                it->second.pop();
                continue;
            }

            BorrowInfo& info = borrowInfosRef[currentIndex];
            cout << "预约记录详情: 用户=" << info.getReaderUsername()
                << ", ISBN=" << info.getBookISBN()
                << ", 是否预约=" << (info.is_Reservation() ? "是" : "否") << endl;

            // 检查预约是否有效
            if (info.is_Reservation() && info.getBookISBN() == isbn) {
                cout << "找到有效预约: 用户=" << info.getReaderUsername() << endl;
                return currentIndex;
            }
            else {
                cout << "无效预约记录，移除: ";
                if (!info.is_Reservation()) cout << "不是预约记录 ";
                if (info.getBookISBN() != isbn) cout << "ISBN不匹配 ";
                cout << endl;
                it->second.pop();
            }
        }

        cout << "队列已空，删除" << endl;
        reservations.erase(it);
        return SIZE_MAX;
    }
    // 获取下一个预约的BorrowInfo指针
    BorrowInfo* peekNext(const std::string& isbn) {
        size_t index = peekNextIndex(isbn);
        if (index == SIZE_MAX || index >= borrowInfosRef.size()) {
            return nullptr;
        }
        return &borrowInfosRef[index];
    }

    // 完成预约
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

        cout << "=== 开始重建预约队列 ===" << endl;
        reservations.clear(); // 清空现有队列

        // 收集所有有效的预约记录
        std::vector<std::pair<size_t, const BorrowInfo*>> validReservations;

        for (size_t i = 0; i < borrowInfosRef.size(); ++i) {
            const BorrowInfo& info = borrowInfosRef[i];

            if (info.is_Reservation() && !info.isReturned()) {
                validReservations.push_back({ i, &info });
                cout << "找到有效预约: 索引=" << i
                    << ", 用户=" << info.getReaderUsername()
                    << ", ISBN=" << info.getBookISBN()
                    << ", 序号=" << info.getReservationSequence() << endl;
            }
        }

        // 按ISBN分组，然后按预约序号排序
        std::map<std::string, std::vector<std::pair<size_t, int>>> groupedReservations;

        for (const auto& pair : validReservations) {
            const BorrowInfo* info = pair.second;
            groupedReservations[info->getBookISBN()].push_back({ pair.first, info->getReservationSequence() });
        }

        // 为每个ISBN重建队列
        for (auto& group : groupedReservations) {
            const std::string& isbn = group.first;
            auto& indices = group.second;

            // 按预约序号排序
            std::sort(indices.begin(), indices.end(),
                [](const auto& a, const auto& b) {
                    return a.second < b.second; // 按序号升序排列
                });

            cout << "重建ISBN " << isbn << " 的预约队列:" << endl;

            // 添加到队列
            for (const auto& indexPair : indices) {
                size_t index = indexPair.first;
                int sequence = indexPair.second;
                reservations[isbn].push(index);

                cout << "  - 添加预约: 索引=" << index
                    << ", 序号=" << sequence
                    << ", 用户=" << borrowInfosRef[index].getReaderUsername() << endl;
            }
        }

        cout << "预约队列重建完成，共重建 " << reservations.size() << " 个队列" << endl;
        debugPrint();
    }
    // 获取队列大小
    size_t getQueueSize(const std::string& isbn) {
        std::lock_guard<std::mutex> lock(mtx);
        auto it = reservations.find(isbn);
        return (it != reservations.end()) ? it->second.size() : 0;
    }

    // 调试打印
    void debugPrint() const {
        
        std::cout << "=== 预约队列调试信息 ===" << std::endl;
        for (const auto& entry : reservations) {
            std::cout << "ISBN: " << entry.first
                << ", 预约数: " << entry.second.size() << std::endl;

            std::queue<size_t> tempQueue = entry.second;
            while (!tempQueue.empty()) {
                size_t index = tempQueue.front();
                if (index < borrowInfosRef.size()) {
                    const BorrowInfo& info = borrowInfosRef[index];
                    std::cout << "  - 索引: " << index
                        << ", 用户: " << info.getReaderUsername()
                        << ", 序号: " << info.getReservationSequence() << std::endl;
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
        std::cout << "已向用户 " << reservedUser.getUsername() << " 发送预约可用通知" << std::endl;
    }
};