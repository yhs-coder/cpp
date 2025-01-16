#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include<cassert>
#include"Date.h"
using namespace std;

/*
class Date
{
public:
	Date(int year = 0, int month = 1, int day = 1) //构造函数
	{
		_year = year;
		_month = month;
		_day = day;
	}

	//Date(const Date& d) //拷贝构造函数
	//{
	//	_year = d._year;
	//	_month = d._month;
	//	_day = d._day;
	//}
	void Print()const
	{
		cout << _year << "年" << _month << "月" << _day << "日" << endl;
	}
	bool operator ==(const Date& d) //运算符重载
	{
		return _year == d._year && _month == d._month && _day == d._day;
	}

	//参数列表第一个默认隐藏的参数：Date* this
	Date& operator=(const Date& d)
	{
		if (this != &d)
		{
			_year = d._year;
			_month = d._month;
			_day = d._day;
		}
		return *this;
	}

private:
	int _year;
	int _month;
	int _day;
};
*/

class Stack
{
public:
	Stack(int capacity = 4)
	{
		_ps = (int*)malloc(sizeof(int) * capacity);
		assert(_ps);
		_size = 0;
		_capacity = capacity;

	}
	void Print()
	{
		cout << _ps << endl; //打印栈的地址空间
	}
private:
	int* _ps;
	int _size;
	int _capacity;
};
void TestStack()
{
	Stack s1;
	s1.Print();
	Stack s2(s1);
	s2.Print();
}
void TestDate()
{
	Date d1(2022, 11, 1);
	Date d2(2022, 11, 11);

	//d1 += 30;
	//d1 = d1 + 20;
	//d1 -= 5;
	/*d1 = d1 - (-5);*/
	cout << "d1 - d2= " << d1 - d2 << endl;
	
	//if (d1!=d2)
	//{ //d2--;
	//	cout << "d1!=d2:";
	//	d1.Print();
	//}
	//else
	//{
	//	cout << "d2:";
	//	d2.Print();
	//}
}

int main()
{
	TestDate();
	//TestStack();
	return 0;
}