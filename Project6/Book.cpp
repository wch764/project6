#pragma once
#include"Serializable.h"
#include"Book.h"
#include<iostream>
#include<fstream>
#include<string>
using namespace std;
Book::Book(const string& is, const string& t , const string& a ,const string&k,const string &i, int q
, BookCategory cat)
        : ISBN(is), title(t), author(a),introduction(i),quantity(q), borrowed(0),category(cat){
    splitkeywords(k);
    }

    

    