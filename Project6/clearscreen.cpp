#include<iostream>
void clearscreen() {
    std::cout << "\033[2J\033[1;1H";  // ANSI ����ָ��
    // \033[2J : �����Ļ
    // \033[1;1H : �ƶ���굽���Ͻ�
}