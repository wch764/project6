#pragma once
#include"Serializable.h"
#include"Book.h"
#include<iostream>
#include<fstream>
#include<string>
using namespace std;
Book::Book(const string& is, const string& t , const string& a ,const string&k,const string &i, int q)
        : ISBN(is), title(t), author(a),introduction(i),quantity(q), borrowed(0) {
    splitkeywords(k);
    }

    

    bool Book::borrowBook() {
        if (borrowed < quantity) {
            borrowed++;
            return true;
        }
        return false;
    }

    bool Book::returnBook() {
        if (borrowed > 0) {
            borrowed--;
            return true;
        }
        return false;
    }

    string Book::getISBN() const { return ISBN; }
    string Book::getTitle() const { return title; }
    string Book::getAuthor() const { return author; }
    int Book::getQuantity() const { return quantity; }
    int Book::getBorrowed() const { return borrowed; }
    int Book::getAvailable() const { return quantity - borrowed; }
