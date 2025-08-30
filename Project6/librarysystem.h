#pragma once
#include"reservemanager.h"
#include"MainMenuPage.h"
class LibrarySystem {
    void dailyCheck() {
        // 检查过期预约
        reservationManager.checkExpiredReservations(books, users);

        // 其他日常任务...
    }
};