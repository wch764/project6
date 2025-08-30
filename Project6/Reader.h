#pragma once
#include"User.h"
class Reader :public User
{
private:
	const bool isadmin = false;
public:
	Reader(const std::string& uname="", const std::string& pwd="", const std::string& nm="", const bool isadmin = false)
		: User(uname, pwd,nm) {
	}
	bool get_isadmin() override{ return isadmin; }
	
};