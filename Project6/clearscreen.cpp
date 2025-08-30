#include<iostream>
void clearscreen() {
    std::cout << "\033[2J\033[1;1H";  // ANSI 清屏指令
    // \033[2J : 清空屏幕
    // \033[1;1H : 移动光标到左上角
}