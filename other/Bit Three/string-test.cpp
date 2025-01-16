#define _CRT_SECURE_NO_WARNINGS
#include"mystring.h"
#include<iostream>
#include<cstring>
using namespace std;

int main()
{
	String s2("hello");
	String s1("hello");
	s1 = "world";
	String* p = new String("hello");
	delete p;

	String s3(s2);
	cout << s3 << endl;

	s3 = s1;
	cout << s3 << endl;
	cout << s1 << endl;
	cout << s2 << endl;


	return 0;
}
