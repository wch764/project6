#pragma once
#include<iostream>
//������
class Date
{
protected:
	int year, month, day;
public:
	Date(int y=0, int m=0, int d=0);
	bool isValid()const;//��֤�Ϸ���
	bool operator>(const Date&)const;//�Ƚ�����
	bool operator<(const Date&)const;
	bool operator==(const Date&)const;
	int operator-(const Date&)const;//������������֮��
	void displayDate() const;
	std::string toString()const;//תΪ�ַ���
	virtual ~Date() {};
	void currentDate();//������ֵΪ��ǰ����
	void addDays(int days);//��������
	static Date currentdate() {
		Date d;
		d.currentDate();  // ���÷Ǿ�̬����
		return d;
	}//��̬����
};