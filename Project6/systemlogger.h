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
    INFO = 0,       // һ����Ϣ
    WARNING = 1,    // ����
    ERROR = 2,      // ����
    CRITICAL = 3    // ���ش���
};

class SystemLogger {
private:
    std::string logFileName;
    std::mutex logMutex;
    static SystemLogger* instance;

    SystemLogger();
public:
    // ����ģʽ
    static SystemLogger* getInstance();
    // ��������
    ~SystemLogger();
    // ��¼��־����Ҫ����
    void log(LogLevel level, const std::string& module, const std::string& message);

    // ��ݷ���
    void logInfo(const std::string& module, const std::string& message);
    void logWarning(const std::string& module, const std::string& message);

    void logError(const std::string& module, const std::string& message);

    void logCritical(const std::string& module, const std::string& message);

    // ϵͳ������־
    void logSystemStart();
    // ϵͳ�ر���־
    void logSystemShutdown();

    // �û�������־
    void logUserLogin(const std::string& username, bool isAdmin);

    void logUserLogout(const std::string& username);
    // ͼ�������־
    void logBookBorrow(const std::string& username, const std::string& isbn, const std::string& title);
    void logBookReturn(const std::string& username, const std::string& isbn, const std::string& title);

    void logBookReservation(const std::string& username, const std::string& isbn, const std::string& title);
    void logBookRenewal(const std::string& username, const std::string& isbn, const std::string& title);

    void logBookReview(const std::string& username, const std::string& isbn, int rating);
    // ����Ա������־
    void logBookAdd(const std::string& adminUser, const std::string& isbn, const std::string& title);

    void logBookDelete(const std::string& adminUser, const std::string& isbn, const std::string& title);

    void logBookModify(const std::string& adminUser, const std::string& isbn, const std::string& field);

    void logUserRegister(const std::string& adminUser, const std::string& newUsername);

    void logUserDelete(const std::string& adminUser, const std::string& deletedUsername);
    void logFinePayment(const std::string& adminUser, const std::string& username, double amount) 
        ;

    // ���ݲ�����־
    void logDataSave(const std::string & filename, int recordCount);
    void logDataLoad(const std::string & filename, int recordCount);

    void logDataLoadError(const std::string & filename, const std::string & error);

    void logDataSaveError(const std::string & filename, const std::string & error);

    // ������־
    void logInputError(const std::string & module, const std::string & expectedInput, const std::string & actualInput);
    void logValidationError(const std::string & module, const std::string & field, const std::string & value);

    void logSystemError(const std::string & module, const std::string & error);
    // ֪ͨϵͳ��־
    void logNotificationSent(const std::string & username, const std::string & notificationType);
    void logNotificationCleanup(int removedCount);
    // ��ȡ��־�ļ���
    std::string getLogFileName() const;

    // �������־�ļ�����ѡ���ܣ�
    void cleanupOldLogs(int daysToKeep = 30);

private:
    // ��ȡ��־�����ַ���
    std::string getLevelString(LogLevel level) const;

    // ��ȡ��ǰʱ���ַ���
    std::string getCurrentTimeString() const;
};



// ȫ����־�궨�壬����ʹ��
#define LOG_INFO(module, message) SystemLogger::getInstance()->logInfo(module, message)
#define LOG_WARNING(module, message) SystemLogger::getInstance()->logWarning(module, message)
#define LOG_ERROR(module, message) SystemLogger::getInstance()->logError(module, message)
#define LOG_CRITICAL(module, message) SystemLogger::getInstance()->logCritical(module, message)