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
        if (cin.fail()) { // ����Ƿ�����ʧ�ܣ�����ĸ��
            cin.clear(); // �������״̬
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // ��ջ�����
            cerr << "������Ч�����������룡\n";
            continue;
        }
        return true;
    }
}