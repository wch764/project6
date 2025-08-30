#pragma once
#include"User.h"
class Admin :public User
{
private:
	const bool isadmin = true;
public:
	Admin(const std::string& uname="", const std::string& pwd="",const std::string nm="",const bool isadmin=true)
		: User(uname, pwd) {
	}
	bool get_isadmin() override { return isadmin; }
};