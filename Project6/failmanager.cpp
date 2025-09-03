
#include <iostream>
#include <limits>
#include <string>   
#include "systemlogger.h"
#include"failmanager.h"
#include <conio.h>
using namespace std;
bool validateInt(int& result, const string& prompt ) {
    string input;

    if (!prompt.empty()) {
        cout << prompt;
    }

    if (!getline(cin, input)) {
        return false;
    }

    // ȥ����β�ո�
    input.erase(0, input.find_first_not_of(" \t\r\n"));
    input.erase(input.find_last_not_of(" \t\r\n") + 1);

    if (input.empty()) {
        cout << "���벻��Ϊ�գ�" << endl;
        return false;
    }

    try {
        size_t pos;
        result = stoi(input, &pos);

        // ����Ƿ������ַ�������ת��
        if (pos != input.length()) {
            SystemLogger::getInstance()->logInputError("INPUT", "����", input);
            cout << "������Ч�������봿���֣�" << endl;
            return false;
        }

        return true;
    }
    catch (const exception& e) {
        SystemLogger::getInstance()->logInputError("INPUT", "����", input);
        cout << "������Ч�����������֣�" << endl;
        return false;
    }
}
void clearInputBuffer() {
    if (!_kbhit()) {
        // ������Ϊ�գ���������
        return;
    }

    // �����������ݣ����
    while (_kbhit()) {
        _getch(); // ��ȡ�������ַ�
    }

    std::cout << "�����������" << std::endl;
}

