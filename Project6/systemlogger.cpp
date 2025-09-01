#pragma warning(disable: 4996)
#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <mutex>
#include "Date.h"
#include"systemlogger.h"




SystemLogger:: SystemLogger() {
        // 生成日志文件名（基于当前日期）
        Date currentDate = Date::currentdate();
        std::ostringstream oss;
        oss << "system_log_" << currentDate.getYear() << "_"
            << std::setfill('0') << std::setw(2) << currentDate.getMonth() << "_"
            << std::setfill('0') << std::setw(2) << currentDate.getDay() << ".log";
        logFileName = oss.str();
    }


SystemLogger* SystemLogger::getInstance() {
        if (instance == nullptr) {
            instance = new SystemLogger();
        }
        return instance;
    }

    // 析构函数
SystemLogger::~SystemLogger() {
        log(LogLevel::INFO, "SYSTEM", "系统日志记录器关闭");
    }

    // 记录日志的主要方法
    void SystemLogger::log(LogLevel level, const std::string& module, const std::string& message) {
        std::lock_guard<std::mutex> lock(logMutex);

        std::ofstream logFile(logFileName, std::ios::app);
        if (!logFile.is_open()) {
            std::cerr << "无法打开日志文件: " << logFileName << std::endl;
            return;
        }

        // 获取当前时间
        auto now = std::time(nullptr);
        auto localTime = *std::localtime(&now);

        // 格式化日志条目
        logFile << "[" << std::put_time(&localTime, "%Y-%m-%d %H:%M:%S") << "] "
            << "[" << getLevelString(level) << "] "
            << "[" << module << "] "
            << message << std::endl;

        logFile.close();

        // 如果是错误或严重错误，同时输出到控制台
        if (level >= LogLevel::ERROR) {
            std::cerr << "[" << std::put_time(&localTime, "%Y-%m-%d %H:%M:%S") << "] "
                << "[" << getLevelString(level) << "] "
                << "[" << module << "] "
                << message << std::endl;
        }
    }

    // 便捷方法
    void  SystemLogger::logInfo(const std::string& module, const std::string& message) {
        log(LogLevel::INFO, module, message);
    }

    void  SystemLogger::logWarning(const std::string& module, const std::string& message) {
        log(LogLevel::WARNING, module, message);
    }

    void  SystemLogger::logError(const std::string& module, const std::string& message) {
        log(LogLevel::ERROR, module, message);
    }

    void  SystemLogger::logCritical(const std::string& module, const std::string& message) {
        log(LogLevel::CRITICAL, module, message);
    }

    // 系统启动日志
    void  SystemLogger::logSystemStart() {
        log(LogLevel::INFO, "SYSTEM", "==== 图书管理系统启动 ====");
        log(LogLevel::INFO, "SYSTEM", "版本: 2.0");
        log(LogLevel::INFO, "SYSTEM", "启动时间: " + getCurrentTimeString());
    }

    // 系统关闭日志
    void  SystemLogger::logSystemShutdown() {
        log(LogLevel::INFO, "SYSTEM", "系统正常关闭");
        log(LogLevel::INFO, "SYSTEM", "==== 图书管理系统关闭 ====");
    }

    // 用户操作日志
    void  SystemLogger::logUserLogin(const std::string& username, bool isAdmin) {
        std::string userType = isAdmin ? "管理员" : "普通用户";
        log(LogLevel::INFO, "AUTH", "用户登录: " + username + " (" + userType + ")");
    }

    void  SystemLogger::logUserLogout(const std::string& username) {
        log(LogLevel::INFO, "AUTH", "用户登出: " + username);
    }

    // 图书操作日志
    void  SystemLogger::logBookBorrow(const std::string& username, const std::string& isbn, const std::string& title) {
        log(LogLevel::INFO, "BORROW", "借书: 用户[" + username + "] 借阅[" + isbn + "] " + title);
    }

    void  SystemLogger::logBookReturn(const std::string& username, const std::string& isbn, const std::string& title) {
        log(LogLevel::INFO, "RETURN", "还书: 用户[" + username + "] 归还[" + isbn + "] " + title);
    }

    void  SystemLogger::logBookReservation(const std::string& username, const std::string& isbn, const std::string& title) {
        log(LogLevel::INFO, "RESERVE", "预约: 用户[" + username + "] 预约[" + isbn + "] " + title);
    }

    void  SystemLogger::logBookRenewal(const std::string& username, const std::string& isbn, const std::string& title) {
        log(LogLevel::INFO, "RENEW", "续借: 用户[" + username + "] 续借[" + isbn + "] " + title);
    }

    void  SystemLogger::logBookReview(const std::string& username, const std::string& isbn, int rating) {
        log(LogLevel::INFO, "REVIEW", "评价: 用户[" + username + "] 对[" + isbn + "] 评分:" + std::to_string(rating) + "星");
    }

    // 管理员操作日志
    void  SystemLogger::logBookAdd(const std::string& adminUser, const std::string& isbn, const std::string& title) {
        log(LogLevel::INFO, "ADMIN", "添加图书: 管理员[" + adminUser + "] 添加[" + isbn + "] " + title);
    }

    void  SystemLogger::logBookDelete(const std::string& adminUser, const std::string& isbn, const std::string& title) {
        log(LogLevel::WARNING, "ADMIN", "删除图书: 管理员[" + adminUser + "] 删除[" + isbn + "] " + title);
    }

    void  SystemLogger::logBookModify(const std::string& adminUser, const std::string& isbn, const std::string& field) {
        log(LogLevel::INFO, "ADMIN", "修改图书: 管理员[" + adminUser + "] 修改[" + isbn + "] 的 " + field);
    }

    void SystemLogger::logUserRegister(const std::string& adminUser, const std::string& newUsername) {
        log(LogLevel::INFO, "ADMIN", "注册用户: 管理员[" + adminUser + "] 注册新用户[" + newUsername + "]");
    }

    void SystemLogger::logUserDelete(const std::string& adminUser, const std::string& deletedUsername) {
        log(LogLevel::WARNING, "ADMIN", "删除用户: 管理员[" + adminUser + "] 删除用户[" + deletedUsername + "]");
    }

    void SystemLogger::logFinePayment(const std::string& adminUser, const std::string& username, double amount) {
        log(LogLevel::INFO, "FINE", "罚款缴纳: 管理员[" + adminUser + "] 为用户[" + username + "] 处理罚款 " + std::to_string(amount) + " 元");
    }

    // 数据操作日志
    void SystemLogger::logDataSave(const std::string& filename, int recordCount) {
        log(LogLevel::INFO, "DATA", "数据保存: " + filename + " 保存了 " + std::to_string(recordCount) + " 条记录");
    }

    void SystemLogger::logDataLoad(const std::string& filename, int recordCount) {
        log(LogLevel::INFO, "DATA", "数据加载: " + filename + " 加载了 " + std::to_string(recordCount) + " 条记录");
    }

    void SystemLogger::logDataLoadError(const std::string& filename, const std::string& error) {
        log(LogLevel::ERROR, "DATA", "数据加载失败: " + filename + " - " + error);
    }

    void SystemLogger::logDataSaveError(const std::string& filename, const std::string& error) {
        log(LogLevel::ERROR, "DATA", "数据保存失败: " + filename + " - " + error);
    }

    // 错误日志
    void SystemLogger::logInputError(const std::string& module, const std::string& expectedInput, const std::string& actualInput) {
        log(LogLevel::WARNING, module, "输入错误: 期望[" + expectedInput + "] 实际输入[" + actualInput + "]");
    }

    void SystemLogger::logValidationError(const std::string& module, const std::string& field, const std::string& value) {
        log(LogLevel::WARNING, module, "验证失败: 字段[" + field + "] 值[" + value + "]");
    }

    void SystemLogger::logSystemError(const std::string& module, const std::string& error) {
        log(LogLevel::ERROR, module, "系统错误: " + error);
    }

    // 通知系统日志
    void SystemLogger::logNotificationSent(const std::string& username, const std::string& notificationType) {
        log(LogLevel::INFO, "NOTIFICATION", "发送通知: 用户[" + username + "] 类型[" + notificationType + "]");
    }

    void SystemLogger::logNotificationCleanup(int removedCount) {
        log(LogLevel::INFO, "NOTIFICATION", "通知清理: 删除了 " + std::to_string(removedCount) + " 条过期通知");
    }

    // 获取日志文件名
    std::string SystemLogger::getLogFileName() const {
        return logFileName;
    }

    // 清理旧日志文件（可选功能）
    void SystemLogger::cleanupOldLogs(int daysToKeep ) {
        log(LogLevel::INFO, "MAINTENANCE", "开始清理超过 " + std::to_string(daysToKeep) + " 天的日志文件");
        // 这里可以实现删除旧日志文件的逻辑
        // 实际实现需要遍历目录，检查文件创建日期等
    }


    // 获取日志级别字符串
    std::string SystemLogger::getLevelString(LogLevel level) const {
        switch (level) {
        case LogLevel::INFO: return "INFO";
        case LogLevel::WARNING: return "WARN";
        case LogLevel::ERROR: return "ERROR";
        case LogLevel::CRITICAL: return "CRIT";
        default: return "UNKNOWN";
        }
    }

    // 获取当前时间字符串
    std::string SystemLogger::getCurrentTimeString() const {
        auto now = std::time(nullptr);
        auto localTime = *std::localtime(&now);
        std::ostringstream oss;
        oss << std::put_time(&localTime, "%Y-%m-%d %H:%M:%S");
        return oss.str();
    }


    SystemLogger* SystemLogger::instance = nullptr; 