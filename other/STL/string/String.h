#pragma once
#include <iostream>
#include <cassert>
#include <utility> 
namespace yang
{
	class string
	{
	public:
		typedef char* iterator;
	public:
		string(const char* str = "")
		{
			_size = strlen(str);
			_capacity = _size;
			_str = new char[_capacity + 1];	// + 1是为了存储'\0'
			strcpy(_str, str);
		}

		// 深拷贝 - 传统方式
		/*
		string(const string& s)
		{
			_size = s._size;
			_capacity = s._capacity;
			_str = new char[strlen(s._str) + 1];
			strcpy(_str, s._str);
		}
		*/

		// 深拷贝 - 现代方式
		string(const string& s)
			: _str(nullptr)
			, _size(0)
			, _capacity(0)
		{
			string tmp(s._str);
			this->swap(tmp);
		}

		~string() {
			if (_str) {
				delete[] _str;
				_str = nullptr;
				_size = _capacity = 0;
				std::cout << "~string" << std::endl;
			}
		}
		/*
		// 传统写法
		string& operator = (const string& s) {
			if (this != &s) {
				delete[] _str;
				_str = new char[strlen(s._str) + 1];
				strcpy(_str, s._str);
				_size = s._size;
				_capacity = s._capacity;
			}
			return *this;
		}
		*/
		// 现代写法
		string& operator = (string s) {
			this->swap(s);
			return *this;
		}


		void swap(string& s) {
			// 调用库里的swap函数
			std::swap(_str, s._str);
			std::swap(_size, s._size);
			std::swap(_capacity, s._capacity);
		}

		/***************************************************************************/
		// 迭代器
		iterator begin() {
			return _str;
		}
		iterator end() {
			return _str + _size;
		}

		/***************************************************************************/
		// capacity 容量操作

		size_t size() const {
			return strlen(_str);
		}

		size_t capacity() const {
			return _capacity;
		}

		bool empty() const {
			return _size == 0;
		}

		// 将有效字符的个数改成n个
		void resize(size_t n, char ch = '\0') {
			// 三种情况：
			//	1. n < _size ,有效字符减少
			//	2. n > _size
			//	   2.1  n > _capacity,需要增容
			//	   2.2. n < _capacity
			if (n < _size) {
				_str[n] = '\0';
				_size = n;
			}
			else {
				if (n > _capacity) {
					reserve(n);
				}
				for (size_t i = _size; i < n; i++) {
					_str[i] = ch;
				}
				_size = n;
				_str[_size] = '\0';	// 记得在_str结尾添加'\0'
			}
		}

		void reserve(size_t n) {
			if (n > _capacity) {
				char* newstr = new char[n + 1];	// + 1是为了存储结束字符'\0'
				strcpy(newstr, _str);
				delete[] _str;
				_str = newstr;
				_capacity = n;
			}
		}

		/***************************************************************************/
		// 访问操作

		char& operator[](size_t index) {
			assert(index < _size);
			return _str[index];
		}

		const char& operator[](size_t index) const {
			assert(index < _size);
			return _str[index];
		}

		const char* c_str() const {
			return _str;
		}

		/***************************************************************************/
		// 修改操作

		void push_back(char ch) {
			/*
			// 空间满了进行增容
			if (_size == _capacity) {
				size_t new_capacity = _capacity == 0 ? 2 : _capacity * 2;
				reserve(new_capacity);
			}
			_str[_size] = ch;
			_size++;
			_str[_size] = '\0';	// 注意，这里需要加个结束字符'\0'
			*/
			// 使用insert进行尾插
			insert(_size, ch);
		}

		// 追加字符串
		void append(const char* str) {
			/*
			int len = strlen(str);
			if (_size + len > _capacity) {
				// 直接增容两倍可能不够
				reserve(_size + len);
			}
			strcpy(_str + _size, str);
			_size += len;
			*/
			insert(_size, str);
		}

		// 插入单个字符
		string& operator +=(char ch) {
			push_back(ch);
			return *this;
		}

		string& operator+= (const char* str) {
			append(str);
			return *this;
		}

		string& insert(size_t pos, char ch) {
			// 下标不可越界
			assert(pos <= _size);
			int end = _size;
			if (_size == _capacity) {
				size_t new_capacity = _capacity == 0 ? 2 : _capacity * 2;
				reserve(new_capacity);
			}
			// 移动从'\0'开始
			while (end >= (int)pos) {
				_str[end + 1] = _str[end];
				end--;
			}
			_str[pos] = ch;
			_size++;
			return *this;
		}

