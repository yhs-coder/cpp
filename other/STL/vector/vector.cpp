#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

// 默认成员函数
void test_vector1()
{
	vector<int> v1;
	v1.push_back(1);
	v1.push_back(2);
	v1.push_back(3);
	v1.push_back(4);
	
	vector<int> v2(v1);
	for (size_t i = 0; i < v2.size(); i++) {
		cout << v1[i] << " ";
	}
	cout << endl;
	for (size_t i = 0; i < v2.size(); i++) {
		cout << v2[i] << " ";
	}
	cout << endl;

	vector<int> v3;
	v3.push_back(10);
	v3.push_back(20);
	v3.push_back(30);
	v3.push_back(40); 
	v1 = v3;
	for (size_t i = 0; i < v2.size(); i++) {
		cout << v1[i] << " ";
	} 
	cout << endl;
}

// 三种遍历方式
void test_vector2()
{
	vector<int> v1;
	v1.push_back(1);
	v1.push_back(2);
	v1.push_back(3);
	v1.push_back(4);

	// 遍历修改数据
	// 方式1： operator[] + size()
	for (size_t i = 0; i < v1.size(); i++) {
		cout << v1[i] << " ";
	}
	cout << endl;

	// 方式2：迭代器 （迭代器属于vector类内类型）
	vector<int>::iterator it = v1.begin();
	while (it != v1.end()) {
		cout << *it << " ";
		it++;
	}
	cout << endl;

	// 方式3：范围for -> 本质是编译器替换成迭代器方式支持遍历的
	for (auto e : v1) {
		cout << e << " ";
	}
	cout << endl;
}

void print_vector(const vector<int>& vt)
{
	// 只读
	vector<int>::const_iterator it = vt.begin();
	while (it != vt.end()) {
		// *it = 1;
		cout << *it << " ";
		it++;
	}
	cout << endl;

}


// 三种类型的迭代器
void test_vector3()
{
	vector<int> v;
	v.push_back(1);
	v.push_back(2);
	v.push_back(3);
	v.push_back(4);

	// 正向迭代器 可读可写
	vector<int>::iterator it = v.begin();
	while (it != v.end()) {
		cout << *it << " ";
		it++;
	}
	cout << endl;

	// 常量迭代器
	print_vector(v);

	// 反向迭代器
	vector<int>::reverse_iterator rit = v.rbegin();
	while (rit != v.rend()) {
		cout << *rit << " ";
		rit++;
	}
	cout << endl;
}

// vector容量操作
void test_vector4()
{
	vector<int> v;
	v.push_back(1);
	v.push_back(2);
	v.push_back(3);
	v.push_back(4);
	v.push_back(5);
	v.push_back(6);
	v.push_back(7);
	cout << v.size() << endl;
	cout << v.capacity() << endl;

	// vector空间增长问题
	size_t sz;
	vector<int> foo;
	sz = foo.capacity();
	// foo.reserve(1000);
	foo.resize(1000);
	cout << "making foo grow:" << endl;
	for (int i = 0; i < 1000; i++) {
		foo.push_back(i);
		if (sz != foo.capacity()) {
			sz = foo.capacity();
			cout << "capacity changed:" << sz << endl;
		}
	}
}

// 元素访问
void test_vector5()
{

	vector<int> v;
	v.push_back(1);
	v.push_back(2);
	v.push_back(3);
	v.push_back(4);
	
	cout << v[1] << endl;
	cout << v.at(1) << endl;
	
	// []与at处理访问越界的方式
	//v[4] = 1;	// 直接断言中断
	//v.at(4);	// 抛出异常
}

// 修改操作 insert / erase
void test_vector6()
{
	vector<int> v;
	v.push_back(1);
	v.push_back(2);
	v.push_back(3);
	v.push_back(4);
	v.push_back(3);

	// 插入
	v.insert(v.begin(), 0);
	v.insert(v.begin(), -1);
	for (auto e : v) {
		cout << e << " ";
	}
	cout << endl;

	// 删除
	v.erase(v.begin() + 2);	// 删除第三个元素
	for (auto e : v) {
		cout << e << " ";
	}
	cout << endl;
}

// 查找和排序，find和sort属于标准算法库
void test_vector7()
{
	vector<int> v;
	v.push_back(5);
	v.push_back(2);
	v.push_back(20);
	v.push_back(1);
	v.push_back(5);
	for (auto e : v) {
		cout << e << " ";
	}
	cout << endl;
	// find算法范围是左闭右开[begin,end)
	vector<int>::iterator pos = find(v.begin(),v.end(), 5);
	// 删除第一个的5
	if (pos != v.end()) {
		v.erase(pos);
	}
	for (auto e : v) {
		cout << e << " ";
	}
	cout << endl;

	// 使用sort算法进行排序
	sort(v.begin(), v.end());
	for (auto e : v) {
		cout << e << " ";
	}
	cout << endl;
}

int main()
{
	//test_vector1();
	//test_vector2();
	//test_vector3();
	//test_vector4();
	//test_vector5();
	//test_vector6();
	test_vector7();
	return 0;
}