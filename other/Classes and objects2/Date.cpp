#define _CRT_SECURE_NO_WARNINGS

#include"Date.h"

//获取某年某月的天数
inline int GetMonthDay(int year, int month)
{
	//数组存储平均每个月的天数
	static int dayArray[13] = { 0,31,28,31,30,31,30,31,31,30,31,30,31 };
	int day = dayArray[month];
	//判断是否为闰年
	if (month == 2 && ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)))
	{
		//闰年2月天数
		day = 29;
	}
	return day;
}

Date::Date(int year, int month, int day)
{
	//检查日期的合法性
	if (year >= 0 && month >= 1 && month <= 12 && day >= 1 && day <= GetMonthDay(year, month))
	{
		_year = year;
		_month = month;
		_day = day;
	}
	else
	{
		cout << "非法日期" << endl;
		cout << year << "年" << month << "月" << day << "日" << endl;
	}
}

void Date::Print()const
{
	cout << _year << "年" << _month << "月" << _day << "日" << endl;
}

Date& Date::operator+=(int day)
{
	if (day < 0)
	{
		*this -= -day;
	}
	else
	{
		_day += day;
		//日期不合法，通过不断调整，直到最后日期合法为止
		while (_day > GetMonthDay(_year, _month))
		{
			_day -= GetMonthDay(_year, _month);
			_month++;
			if (_month > 12)
			{
				_month = 1;
				_year++;
			}
		}
	}
	return *this;
}
Date& Date::operator-=(int day)
{
	if (day < 0)
	{
		*this += -day;
	}
	else
	{
		_day -= day;
		while (_day <= 0)
		{
			_month--;
			if (_month <= 0)
			{
				_year--;
				_month = 12;
			}
			_day += GetMonthDay(_year, _month);
		}
	}
	return *this;
}
Date Date::operator + (int day)const
{
	Date tmp(*this);//拷贝构造tmp，用于返回
	tmp += day;
	return tmp;
}

Date Date::operator - (int day)const
{
	Date tmp(*this);//拷贝构造tmp，用于返回
	tmp -= day;
	return tmp;
}

Date& Date::operator++()
{
	//复用operator+=
	*this += 1;
	return *this;
}

Date Date::operator++(int)
{
	Date tmp(*this);
	//复用operator+=
	*this += 1;
	return tmp;
}

Date& Date::operator--()
{
	*this -= 1;
	return *this;
}

Date Date::operator--(int)
{
	Date tmp(*this);
	//复用operator+=
	*this -= 1;
	return tmp;
}

bool Date::operator>(const Date& d)const
{
	if (_year > d._year)
		return true;
	else if (_year == d._year)
	{
		if (_month > d._month)
			return true;
		else if (_month == d._month)
		{
			if (_day > d._day)
				return true;	
		}
	}
		return false;
}

bool Date::operator>=(const Date& d)const
{
	return *this > d || *this == d;
}


bool Date::operator==(const Date& d)const
{
	return _year == d._year && _month == d._month && _day == d._day;
}

bool Date::operator<(const Date& d)const
{
	return !(*this >= d);
}

bool Date::operator<=(const Date& d)const
{
	return !(*this > d);
}

bool Date::operator!=(const Date& d)const
{
	return !(*this == d);
}

int Date::operator -(const Date& d)const
{

	Date max = *this;//假设*this日期较大
	Date min = d; //d日期较小
	int flag = 1; //此时结果为正值
	//标志位，1表示正，-1表示负

	//假设错误
	if (max < min)
	{
		max = d;
		min = *this;
		flag = -1; //结果应该为负值
	}
	int count = 0; //记录相差的总天数（绝对值）
	while (max != min)
	{
		min ++; //日期++
		count++;//相差的总天数
	}

	//结果要乘flag,才可以结果正负
	return count*flag;

}

