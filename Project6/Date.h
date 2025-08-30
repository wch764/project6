#pragma once
#include<iostream>
//日期类
class Date
{
protected:
	int year, month, day;
public:
	Date(int y=0, int m=0, int d=0);
	bool isValid()const;//验证合法性
	bool operator>(const Date&)const;//比较日期
	bool operator<(const Date&)const;
	bool operator==(const Date&)const;
	int operator-(const Date&)const;//返回两个日期之差
	void displayDate() const;
	std::string toString()const;//转为字符串
	virtual ~Date() {};
	void currentDate();//将对象赋值为当前日期
	void addDays(int days);//增加天数
	static Date currentdate() {
		Date d;
		d.currentDate();  // 调用非静态方法
		return d;
	}//静态方法
};