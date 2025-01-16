#pragma once
#include <assert.h>
// 模拟实现vector

namespace yang {

	template <class T>
	class vector {
	public:
		typedef T* iterator;
		typedef const T* const_iterator;

		vector()
			: _start(nullptr)
			, _finish(nullptr)
			, _endofstorage(nullptr) {}

		vector(int n, const T& value = T()) 
			: _start(nullptr)
			, _finish(nullptr)
			, _endofstorage(nullptr) 
		{
			reserve(n);
			while (n--) {
				push_back(value);
			}
			cout << "call vector(int n, const T& value = T()) " << endl;
		}
		// 若使用iterator做迭代器，会导致初始化的迭代器区间[first,last)只能是vector的迭代器
		// 重新声明迭代器，迭代器区间[first,last)可以是任意容器的迭代器
		template <class InputIterator>
		vector(InputIterator first, InputIterator last) {
			reserve(last - first);
			while (first != last) {
				push_back(*first);
				first++;
			}
			cout << "call vector(InputIterator first, InputIterator last)" << endl;
		}
		
		~vector() {
			delete[] _start;
			_start = _finish = _endofstorage = nullptr;
		}

		// 拷贝构造方法一
		/*vector(const vector<T>& v) {
			// 深拷贝
			_start = new T[v.capacity()];
			_finish = _start;
			_endofstorage = _start + v.capacity();

			// 拷贝数据
			for (size_t i = 0; i < v.size(); i++) {
				*_finish = v[i];
				_finish++;
			}
		}*/

		// 拷贝构造方法二
		vector(const vector<T>& v)
			: _start(nullptr)
			, _finish(nullptr)
			, _endofstorage(nullptr)
		{
			// 先开辟预留空间
			reserve(v.capacity());
			for (const auto& e : v) {
				push_back(e);
			}
		}

		/*vector<T>& operator = (const vector<T>& v) {
			// 防止自赋值
			if (this != &v) {
				delete[] _start;
				_start = new T[v.capacity()];
				memcpy(_start, v._start, sizeof(T) * v.size());
				/ *_finish = _start + v.size();
				_endofstorage = _start + v.capacity();* /
			}
			return *this;
		}*/

		// v1 = v3 现代写法
		vector<T>& operator = (vector<T>& v) {
			swap(v);
			return *this;
		}


		void swap(vector<T>& v) {
			std::swap(_start, v._start);
			std::swap(_finish, v._finish);
			std::swap(_endofstorage, v._endofstorage);
		}
		// 迭代器相关
		iterator begin() {
			return _start;
		}

		iterator end() {
			return _finish;
		}
		const_iterator begin() const {
			return _start;
		}

		const_iterator end() const {
			return _finish;
		}

		// 访问：可读可写
		T& operator[] (size_t pos) {
			assert(pos < size());
			return _start[pos];
		}

		// 常量版本的operator[]
		const T& operator[] (size_t pos) const {
			assert(pos < size());
			return _start[pos];
		}

		void reserve(size_t n) {
			size_t sz = size();
			if (n > capacity()) {
				T* tmp = new T[n];
				if (_start) {
					// memcpy(tmp, _start, sizeof(T) * sz); // 按字节拷贝，浅拷贝
					for (size_t i = 0; i < sz; i++) {
						tmp[i] = _start[i];		// 调用的是operator = ，深拷贝
					}
					delete[] _start;
				}
				_start = tmp;
				_finish = tmp + sz;
				_endofstorage = tmp + n;
			}
		}

		// T& value = T(), 给value赋缺省值，C++内置类型提供 和 自定义类型类似的构造，本质是为了支持模板
		void resize(size_t n, const T& val = T()) {
			// 分情况
			/*	(1) n > size()
					  1. n < capacity()
					  2. n > capacity() 扩容
				(2) n < size()
			*/
			// 1. 如果n小于当前的size,则数据个数缩小到n
			if (n < size()) {
				_finish = _start + n;
			}
			else{
				// 空间不够增容
				if (n > capacity()) {
					reserve(n);
				}
				// 填充数据
				// 不可用memset！！！,因为menset是按字节为单位进行操作
				while (_finish < _start + n) {
					*_finish = val;
					_finish++;
				}
			}

		}

		void push_back(const T& x) {
			/*if (_finish == _endofstorage) {
				size_t newcapacity = capacity() == 0 ? 2 : capacity() * 2;
				reserve(newcapacity);
			}
			*_finish = x;
			_finish++;*/
			insert(_finish, x);
		}

		void pop_back() {
			/*assert(_start < _finish);
			_finish--;*/
			erase(_finish - 1);
		}

