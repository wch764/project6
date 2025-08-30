#include<iostream>
#include<fstream>
#include<string>
using namespace std;
bool validateInt(int& num, const std::string& prompt = "") {
    while (true)
    {
        if (!prompt.empty()) cout << prompt;
        cin >> num;
        cin.ignore();
        if (cin.fail()) { // 检查是否输入失败（如字母）
            cin.clear(); // 清除错误状态
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // 清空缓冲区
            cerr << "输入无效，请重新输入！\n";
            continue;
        }
        return true;
    }
}