		// 在pos位置插入字符串
		string& insert(size_t pos, const char* str) {
			assert(pos <= _size);
			size_t len = strlen(str);
			// 插入字符串空间不够就先进行扩容
			if (_size + len > _capacity) {
				reserve(_size + len);
			}

			int end = _size;
			while (end >= (int)pos) {
				_str[end + len] = _str[end];
				end--;
			}
			//!strcpy(_str + pos, str); // 很严重strcpy会拷贝结束字符'\0',会把pos+len+1位置的字符覆盖掉
			// 使用遍历的方式插入字符
			/*for(size_t i = 0; i < len; i++) {
				_str[pos++] = str[i++];
			}*/
			strncpy(_str + pos, str, len);
			_size += len;
			return *this;
		}

		void clear() {
			_size = 0;
			_str[_size] = '\0';
		}
		void erase(size_t pos, size_t len = npos) {
			assert(pos < _size);
			if (len >= _size - pos) {
				_str[pos] = '\0';
				_size = pos;
			}
			else {
				size_t i = pos + len;
				while (i <= _size) {
					_str[i - len] = _str[i];
					i++;
				}
				_size -= len;
			}
		}

		size_t find(char ch, size_t pos = 0) {
			for (size_t i = pos; i < _size; i++) {
				if (_str[i] == ch) {
					return i;
				}
			}
			return npos;
		}
		size_t find(const char* str, size_t pos = 0) {
			const char* tmp = strstr(_str + pos, str);
			if (tmp == nullptr) {
				return npos;
			}
			return tmp - _str;
		}

		// 反方向查找第一个匹配的字符
		size_t rfind(char ch, size_t pos = npos) {
			string tmp(*this);
			std::reverse(tmp.begin(),tmp.end());	// 逆置字符串
			// pos位和镜像位置pos'相加等于_size - 1,即 pos + pos' = _size - 1;
			if (pos > _size) {
				pos = _size - 1;
			}
			pos = _size - 1 - pos;
			size_t ret = tmp.find(ch, pos);
			if (ret != npos) {
				return _size - 1 - ret;
			}
			else {
				return npos;
			}
		}

		size_t rfind(const char* str, size_t pos = npos) {
			string tmp(*this);
			std::reverse(tmp.begin(), tmp.end());
			int len = strlen(str);
			char* arr = new char[len + 1];
			strcpy(arr, str);
			// 逆置带查找的字符串
			size_t left = 0, right = len - 1;
			while (left < right) {
				std::swap(arr[left],arr[right]);
				left++;
				right--;
			}
			if (pos >= _size) {
				pos = _size - 1;
			}
			pos = _size - 1 - pos;
			size_t ret = tmp.find(arr, pos);
			delete[] arr;
			if (ret != npos) {
				return _size - len - pos;
			}
			else {
				return npos;
			}
		}

		bool operator > (const string& s) {
			return strcmp(_str, s._str) > 0;
		}
		bool operator == (const string& s) {
			return strcmp(_str, s._str) == 0;
		}
		bool operator >= (const string& s) {
			return (*this > s) || (*this == s);
		}
		bool operator < (const string& s) {
			return  !(*this >= s);
		}
		bool operator <= (const string& s) {
			return !(*this > s);
		}
	
		bool operator != (const string& s) {
			return !(*this == s);
		}

	private:
		friend std::ostream& operator << (std::ostream& out, const yang::string& s);
		friend std::istream& operator >> (std::istream& in,  yang::string& s);
		// 读取一行含有空格的字符串
		friend std::istream& getline(std::istream& in, string& s);

	private:
		char*	_str;			// 存储字符的数组
		size_t	_size;			// 有效字符的个数
		size_t	_capacity;		// 存储有效字符的容量,'\0'不是有效字符
		static const size_t npos;
	};

	const size_t string::npos = -1;

	std::ostream& operator << (std::ostream& out, const yang::string& s) {
		for (size_t i = 0; i < s.size(); i++) {
			out << s[i];
		}
		return out;
	}
	std::istream& operator >> (std::istream& in, yang::string& s) {
		s.clear();			// 清空字符串
		char ch = in.get();	// 读取一个字符
		while (ch != ' ' && ch != '\n') {
			s += ch;
			ch = in.get();
		}
		return in;
	}

	std::istream& getline(std::istream& in, string& s) {
		s.clear();
		char ch = in.get();
		while (ch != '\n') {
			s += ch;
			ch = in.get();
		}
		return in;
	}

};