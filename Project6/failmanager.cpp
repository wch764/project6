
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

    // 去除首尾空格
    input.erase(0, input.find_first_not_of(" \t\r\n"));
    input.erase(input.find_last_not_of(" \t\r\n") + 1);

    if (input.empty()) {
        cout << "输入不能为空！" << endl;
        return false;
    }

    try {
        size_t pos;
        result = stoi(input, &pos);

        // 检查是否整个字符串都被转换
        if (pos != input.length()) {
            SystemLogger::getInstance()->logInputError("INPUT", "整数", input);
            cout << "输入无效，请输入纯数字！" << endl;
            return false;
        }

        return true;
    }
    catch (const exception& e) {
        SystemLogger::getInstance()->logInputError("INPUT", "整数", input);
        cout << "输入无效，请输入数字！" << endl;
        return false;
    }
}
void clearInputBuffer() {
    if (!_kbhit()) {
        // 缓冲区为空，立即返回
        return;
    }

    // 缓冲区有内容，清空
    while (_kbhit()) {
        _getch(); // 读取并丢弃字符
    }

    std::cout << "缓冲区已清空" << std::endl;
}

