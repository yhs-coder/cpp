#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
using namespace std;


//class Date
//{
//public:
//	/*
//	//全缺省参数的的构造函数
//	Date(int year = 0, int month = 0, int day = 1)
//	{
//		_year = year;
//		_month = month;
//		_day = day;
//	}
//	*/
//	//构造函数初始化列表
//	Date(int year,int month,int day):_year(year),_month(month),_day(day){}
//
//private:
//	int _year;
//	int _month;
//	int _day;
//};

/*初始化列表：初始化自定义类型成员（没有默认构造函数）*/
class A
{
public:
	A(int val)
	{
		_val = val;
	}
private:
	int _val;
};

class B
{
public:
	B() :_a(2022){}
private:
	A _a;
};


//初始化列表可以提高代码效率
class Time
{
public:
	Time(int hour = 0)
	{
		_hour = hour;
	}

private:
	int _hour;
};
class Test
{
public:
	/*
	//使用初始化列表
	Test(int hour):_t(hour){}
	//实例化Test对象时，在初始化列表那调用了一次Time的构造函数
	*/

	//不使用初始化列表
	Test(int hour)
	{
		//不使用初始化列表也会调用一次Time的构造函数（明确对象需要定义的成员变量）
		Time t(hour); //再调用一次Time的构造函数
		_t = t;		//调用一次Time类的赋值运算符重载函数
	}

private:
	Time _t;
};


/*
	初始化列表初始化顺序：成员变量在类中声明的次序，而不是在初始化列表的先后顺序
*/
int i = 0;
class Test1
{
public:
	Test1():_b(i++),_a(i++){}
	//_a先声明，_b后声明，初始化列表先初始化_a后_b
	void Print()
	{
		cout << "_a:" << _a << endl; //0
		cout << "_b:" << _b << endl; //1
	}
private:
	int _a;
	int _b;


};


/*
	Static成员
	1.静态成员变量一定要在类外进行初始化
	2.静态成员函数没有隐藏的this指针，不能访问任何非静态成员，即只能访问静态成员变量。
	3.静态成员变量必须在类外定义，定义时不添加static关键字
	4.访问静态成员变量的方法
*/

class Test2
{
public:
	static void Fun()
	{
		//cout << _a << endl;//没有this指针，不能访问！
		cout << _n << endl;
	}
	static int GetN()
	{
		return  _n;
	}
private:
	static int _n;
	int _a;
};

class Test3
{
public:
	static int _n2;
};

int Test3::_n2 = 666;
//静态成员变量的定义初始化
int Test2::_n = 10;


class Date
{
	//友元函数的声明
	friend  ostream& operator << (ostream & out, const Date & d);
	friend istream& operator>>(istream& in, Date& d);
	public:
	//全缺省参数的的构造函数
	Date(int year = 0, int month = 0, int day = 1)
	{
		_year = year;
		_month = month;
		_day = day;
	}
	

private:
	int _year;
	int _month;
	int _day;
};

ostream& operator << (ostream& out, const Date& d)
{
	out << d._year << "-" << d._month << "-" << d._day << endl;
	return out;
}

istream& operator>>(istream& in, Date& d)
{
	in >> d._year >>d._month >>  d._day ;
	return in;
}


/*
	内部类:
	外部类的大小与内部类的大小无关
*/

class C
{
public:
	class B
	{
	private:
		int _b;
	};
private:
	int _a;
};


int main()
{
	//Test test(21);
	//Test1 t;
	//t.Print();
	//cout << sizeof(Test2) << endl;

	/* .当静态成员变量为公有时,有三种访问方式：
	Test3 t;
	cout << t._n2 <<endl; //通过类对象突破类域进行访问
	cout << Test3()._n2 << endl;//通过匿名对象突破类域进行访问
	cout << Test3::_n2 << endl; //通过类名突破类域进行访问
	*/

	
	/*当静态成员变量为私有时，有三种访问方式：
	Test2 t;
	cout << t.GetN() << endl; //通过类对象调用成员函数进行访问
	cout << Test2().GetN() << endl;//通过匿名对象调用静态成员函数进行访问
	cout << Test2::GetN() << endl; //通过类名调用静态成员函数进行访问
	*/
	cout << sizeof(C) << endl;
	return 0;
}