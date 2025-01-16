#include <iostream>
using namespace std;

// 可变模板参数,传0到任意个参数，参数类型与个数都不确定
template <typename...T>
void print(T... t)
{
	cout << "sizeof...(T) = " << sizeof...(T) << endl;
	cout << "sizeof...(t) = " << sizeof...(t) << endl;
}

void test1()
{
	print();
	print(1, "hello");
	print(1, true, 3.3, "hello");

}


// 当参数包为空，递归停止
void show()
{
	cout << "参数为空" << endl;
}

template <typename T, typename... Args>
void show(T t, Args... args)
{
	cout << t << " ";
	// 递归， ...位于参数右边的时候，称为解包
	// ...位于左边的时候，称为打包
	show(args...);
}

void test2()
{
	// 多个实参，调用可变参数的函数模板
	show(1, "hello", 3.3);
	show(1, true, 3.14);
	// 参数为空，调用普通的函数
	show();
}


int sum()
{
	return 0;
}

// 使用可变模板参数实现递增
template <typename T, typename... Args>
T sum(T t, Args... args)
{
	return t + sum(args...);
}

void test3()
{
	cout << sum(1, 2, 3, 4, 5, 6, 7, 8, 9, 10); // 55
}

union Un
{
	int i;
	char c;
};
union Un un;


int main()
{
	//test1();
	/*test2();
	test3();*/

	un.i = 0x11223344;
	un.c = 0x55;
	printf("%x\n", un.i);
	printf("%x\n", un.c);
	return 0;
}