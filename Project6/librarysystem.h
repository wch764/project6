#pragma once
#include"reservemanager.h"
#include"MainMenuPage.h"
class LibrarySystem {
    void dailyCheck() {
        // ������ԤԼ
        reservationManager.checkExpiredReservations(books, users);

        // �����ճ�����...
    }
};