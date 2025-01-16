#pragma once

namespace yang
{
	template <class T>
	// 定义链表节点
	struct __list_node {
		__list_node<T>* prev;
		__list_node<T>* next;
		T data;

		__list_node(const T& x = T())
			: data(x)
			, prev(nullptr)
			, next(nullptr)
		{}
	};

	template <class T, class Ref, class Ptr>
	// 迭代器
	struct __list_iterator {
		typedef __list_node<T> Node;
		typedef __list_iterator<T, Ref, Ptr> Self;
		Node* node;

		__list_iterator(Node* node) : node(node) {}

		// *it
		Ref operator*()
		{
			return node->data;
		}

		// it->
		Ptr operator->()
		{
			return &node->data;
		}

		// ++it 前置++
		Self& operator++()
		{
			node = node->next;
			return *this;
		}
		// 后置++
		Self operator++(int)
		{
			/*__list_iterator<T> tmp(node);
			node = node->next;
			return tmp;*/
			Self tmp(*this);
			++(*this);
			return tmp;
		}

		// --it 前置--
		Self& operator--()
		{
			node = node->prev;
			return *this;
		}

		// --it 后置--
		Self operator--(int)
		{
			// __list_iterator<T> tmp(node);
			Self tmp(*this);
			// node = node->prev;
			--(*this);
			return tmp;
		}

		// it != end()
		bool operator != (const Self& it)
		{
			return node != it.node;
		}

		bool operator == (const Self& it)
		{
			return node == it.node;
		}

	};

	template <class T>
	class list {
		typedef __list_node<T> Node;
	public:
		typedef __list_iterator<T, T&, T*> iterator;
		typedef __list_iterator<T, const T&, const T*> const_iterator;

		// 带头双向循环链表
		list()
		{
			_head = new Node;
			_head->next = _head;
			_head->prev = _head;
		}

		list(const list<T>& lt)
		{
			_head = new Node;
			_head->prev = _head;
			_head->next = _head;
			/*list<T>::const_iterator it = lt.begin();
			while (it != lt.end())
			{
				push_back(*it);
				++it;
			}*/

			// 更简洁的方法
			for (auto e : lt)
			{
				push_back(e);
			}
		}

		// lt1 = lt2
		/*list<T>& operator = (const list<T>& lt)
		{
			// 防止自赋值
			if (this != &lt)
			{
				// 思路：先清空原链表的数据,再输入lt链表的数据
				clear();
				for (auto e : lt)
				{
					push_back(e);
				}

				// 下面这种效率低，还有额外再调用拷贝构造、析构，多了两个函数栈帧
				//list<T> tmp(lt);
				//::swap(_head, tmp._head);
			}
			return *this;
		}*/

		list<T>& operator = (list<T> lt)
		{
			swap(_head, lt._head);
			return *this;
		}
 		~list()
		{
			clear();
			delete _head;	// 释放头节点
			_head = nullptr;
		}

		void clear()
		{
			// 释放除头节点外所有的节点
			iterator it = begin();
			while (it != end())
			{
				erase(it++);
			}
		}

		iterator begin()
		{
			return iterator(_head->next);
		}

		// 最后一个数据的下一位置
		iterator end()
		{
			return iterator(_head);
		}

		// 常量对象得到常量迭代器
		const_iterator begin() const
		{
			cout << "const_iterator" << endl;
			return const_iterator(_head->next);
		}

		// 最后一个数据的下一位置
		const_iterator end() const
		{
			return const_iterator(_head);
		}


		// 插入
		void push_back(const T& x)
		{
			/*Node* newnode = new Node(x);
			Node* tail = _head->prev;	// 链表尾节点
			tail->next = newnode;
			newnode->prev = tail;
			newnode->next = _head;
			_head->prev = newnode;*/

			// 复用insert
			insert(end(), x);
		}

		void pop_back()
		{
			// 复用erase
			// erase(iterator(_head->prev));
			erase(--end());	// 哨兵节点的前一节点就是尾节点
		}
		void push_front(const T& x)
		{
			insert(begin(), x);
		}
		void pop_front()
		{
			erase(begin());
		}

		void insert(iterator pos, const T& x)
		{
			Node* cur = pos.node;
			Node* prev = cur->prev;
			Node* newnode = new Node(x);
			// prev newnode cur
			prev->next = newnode;
			newnode->prev = prev;
			newnode->next = cur;
			cur->prev = newnode;
		}

		iterator erase(iterator pos)
		{
			assert(pos != end());

			Node* cur = pos.node;
			Node* prev = cur->prev;
			Node* next = cur->next;
			delete cur;
			prev->next = next;
			next->prev = prev;
			return next;
		}

	private:
		Node* _head;	// 哨兵头节点
	};

	void test_list1()
	{
		list<int> lt;
		lt.push_back(1);
		lt.push_back(2);
		lt.push_back(4);
		lt.push_back(5);
		list<int>::iterator it = lt.begin();
		while (it != lt.end())
		{
			cout << *it << " ";
			//++it;
			it++;
		}
		cout << endl;
	}

	struct Date
	{
		int year = 0;
		int month = 1;
		int day = 1;
	};

	void test_list2()
	{
		list<Date> lt;
		lt.push_back(Date());
		lt.push_back(Date());

		list<Date>::iterator it = lt.begin();
		while (it != lt.end())
		{
			//cout << *it << " "; 
			// 原本是it->()->year，但是为了可读性，编译器进行了特殊的处理
			cout << it->year << "-" << it->month << "-" << it->day << endl;
			cout << (*it).year << "-" << (*it).month << "-" << (*it).day << endl;
			it++;
		}
	}


	void print_list(const list<int>& lt)
	{
		list<int>::const_iterator it = lt.begin();
		while (it != lt.end())
		{
			//!*it = 2;
			cout << *it << " ";
			it++;
		}
		cout << endl;
	}
	void test_list3()
	{
		list<int> lt;
		lt.push_back(1);
		lt.push_back(2);
		lt.push_back(4);
		lt.push_back(5);
		print_list(lt);

		lt.pop_back();
		lt.pop_front();
		lt.push_front(111);
		lt.push_back(666);
		print_list(lt);

		lt.clear();
		lt.push_back(100);
		print_list(lt);
	}

	void test_list4()
	{
		list<int> lt1;
		lt1.push_back(1);
		lt1.push_back(2);
		lt1.push_back(3);
		lt1.push_back(4);

		list<int> lt2(lt1);
		print_list(lt2);

		list<int> lt3;
		lt3.push_back(10);
		lt3.push_back(20);
		lt3.push_back(30);
		lt3.push_back(40);
		lt1 = lt3;
		print_list(lt1);
	}
}
