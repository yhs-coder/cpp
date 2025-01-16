#pragma once
#include<iostream>
using std::cout;
using std::endl;

class Date
{

public:
	//构造函数
	Date(int year = 0, int month = 1,int day = 1);
	//Date(int year, int month, int day);
	//打印函数
	void Print()const;

	//日期+=天数
	Date& operator+=(int day);

	//日期+天数
	Date operator + (int day)const;

	//日期-=天数
	Date& operator-=(int day);

	//日期-天数
	Date operator - (int day)const;

	//前置++
	Date& operator++();
	//后置++
	Date operator++(int);
	//前置--
	Date& operator--();
	//后置--
	Date operator--(int);

	//日期的大小关系比较
	bool operator>(const Date& d)const;
	bool operator>=(const Date& d)const;
	bool operator<(const Date& d)const;
	bool operator<=(const Date& d)const;
	bool operator==(const Date& d)const;
	bool operator!=(const Date& d)const;

	//日期减日期
	int operator -(const Date& d)const;


private:
	int _year;
	int _month;
	int _day;
};

//获取某年某月的天数
inline int GetMonthDay(int year, int month);
