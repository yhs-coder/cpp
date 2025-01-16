#include <iostream>
using std::cout;
using std::endl;
#include "simulate_vector.h"


namespace yang {

	// 模拟实现vector
	template <class T>
	class vector {
	public:
		// 类型别名
		typedef T* iterator;
		typedef const T* const_iterator;

		// 默认成员函数
		// 构造函数
		vector();
		vector(int n, const T& value = T())

	};

}


int main()
{
	//yang::test_vector1();
	//yang::test_vector2();
	//yang::test_vector3();
	//yang::test_vector4();
	//yang::test_vector5();
	yang::test_vector6();
	return 0;
}





