#pragma once
#include"Page.h"
#include"Book.h"
#include"clearscreen.h"
#include"failmanager.h"

#include<functional>
class BookDetailPage :public Page{
private:
    const Book& book;
    std::function<void(const string&)> borrowBookCallback;
public:
    BookDetailPage(const Book& b, std::function<void(const string&)> callback );

    bool display() const override;

private:
    void displayReviews() const;
    
};
