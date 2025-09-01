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
        // ������־�ļ��������ڵ�ǰ���ڣ�
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

    // ��������
SystemLogger::~SystemLogger() {
        log(LogLevel::INFO, "SYSTEM", "ϵͳ��־��¼���ر�");
    }

    // ��¼��־����Ҫ����
    void SystemLogger::log(LogLevel level, const std::string& module, const std::string& message) {
        std::lock_guard<std::mutex> lock(logMutex);

        std::ofstream logFile(logFileName, std::ios::app);
        if (!logFile.is_open()) {
            std::cerr << "�޷�����־�ļ�: " << logFileName << std::endl;
            return;
        }

        // ��ȡ��ǰʱ��
        auto now = std::time(nullptr);
        auto localTime = *std::localtime(&now);

        // ��ʽ����־��Ŀ
        logFile << "[" << std::put_time(&localTime, "%Y-%m-%d %H:%M:%S") << "] "
            << "[" << getLevelString(level) << "] "
            << "[" << module << "] "
            << message << std::endl;

        logFile.close();

        // ����Ǵ�������ش���ͬʱ���������̨
        if (level >= LogLevel::ERROR) {
            std::cerr << "[" << std::put_time(&localTime, "%Y-%m-%d %H:%M:%S") << "] "
                << "[" << getLevelString(level) << "] "
                << "[" << module << "] "
                << message << std::endl;
        }
    }

    // ��ݷ���
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

    // ϵͳ������־
    void  SystemLogger::logSystemStart() {
        log(LogLevel::INFO, "SYSTEM", "==== ͼ�����ϵͳ���� ====");
        log(LogLevel::INFO, "SYSTEM", "�汾: 2.0");
        log(LogLevel::INFO, "SYSTEM", "����ʱ��: " + getCurrentTimeString());
    }

    // ϵͳ�ر���־
    void  SystemLogger::logSystemShutdown() {
        log(LogLevel::INFO, "SYSTEM", "ϵͳ�����ر�");
        log(LogLevel::INFO, "SYSTEM", "==== ͼ�����ϵͳ�ر� ====");
    }

    // �û�������־
    void  SystemLogger::logUserLogin(const std::string& username, bool isAdmin) {
        std::string userType = isAdmin ? "����Ա" : "��ͨ�û�";
        log(LogLevel::INFO, "AUTH", "�û���¼: " + username + " (" + userType + ")");
    }

    void  SystemLogger::logUserLogout(const std::string& username) {
        log(LogLevel::INFO, "AUTH", "�û��ǳ�: " + username);
    }

    // ͼ�������־
    void  SystemLogger::logBookBorrow(const std::string& username, const std::string& isbn, const std::string& title) {
        log(LogLevel::INFO, "BORROW", "����: �û�[" + username + "] ����[" + isbn + "] " + title);
    }

    void  SystemLogger::logBookReturn(const std::string& username, const std::string& isbn, const std::string& title) {
        log(LogLevel::INFO, "RETURN", "����: �û�[" + username + "] �黹[" + isbn + "] " + title);
    }

    void  SystemLogger::logBookReservation(const std::string& username, const std::string& isbn, const std::string& title) {
        log(LogLevel::INFO, "RESERVE", "ԤԼ: �û�[" + username + "] ԤԼ[" + isbn + "] " + title);
    }

    void  SystemLogger::logBookRenewal(const std::string& username, const std::string& isbn, const std::string& title) {
        log(LogLevel::INFO, "RENEW", "����: �û�[" + username + "] ����[" + isbn + "] " + title);
    }

    void  SystemLogger::logBookReview(const std::string& username, const std::string& isbn, int rating) {
        log(LogLevel::INFO, "REVIEW", "����: �û�[" + username + "] ��[" + isbn + "] ����:" + std::to_string(rating) + "��");
    }

    // ����Ա������־
    void  SystemLogger::logBookAdd(const std::string& adminUser, const std::string& isbn, const std::string& title) {
        log(LogLevel::INFO, "ADMIN", "���ͼ��: ����Ա[" + adminUser + "] ���[" + isbn + "] " + title);
    }

    void  SystemLogger::logBookDelete(const std::string& adminUser, const std::string& isbn, const std::string& title) {
        log(LogLevel::WARNING, "ADMIN", "ɾ��ͼ��: ����Ա[" + adminUser + "] ɾ��[" + isbn + "] " + title);
    }

    void  SystemLogger::logBookModify(const std::string& adminUser, const std::string& isbn, const std::string& field) {
        log(LogLevel::INFO, "ADMIN", "�޸�ͼ��: ����Ա[" + adminUser + "] �޸�[" + isbn + "] �� " + field);
    }

    void SystemLogger::logUserRegister(const std::string& adminUser, const std::string& newUsername) {
        log(LogLevel::INFO, "ADMIN", "ע���û�: ����Ա[" + adminUser + "] ע�����û�[" + newUsername + "]");
    }

    void SystemLogger::logUserDelete(const std::string& adminUser, const std::string& deletedUsername) {
        log(LogLevel::WARNING, "ADMIN", "ɾ���û�: ����Ա[" + adminUser + "] ɾ���û�[" + deletedUsername + "]");
    }

    void SystemLogger::logFinePayment(const std::string& adminUser, const std::string& username, double amount) {
        log(LogLevel::INFO, "FINE", "�������: ����Ա[" + adminUser + "] Ϊ�û�[" + username + "] ������ " + std::to_string(amount) + " Ԫ");
    }

    // ���ݲ�����־
    void SystemLogger::logDataSave(const std::string& filename, int recordCount) {
        log(LogLevel::INFO, "DATA", "���ݱ���: " + filename + " ������ " + std::to_string(recordCount) + " ����¼");
    }

    void SystemLogger::logDataLoad(const std::string& filename, int recordCount) {
        log(LogLevel::INFO, "DATA", "���ݼ���: " + filename + " ������ " + std::to_string(recordCount) + " ����¼");
    }

    void SystemLogger::logDataLoadError(const std::string& filename, const std::string& error) {
        log(LogLevel::ERROR, "DATA", "���ݼ���ʧ��: " + filename + " - " + error);
    }

    void SystemLogger::logDataSaveError(const std::string& filename, const std::string& error) {
        log(LogLevel::ERROR, "DATA", "���ݱ���ʧ��: " + filename + " - " + error);
    }

    // ������־
    void SystemLogger::logInputError(const std::string& module, const std::string& expectedInput, const std::string& actualInput) {
        log(LogLevel::WARNING, module, "�������: ����[" + expectedInput + "] ʵ������[" + actualInput + "]");
    }

    void SystemLogger::logValidationError(const std::string& module, const std::string& field, const std::string& value) {
        log(LogLevel::WARNING, module, "��֤ʧ��: �ֶ�[" + field + "] ֵ[" + value + "]");
    }

    void SystemLogger::logSystemError(const std::string& module, const std::string& error) {
        log(LogLevel::ERROR, module, "ϵͳ����: " + error);
    }

    // ֪ͨϵͳ��־
    void SystemLogger::logNotificationSent(const std::string& username, const std::string& notificationType) {
        log(LogLevel::INFO, "NOTIFICATION", "����֪ͨ: �û�[" + username + "] ����[" + notificationType + "]");
    }

    void SystemLogger::logNotificationCleanup(int removedCount) {
        log(LogLevel::INFO, "NOTIFICATION", "֪ͨ����: ɾ���� " + std::to_string(removedCount) + " ������֪ͨ");
    }

    // ��ȡ��־�ļ���
    std::string SystemLogger::getLogFileName() const {
        return logFileName;
    }

    // �������־�ļ�����ѡ���ܣ�
    void SystemLogger::cleanupOldLogs(int daysToKeep ) {
        log(LogLevel::INFO, "MAINTENANCE", "��ʼ������ " + std::to_string(daysToKeep) + " �����־�ļ�");
        // �������ʵ��ɾ������־�ļ����߼�
        // ʵ��ʵ����Ҫ����Ŀ¼������ļ��������ڵ�
    }


    // ��ȡ��־�����ַ���
    std::string SystemLogger::getLevelString(LogLevel level) const {
        switch (level) {
        case LogLevel::INFO: return "INFO";
        case LogLevel::WARNING: return "WARN";
        case LogLevel::ERROR: return "ERROR";
        case LogLevel::CRITICAL: return "CRIT";
        default: return "UNKNOWN";
        }
    }

    // ��ȡ��ǰʱ���ַ���
    std::string SystemLogger::getCurrentTimeString() const {
        auto now = std::time(nullptr);
        auto localTime = *std::localtime(&now);
        std::ostringstream oss;
        oss << std::put_time(&localTime, "%Y-%m-%d %H:%M:%S");
        return oss.str();
    }


    SystemLogger* SystemLogger::instance = nullptr; 