		iterator insert(iterator pos, const T& x) {
			// 检查pos位置合法性
			assert(pos >= _start);	// pos == _start头插	
			assert(pos <= _finish); // pos == _finish 相当于尾插

			// 插入时需要考虑扩容
			if (_finish == _endofstorage) {
				size_t n = pos - _start;
				size_t newcapacity = capacity() == 0 ? 2 : capacity() * 2;
				reserve(newcapacity);
				// 如果增容，原来的pos迭代器就失效了，这里pos需要重新计算位置，指向新空间
				pos = _start + n;
			}
			iterator end = _finish - 1;
			while (end >= pos) {
				*(end + 1) = *(end);
				end--;
			}
			*pos = x;
			_finish++;
			return pos;
		}

		iterator erase(iterator pos) {
			// 检查位置的有效性
			assert(pos < _finish);
			iterator it = pos;
			while (it < _finish) {
				*it = *(it + 1);
				it++;
			}
			_finish--;
			// 返回值是原始pos位置的下一位置的迭代器
			// 删除移动完元素后，pos指向原来位置的下一位置
			return pos;
		}
		size_t size() const {
			return _finish - _start;
		}

		size_t capacity() const {
			return _endofstorage - _start;
		}
		bool empty() const {
			return _start == _finish;
		}

	private:
		iterator _start;		// 指向数据块的开始
		iterator _finish;		// 指向最后有效一个元素的下一位置
		iterator _endofstorage;	// 指向存储容量的下一位置
	};


	void print_vector(const vector<int>& v) {
		vector<int>::const_iterator it = v.begin();
		while (it != v.end()) {
			//*it += 1;
			cout << *it << " ";
			it++;
		}
		cout << endl;
	}
	void test_vector1() {
		vector<int> v;
		v.push_back(1);
		v.push_back(2);
		v.push_back(3);
		v.push_back(4);
		cout << v.size() << endl;
		cout << v.capacity() << endl;

		v.push_back(5);
		// 扩容
		cout << v.size() << endl;
		cout << v.capacity() << endl;

		// 使用常量迭代器
		print_vector(v);

		vector<int>::iterator it = v.begin();
		while (it != v.end()) {
			*it += 1;
			cout << *it << " ";
			it++;
		}
		cout << endl;

		// 测试范围for遍历
		for (auto& e : v) {
			e -= 1;
			cout << e << " ";
		}
		cout << endl;

		// 测试operator[]
		for (size_t i = 0; i < v.size(); i++) {
			cout << v[i] << " ";
		}
		cout << endl;
	}

	// 测试插入、删除
	void test_vector2() {
		vector<int> v;
		v.push_back(1);
		v.push_back(2);
		v.push_back(3);
		v.push_back(4);
		v.push_back(5);
		v.push_back(6);
		v.push_back(7);

		v.insert(v.begin(), 0);
		print_vector(v);

		v.pop_back();
		vector<int>::iterator it = v.begin();
		while (it != v.end()) {
			if ((*it % 2) == 0)
				it = v.erase(it);
			else
				it++;
		}
		print_vector(v);
	}

	// 测试resize
	void test_vector3() {
		vector<int> v;
		v.push_back(1);
		v.push_back(2);
		v.push_back(3);
		v.push_back(4);
		v.push_back(5);
		v.push_back(6);
		v.push_back(7);

		print_vector(v);
		cout << v.size() << endl;
		cout << v.capacity() << endl << endl;

		v.resize(4);
		print_vector(v);
		cout << v.size() << endl;
		cout << v.capacity() << endl << endl;

		v.resize(8,8);
		print_vector(v);
		cout << v.size() << endl;
		cout << v.capacity() << endl << endl;

		v.resize(12,12);
		print_vector(v);
		cout << v.size() << endl;
		cout << v.capacity() << endl << endl;

	}

	void test_vector4() {
		vector<int> v1;
		v1.push_back(1);
		v1.push_back(2);
		v1.push_back(3);
		v1.push_back(4);
		
		vector<int> v2(v1);
		for (size_t i = 0; i < v1.size(); i++) {
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
		v1.push_back(50);
		for (auto e : v1) {
			cout << e << " ";
		}
		
		cout << endl;
		for (auto e : v3) {
			cout << e << " ";
		}
		cout << endl;
	}

	void test_vector5() {
		// 测试memcpy深拷贝的问题及解决
		vector<std::string> v;
		v.push_back("1111");
		v.push_back("2222");
		v.push_back("3333");
		v.push_back("4444");
		for (auto e : v) {
			cout << e << " ";
		}
		cout << endl;
	}

	void test_vector6() {
		vector<int> v(5,7);
		vector<int> first;
		if (first.empty()) {
			cout << "It's empty" << endl;
		}

		vector<int> second(4, 100);
		print_vector(second);
		vector<int> third(second.begin(), second.end());	  // 使用迭代器进行初始化
		print_vector(third);
	}
}