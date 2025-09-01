#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <mutex>
#include "Date.h"


enum class LogLevel {
    INFO = 0,       // 一般信息
    WARNING = 1,    // 警告
    ERROR = 2,      // 错误
    CRITICAL = 3    // 严重错误
};

class SystemLogger {
private:
    std::string logFileName;
    std::mutex logMutex;
    static SystemLogger* instance;

    SystemLogger();
public:
    // 单例模式
    static SystemLogger* getInstance();
    // 析构函数
    ~SystemLogger();
    // 记录日志的主要方法
    void log(LogLevel level, const std::string& module, const std::string& message);

    // 便捷方法
    void logInfo(const std::string& module, const std::string& message);
    void logWarning(const std::string& module, const std::string& message);

    void logError(const std::string& module, const std::string& message);

    void logCritical(const std::string& module, const std::string& message);

    // 系统启动日志
    void logSystemStart();
    // 系统关闭日志
    void logSystemShutdown();

    // 用户操作日志
    void logUserLogin(const std::string& username, bool isAdmin);

    void logUserLogout(const std::string& username);
    // 图书操作日志
    void logBookBorrow(const std::string& username, const std::string& isbn, const std::string& title);
    void logBookReturn(const std::string& username, const std::string& isbn, const std::string& title);

    void logBookReservation(const std::string& username, const std::string& isbn, const std::string& title);
    void logBookRenewal(const std::string& username, const std::string& isbn, const std::string& title);

    void logBookReview(const std::string& username, const std::string& isbn, int rating);
    // 管理员操作日志
    void logBookAdd(const std::string& adminUser, const std::string& isbn, const std::string& title);

    void logBookDelete(const std::string& adminUser, const std::string& isbn, const std::string& title);

    void logBookModify(const std::string& adminUser, const std::string& isbn, const std::string& field);

    void logUserRegister(const std::string& adminUser, const std::string& newUsername);

    void logUserDelete(const std::string& adminUser, const std::string& deletedUsername);
    void logFinePayment(const std::string& adminUser, const std::string& username, double amount) 
        ;

    // 数据操作日志
    void logDataSave(const std::string & filename, int recordCount);
    void logDataLoad(const std::string & filename, int recordCount);

    void logDataLoadError(const std::string & filename, const std::string & error);

    void logDataSaveError(const std::string & filename, const std::string & error);

    // 错误日志
    void logInputError(const std::string & module, const std::string & expectedInput, const std::string & actualInput);
    void logValidationError(const std::string & module, const std::string & field, const std::string & value);

    void logSystemError(const std::string & module, const std::string & error);
    // 通知系统日志
    void logNotificationSent(const std::string & username, const std::string & notificationType);
    void logNotificationCleanup(int removedCount);
    // 获取日志文件名
    std::string getLogFileName() const;

    // 清理旧日志文件（可选功能）
    void cleanupOldLogs(int daysToKeep = 30);

private:
    // 获取日志级别字符串
    std::string getLevelString(LogLevel level) const;

    // 获取当前时间字符串
    std::string getCurrentTimeString() const;
};



// 全局日志宏定义，方便使用
#define LOG_INFO(module, message) SystemLogger::getInstance()->logInfo(module, message)
#define LOG_WARNING(module, message) SystemLogger::getInstance()->logWarning(module, message)
#define LOG_ERROR(module, message) SystemLogger::getInstance()->logError(module, message)
#define LOG_CRITICAL(module, message) SystemLogger::getInstance()->logCritical(module, message)