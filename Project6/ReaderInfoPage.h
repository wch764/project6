#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <iomanip>
#include "Page.h"
#include "Action.h"
#include "Reader.h"
#include "User.h"
#include "clearscreen.h"
#include "failmanager.h"

class ReaderInfoPage : public Page, public Action {
private:
    std::vector<Reader>& readers;
    User* currentUser;

public:
    ReaderInfoPage(std::vector<Reader>& rdrs, User* user);
    bool display() const override final;
    void performAction() override final;

private:
    // ����Ա���ܣ��鿴���ж�����Ϣ
    void showAllReaders() const;
    // ����Ա���ܣ�ע�����û�
    void registerNewUser();

    // ����Ա���ܣ��޸Ķ��߱�ҵ����
    void modifyGraduationDate();

    // ����Ա���ܣ����������
    void processFinePay();

    // ����Ա���ܣ�ɾ�������˺�
    void deleteReader();

    // ��ͨ�û����ܣ��鿴������Ϣ
    void showPersonalInfo() const;

    // ��ͨ�û����ܣ��޸��û���
    void modifyUsername();

    // ��ͨ�û����ܣ��޸�����
    void modifyPassword();

    // ������������Ƿ���֪ͨΪ�ѽ���
    void markFineNotificationsAsPaid(Reader& reader);
};