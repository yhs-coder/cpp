//#include <iostream>
//#include <cassert>
//#include <utility> 
#pragma warning(disable:4996)
using std::cout;
using std::endl;
using std::cin;
#include "String.h"

using namespace yang;

// 测试基本构造
void test_string1() {
	yang::string s1;
	yang::string s2("hello");
	yang::string s3("world");
	s3 = s2;
	cout << s1.c_str() << endl;
	cout << s2.c_str() << endl;
	cout << s3.c_str() << endl;
	for (size_t i = 0; i < s1.size(); i++) {
		cout << s1[i];
	}
	cout << endl;
	for (size_t i = 0; i < s2.size(); i++) {
		s2[i] += 1;
		cout << s2[i];
	}
	cout << endl;
	for (size_t i = 0; i < s3.size(); i++) {
		cout << s3[i];
	}
	cout << endl;

}

// 测试迭代器
void test_string2()
{
	string s1("hello");
	string s2(s1);
	cout << s1.c_str() << endl;
	cout << s2.c_str() << endl;
	string s3("world");
	s1 = s3;
	cout << s3.c_str() << endl;
	cout << s1.c_str() << endl;

	// 三种遍历方式
	for (int i = 0; i < s1.size(); i++) {
		s1[i] += 1;
		cout << s1[i];
	}
	cout << endl;
	yang::string::iterator it = s1.begin();
	while (it != s1.end()) {
		*it -= 1;
		cout << *it;
	}
	cout << endl;

	for (auto ch : s1) {
		cout << ch;
	}
	cout << endl;
}

// 测试容量操作
void test_string3()
{
	yang::string s1;
	yang::string s2("hello");
	cout << s1.size() << endl;
	cout << s1.capacity() << endl;
	cout << s2.size() << endl;
	cout << s2.capacity() << endl;

	if (s1.empty()) {
		cout << "s1 is empty." << endl;
	}
	s2.reserve(10);
	cout << s2.size() << endl;
	cout << s2.capacity() << endl;

	s2.resize(7, 'a');
	cout << s2.c_str() << endl;
	cout << s2.size() << endl;
	cout << s2.capacity() << endl;

	s2.resize(17, 'Q');
	cout << s2.c_str() << endl;
	cout << s2.size() << endl;
	cout << s2.capacity() << endl;

	s2.resize(2);
	cout << s2.c_str() << endl;
	cout << s2.size() << endl;
	cout << s2.capacity() << endl;

}

// 测试修改
void test_string4()
{
	string s1("hello");
	s1.push_back('x');
	s1.append("wo--------");
	s1 += 'r';
	s1 += "ld";
	cout << s1.c_str() << endl;

	string s2("hello");
	s2.insert(1, 'x');
	cout << s2.c_str() << endl;
	s2.insert(5, 'y');
	cout << s2.c_str() << endl;
	s2.insert(1, "zzzz");
	cout << s2.c_str() << endl;
	s2.insert(10, "tttt");
	cout << s2.c_str() << endl;
	cout << endl;
	cout << s2.c_str() << endl;

}
void test_string5()
{
	string s1("helloworld");
	s1.erase(5, 2);
	cout << s1.c_str() << endl;

	s1.erase(5, 4);
	cout << s1.c_str() << endl;

	s1.erase(1, 8);
	cout << s1.c_str() << endl;

}

void test_string6()
{
	//string s1("helloworld");
	//s1.erase(5, 2);
	//cout << s1 << endl;
	//s1.erase(5, 4);
	//cout << s1 << endl;
	string s2("abcdeffgcda");
	cout << s2.find('g') << endl;
	cout << s2.find("cda") << endl;
	cout << s2.size() << endl;
	cout << s2.rfind('d', 7) << endl;
	cout << s2.rfind("fgc") << endl;


}

void test_string7()
{
	string s1("hello");
	cout << s1 << endl;
	s1 += "y";
	s1 += "xxxx";
	s1.insert(0, "ooo");
	cout << s1 << endl;
}

void test_string8()
{
	string s("hello");
	cin >> s;
	cout << s << endl;
	getline(cin, s);
	{
		size_t pos = s.rfind(' ');
		cout << s.size() - pos - 1 << endl;
	}
	cout << s << endl;
}
void test_string9()
{
	string s1("hello");
	string s2("hello");
	string s3("world");
	if (s1 == s2) {
		cout << "s1 == s2" << endl;
	}

	if (s1 > s3) {
		cout << "s1 > s3" << endl;
	}

	if (s1 <= s3) {
		cout << "s1 <= s3" << endl;
	}
	if (s1 != s3) {
		cout << "s1 != s3" << endl;
	}
}

int main()
{
	test_string1();
	//test_string2();
	//test_string3();
	//test_string4();
	//test_string5();
	//test_string6();
	//test_string7();
	//test_string8();
	//test_string9();


	return 0;
}
